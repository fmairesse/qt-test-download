#ifndef QTILESERVER_UPDATER_DOWNLOAD_PIMPL_H
#define QTILESERVER_UPDATER_DOWNLOAD_PIMPL_H

#include <memory>

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslError>


typedef std::unique_ptr<QIODevice> QIODevicePtr;

class Download : public QObject
{
	Q_OBJECT
public:
	const QUrl url;

	explicit Download(const QUrl &sourceUrl, QIODevicePtr &&output);
	void start(QNetworkAccessManager &);
	void cancel();

signals:
	void error(Download *, const QString &reason);
	void progress(Download *, double);
	void done(Download *);
	void cancelled(Download *);
	//! Called when cancelled, error or done
	void finished(Download *);

private slots:
	void onError(QNetworkReply::NetworkError);
	void onReadyRead();
	void onProgress(qint64 bytesReceived, qint64 bytesTotal);
	void onFinished();
	void onSslErrors(const QList<QSslError> &);

private:
	QIODevicePtr mOutput;
	QNetworkReply *mReply;
	bool mCancelled;
	//! To remember whether we already sent the finished signal.
	//! Because from doc of QNetworkReply::error: "The finished() signal will probably follow"
	bool mFinished;
	QString mErrorReason;
};

#endif // DOWNLOAD__H
