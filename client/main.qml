import QtQuick 2.7
import QtQuick.XmlListModel 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.0
import EnvVarModule 1.0


ApplicationWindow {
    property string appTitle: "BusyPinger"

    id: window
    color: "lightgray"
    visible: false
    width: 480
    height: 480
    title: qsTr(appTitle)

    XmlListModel {
        id: xmlModel
        source: "file:///" + EnvVar.value("USERPROFILE") + "/config.xml"
        query: "/body/set"

        XmlRole { name: "setid"; query: "@id/string()" }
    }

    ListView {
        id: mainList
        spacing: 3
        anchors.fill: parent
        model: xmlModel
        delegate: MyListDelegate {
            parentId: setid
        }
    }

    SystemTrayIcon {
        visible: true
        iconSource: "qrc:/utilities-system-monitor-4.ico"
        tooltip: appTitle

        menu: Menu {
            MenuItem {
                text: qsTr("Quit")
                onTriggered: Qt.quit()
            }
        }

        onActivated: {
            if(reason !== SystemTrayIcon.Context) {
                if(window.visibility)
                {
                    window.hide()
                } else {
                    window.show()
                    window.raise()
                    window.requestActivate()
                }
            }
        }
    }

    onClosing: {
        close.accepted = false
        window.hide()
    }

}
