import QtQuick 2.7
import QtQuick.XmlListModel 2.0
import QtQuick.Layouts 1.11
import EnvVarModule 1.0

ListView {
    id: horList
    property string parentId
    spacing: 3
    anchors.left: parent.left
    anchors.right: parent.right
    height: 24
    orientation: ListView.Horizontal

    model: XmlListModel {
        id: xmlModelHor
        source: "file:///" + EnvVar.value("USERPROFILE") + "/config.xml"
        query: "/body/set[@id='" + parentId + "']/item"

        XmlRole { name: "address"; query: "address/string()" }
        XmlRole { name: "title"; query: "title/string()" }
    }

    delegate: Rectangle {
        id: itemsRect
        width: rowsLayout.implicitWidth
        height: horList.height
        smooth: true
        radius: 12
        color: "white"

        RowLayout {
            id: rowsLayout
            anchors.fill: parent

            Rectangle {
                width: 1
                color: "white"
            }

            TextMetrics {
                id: txtMeter
                font.family: "Courier"
                font.pixelSize: 14
                text: title
            }

            Text {
                font: txtMeter.font
                text: txtMeter.text
            }

            property string state: "n/a"

            Image {
                id: stateIcon
                mipmap: true
                sourceSize.height: txtMeter.height * 1.1
                fillMode: Image.PreserveAspectFit
                source: "qrc:/emblem-unreadable.png"

                Timer {
                    interval: 3000
                    running: true
                    repeat: true
                    triggeredOnStart : true
                    onTriggered: {
                        var http = new XMLHttpRequest()
                        var url = "http://" + address +"/";
                        http.open("GET", url, true)

                        http.onreadystatechange = function() {
                            if (http.readyState == 4) {
                                if (http.status == 200) {
                                    switch (http.responseText) {
                                    case "1": stateIcon.source = "qrc:/emblem-important-4.png"
                                        break
                                    case "0": stateIcon.source = "qrc:/emblem-default.png"
                                        break
                                    }
                                } else {
                                    stateIcon.source = "qrc:/emblem-unreadable.png"
                                }
                            }
                        }
                        http.send();
                    }
                }
            }

            Rectangle {
                width: 1
                color: "white"
            }
        }
    }
}
