#include "./download.h"


Download::Download(const QUrl &url, qint64 expectedSize, QIODevicePtr &&output)
	: QObject(nullptr)
	, url(url)
	, expectedSize(expectedSize)
	, mOutput(std::move(output))
	, mReply(nullptr)
	, mCancelled(false)
	, mFinished(false)
{
}

void Download::start(QNetworkAccessManager& networkManager)
{
	if (!mOutput->open(QIODevice::WriteOnly)) {
		emit error(this, QString("Cannot open output to write download %1: %2")
				.arg(url.toString())
				.arg(mOutput->errorString()));
		return;
	}

	QNetworkRequest request;
	request.setUrl(url);
	mReply = networkManager.get(request);
	connect(mReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onError(QNetworkReply::NetworkError)));
	connect(mReply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	connect(mReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(onProgress(qint64,qint64)));
	connect(mReply, SIGNAL(finished()), this, SLOT(onFinished()));
	connect(mReply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(onSslErrors(QList<QSslError>)));
}

void Download::cancel()
{
	if (!mCancelled) {
		mCancelled = true;
		if (mReply) {
			mReply->abort();
		} else {
			emit cancelled(this);
		}
	}
}

void Download::onError(QNetworkReply::NetworkError error)
{
	if (mErrorReason.isEmpty()) {
		mErrorReason = QString("Network error %1: %2").arg(QString::number(error)).arg(mReply->errorString());
	}
	onFinished();
}

void Download::onSslErrors(const QList<QSslError> &errors)
{
	for (const auto &error : errors) {
		qWarning() << "SSL error: " << error;
	}
}

void Download::onReadyRead()
{
	qint64 bytesWritten = mOutput->write(mReply->readAll());
	if (bytesWritten == -1) {
		mErrorReason = QString("Error while writing the download %1 to file: %2")
				.arg(url.toString())
				.arg(mOutput->errorString());
		mReply->abort();
	}
}

void Download::onProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	if (bytesTotal != -1)
		emit this->progress(this, double(bytesReceived) / bytesTotal);
}

void Download::onFinished()
{
	if (mFinished)
		return;

	mOutput->close();
	mReply->deleteLater();
	mFinished = true;

	if (mCancelled) {
		emit cancelled(this);
	} else if (mErrorReason.isEmpty()) {
		emit done(this);
	} else {
		emit error(this,  mErrorReason);
	}

	emit finished(this);
}
