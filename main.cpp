#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>
#include <QDebug>

#include "downloadmanager.h"

static const DownloadRequests REQUESTS({
	// Always fail on WinRT
	// To serve the file, download it from https://download.blender.org/peach/bigbuckbunny_movies/BigBuckBunny_640x360.m4v
	// then from your download directory, run python -m SimpleHTTPServer
	{"http://localhost:8000/BigBuckBunny_640x360.m4v", 121283919},

	// Always fail on WinRT
	// To serve the file, create a new text file that contains "foo"
	// and serve it with python SimpleHTTPServer
	{"http://localhost:8000/foo.txt", 3},

	// Fails randomly on WinRT. Sometimes succeeds but file is incomplete or corrupted
	// This seems to be relative to the network configuration of the place I work at
	// because this works much better when trying from home.
	// This always work when using mingw or msvc (not UWP).
	{"https://test.path4flight.com/tiles/openmaptiles/openmaptiles.mbtiles", 143708160},

	// Always succeeds
	{"https://farm8.staticflickr.com/7321/11218357214_9fe189a4ec_o_d.jpg", 2630725},
	{"https://download.blender.org/peach/bigbuckbunny_movies/BigBuckBunny_640x360.m4v", 121283919},
	{"https://farm5.staticflickr.com/4140/4878877825_edd5f3bec5_o_d.jpg", 269471},
	{"https://farm9.staticflickr.com/8160/7466201514_59d9dbf7d2_o_d.jpg", 2663425},
});

int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);

	QString outPath(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
	qDebug() << "Downloading to " << outPath;

	DownloadManager downloadManager(REQUESTS, outPath);

	QQmlApplicationEngine engine;
	QQmlContext* qmlContext = engine.rootContext();
	qmlContext->setContextProperty("downloadManager", &downloadManager);
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}
