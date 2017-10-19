#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QBuffer>
#include <QNetworkConfiguration>

#include "downloadmanager.h"
#include "download.h"


DownloadRequest::DownloadRequest(const QString &url, qint64 size)
	: url(QUrl::fromUserInput(url))
	, size(size)
{
}


DownloadManager::DownloadManager(const DownloadRequests &requests, const QString &outDirPath)
	: QObject(nullptr)
	, mOutDirPath(outDirPath)
	, mIsParallel(false)
{
	for (const DownloadRequest &request : requests) {
		QFile *outFile = new QFile(this->urlToPath(request.url));
		if (outFile->exists()) {
			outFile->remove();
		}
		Download *download = new Download(request.url, request.size, QIODevicePtr(outFile));
//		Download *download = new Download(url, QIODevicePtr(new QBuffer(&mBuffer)));

		connect(download, SIGNAL(error(Download*,QString)), this, SLOT(onDownloadError(Download*,QString)));
		connect(download, SIGNAL(done(Download*)), this, SLOT(onDownloadDone(Download*)));
		connect(download, SIGNAL(finished(Download*)), this, SLOT(onDownloadFinished(Download*)));
		connect(download, SIGNAL(progress(Download*,double)), this, SLOT(onDownloadProgress(Download*,double)));

		mWaitingDownloads.append(download);
	}

	qDebug() << "Timeout="<< mNetworkAccessManager.configuration().connectTimeout();
}

QString DownloadManager::urlToPath(const QUrl &url)
{
	QDir outDir(mOutDirPath);
	return outDir.absoluteFilePath(QUrl::toPercentEncoding(url.path()));
}

void DownloadManager::start()
{
	if (mWaitingDownloads.isEmpty())
		return;

	if (mIsParallel) {
		while (!mWaitingDownloads.isEmpty()) {
			startDownload(mWaitingDownloads.dequeue());
		}
	} else {
		startDownload(mWaitingDownloads.dequeue());
	}
}

void DownloadManager::startDownload(Download *download)
{
	qDebug() << "Starting download: " << download->url.toString();
	mPendingDownloads.insert(download);
	download->start(mNetworkAccessManager);
}

bool DownloadManager::isParallel() const
{
	return mIsParallel;
}

void DownloadManager::setIsParallel(bool isParallel)
{
	qDebug() << "Setting paralled to " << isParallel;
	mIsParallel = isParallel;
	emit isParallelChanged(isParallel);
}

int DownloadManager::remainingDownloads() const
{
	return mWaitingDownloads.size() + mPendingDownloads.size();
}

void DownloadManager::onDownloadError(Download *download, const QString &reason)
{
	qWarning() << "Download error: " << download->url.toString() << ", " << reason;
	emit error(download->url.toString(), reason);
}

void DownloadManager::onDownloadDone(Download *download)
{
	qDebug() << "Download done: " << download->url.toString();
	QFileInfo downloadedFileInfo(this->urlToPath(download->url));
	const qint64 realSize = downloadedFileInfo.size();
	if (realSize == download->expectedSize) {
		emit downloaded(download->url.toString());
	} else {
		emit error(download->url.toString(),
			QString("Wrong size: expected %1, got %2").arg(download->expectedSize).arg(realSize));
	}
}

void DownloadManager::onDownloadProgress(Download *download, double progress)
{
	//qDebug() << "Download progress: " << download->url.toString() << "  " << progress;
}

void DownloadManager::onDownloadFinished(Download *download)
{
	mPendingDownloads.remove(download);
	emit remainingDownloadsChanged();

	if (!mWaitingDownloads.isEmpty()) {
		startDownload(mWaitingDownloads.dequeue());
	} else if (mPendingDownloads.isEmpty()) {
		emit done();
	}

	delete download;
}
