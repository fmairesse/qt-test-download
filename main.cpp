#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QDebug>

#include "downloadmanager.h"

static const QStringList URLS({
	{"https://farm8.staticflickr.com/7321/11218357214_9fe189a4ec_o_d.jpg"},
//	{"http://mirrorblender.top-ix.org/peach/bigbuckbunny_movies/big_buck_bunny_1080p_stereo.ogg"},
	{"https://farm5.staticflickr.com/4140/4878877825_edd5f3bec5_o_d.jpg"},
	{"https://farm9.staticflickr.com/8160/7466201514_59d9dbf7d2_o_d.jpg"},
//	{"http://mirrorblender.top-ix.org/peach/bigbuckbunny_movies/big_buck_bunny_1080p_surround.avi"},
	{"https://farm4.staticflickr.com/3784/12916956124_4010a628f3_o_d.jpg"},
	{"https://farm4.staticflickr.com/3799/9208105211_44f6bbe376_o_d.jpg"},
	{"https://farm6.staticflickr.com/5205/5262879347_3ba0da7e09_o_d.jpg"},
	{"https://farm9.staticflickr.com/8150/7668798224_b0e72761a7_o_d.jpg"},
//	{"http://mirrorblender.top-ix.org/peach/bigbuckbunny_movies/big_buck_bunny_1080p_h264.mov"},
	{"https://farm7.staticflickr.com/6037/6254709876_6243c21f7e_o_d.jpg"},
	{"https://farm6.staticflickr.com/5177/5431896346_24cedcc96e_o_d.jpg"},
	{"https://farm8.staticflickr.com/7306/16274930900_853434d7b9_o_d.jpg"},
	{"https://farm9.staticflickr.com/8252/8509304278_8d44f2a371_o_d.jpg"},
	{"https://farm8.staticflickr.com/7496/15997048851_653a96a3b6_o_d.jpg"},
	{"https://farm6.staticflickr.com/5297/5424148798_8fb4254fd1_o_d.jpg"},
//	{"http://mirrorblender.top-ix.org/peach/bigbuckbunny_movies/big_buck_bunny_1080p_stereo.avi"},
	{"https://farm7.staticflickr.com/6203/6128355587_0df27a8086_o_d.jpg"},
	{"https://farm8.staticflickr.com/7278/7669569658_9d93ca0307_o_d.jpg"},
	{"https://farm8.staticflickr.com/7113/7466197970_2b6d193442_o_d.jpg"},
	{"https://farm6.staticflickr.com/5492/14133945990_402efcc466_o_d.jpg"},
	{"https://farm9.staticflickr.com/8304/7781186636_0819e82f41_o_d.jpg"},
	{"https://farm9.staticflickr.com/8821/17357337821_8271402e24_o_d.jpg"},
	{"https://farm9.staticflickr.com/8374/8509294796_077724bca6_o_d.jpg"},
	{"https://farm9.staticflickr.com/8174/8016458596_a3533ed481_o_d.jpg"},
});

int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);

	QString outPath(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
	qDebug() << "Downloading to " << outPath;

	DownloadManager downloadManager(URLS, outPath);

	QQmlApplicationEngine engine;
	QQmlContext* qmlContext = engine.rootContext();
	qmlContext->setContextProperty("downloadManager", &downloadManager);
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}
