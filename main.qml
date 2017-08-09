import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 2.2;

Window {
	property bool started : false;

	visible: true
	width: 640
	height: 480
	title: qsTr("Hello World")

	Column {
		anchors.centerIn: parent
		visible: !started
		Button {
			text: "Start"
			onClicked: {
				downloadManager.start();
				started = true;
			}
		}

		Row {
			CheckBox {
				id: parallelCheckbox
				checked: downloadManager.isParallel
				onClicked: {
					downloadManager.isParallel = !downloadManager.isParallel;
				}
			}
			Text {
				anchors.verticalCenter: parallelCheckbox.verticalCenter
				text: "Parallel downloads"
			}
		}
	}


	Item {
		visible: started
		anchors.fill: parent

		Text {
			id: statusText
			text: "Remaining downloads: " + downloadManager.remainingDownloads;
			height: 30
		}

		ListModel {
			id: finishedDownloads
		}

		ListView {
			model: finishedDownloads
			anchors.top: statusText.bottom
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.bottom: parent.bottom
			delegate : Text {
				text: url
				anchors.left: parent.left
				anchors.right: parent.right
				color: success ? "green" : "red"
			}
		}
	}

	Connections {
		target: downloadManager
		onDownloaded: {
			finishedDownloads.append({'url': url, 'success': true});
		}
		onError: {
			finishedDownloads.append({'url': url, 'success': false});
		}
	}
}
