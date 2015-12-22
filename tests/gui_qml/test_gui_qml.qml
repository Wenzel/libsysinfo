import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4

ApplicationWindow {
    id: app
    visible: true
    x: Screen.width / 2 - width / 2
    y: Screen.height / 2 - height / 2
    width: Screen.width * 0.7
    height: Screen.height * 0.7

    TableView {
        anchors.fill: parent
        TableViewColumn {
            role: "pid"
            title: "PID"
            width: 100
        }
        TableViewColumn {
            role: "name"
            title: "Name"
            width: 200
        }
        TableViewColumn {
            role: "cmdline"
            title: "Command line"
            width: app.width
        }
        model: myModel
    }
}

