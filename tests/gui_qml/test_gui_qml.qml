import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.1

ApplicationWindow {
    visible: true
    x: Screen.width / 2 - width / 2
    y: Screen.height / 2 - height / 2
    width: Screen.width * 0.7
    height: Screen.height * 0.7


    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }

    Text {
        text: qsTr("Hello World")
        anchors.centerIn: parent
    }
}

