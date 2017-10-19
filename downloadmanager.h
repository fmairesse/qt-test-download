#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QStringList>
#include <QQueue>
#include <QSet>
#include <QNetworkAccessManager>
#include <QByteArray>

class Download;

struct DownloadRequest
{
	QUrl url;
	qint64 size;

	DownloadRequest(const QString &, qint64);
};

typedef QList<DownloadRequest> DownloadRequests;

class DownloadManager : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool isParallel READ isParallel WRITE setIsParallel NOTIFY isParallelChanged)
	Q_PROPERTY(int remainingDownloads READ remainingDownloads NOTIFY remainingDownloadsChanged)
public:
	explicit DownloadManager(const DownloadRequests &, const QString &outDirPath);

	Q_INVOKABLE void start();

	bool isParallel() const;
	void setIsParallel(bool);

	int remainingDownloads() const;

signals:
	//! Emitted when the download for the given URL failed
	void error(const QString &url, const QString &reason);

	//! Emitted when the download for the given URL succeeded
	void downloaded(const QString &url);

	//! Emitted when all downloads have finished
	void done();

	void isParallelChanged(bool);
	void remainingDownloadsChanged();

private slots:
	void onDownloadError(Download *download, const QString &reason);
	void onDownloadDone(Download *download);
	void onDownloadFinished(Download *download);
	void onDownloadProgress(Download *download, double);

private:
	const QString mOutDirPath;
	QNetworkAccessManager mNetworkAccessManager;
	QQueue<Download*> mWaitingDownloads;
	QSet<Download*> mPendingDownloads;
	bool mIsParallel;
	QByteArray mBuffer;

	void startDownload(Download *);
	QString urlToPath(const QUrl &);
};

#endif // DOWNLOADMANAGER_H
