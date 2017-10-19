#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QDebug>

#include "downloadmanager.h"

static const QStringList URLS({
	// Always fail on WinRT
	{"http://localhost:8000/BigBuckBunny_640x360.m4v"},

	// Fails randomly on WinRT. Sometimes succeeds but file is incomplete or corrupted
	// This seems to be relative to the network configuration of the place I work at
	// because this works much better when trying from home.
	// This always work when using mingw or msvc (not UWP).
	{"https://test.path4flight.com/tiles/openmaptiles/openmaptiles.mbtiles"},

	// Always succeeds
	{"https://farm8.staticflickr.com/7321/11218357214_9fe189a4ec_o_d.jpg"},
	{"https://download.blender.org/peach/bigbuckbunny_movies/BigBuckBunny_640x360.m4v"},
	{"https://farm5.staticflickr.com/4140/4878877825_edd5f3bec5_o_d.jpg"},
	{"https://farm9.staticflickr.com/8160/7466201514_59d9dbf7d2_o_d.jpg"},
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
