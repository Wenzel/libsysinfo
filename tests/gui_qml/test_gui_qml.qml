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

    TabView {
        id: tab
        anchors.fill: parent

        Tab {
            id: procexp
            title: "Process Explorer"

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
                    width: procexp.width
                }
                model: process_model
            }
        }

        Tab {
            id: socket_unix
            title: "Socket UNIX"

            TableView {
                anchors.fill: parent
                TableViewColumn {
                    role: "num"
                    title: "Num"
                    width: 100
                }
                TableViewColumn {
                    role: "refcount"
                    title: "RefCount"
                    width: 100
                }
                TableViewColumn {
                    role: "protocol"
                    title: "Protocol"
                    width: 100
                }
                TableViewColumn {
                    role: "flags"
                    title: "flags"
                    width: 100
                }
                TableViewColumn {
                    role: "type"
                    title: "Type"
                    width: 100
                }
                TableViewColumn {
                    role: "state"
                    title: "State"
                    width: 100
                }
                TableViewColumn {
                    role: "inode"
                    title: "Inode"
                    width: 100
                }
                TableViewColumn {
                    role: "path"
                    title: "Path"
                    width: socket_unix.width
                }
                model: socket_unix_model
            }
        }
    }
}

