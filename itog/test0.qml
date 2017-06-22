import QtQuick 2.5; import QtQuick.Window 2.2; import QtQuick.Controls 1.4; Window { visible: true; width: 400; height: 500; title: qsTr("Кофемашина"); color: "maroon"; Rectangle { id: panel1; width: parent.width*0.9; height: parent.height*0.9; color: "white"; anchors.centerIn: parent; opacity: 0.8; Text { id: message; color: "black"; text: "Выберите напиток!"; font.family: "Helvetica"; font.pixelSize: 25; anchors.horizontalCenter: parent.horizontalCenter; y:panel1.height*0.06 } Image { anchors.centerIn: panel1; source: "coff.png"; } MouseArea { id: mouseArea; anchors.rightMargin: 0; anchors.bottomMargin: 0; anchors.leftMargin: 0; anchors.topMargin: 0; anchors.fill: parent; Button { id: button1; onClicked: { _Handler.callFunc("Эспресо"); } x:panel1.width*0.1; y:panel1.height*0.3; width: 130; height: 40; text: qsTr("Эспресо") } Button { id: button2; onClicked: { _Handler.callFunc("Латте"); } x:panel1.width*0.1; y:panel1.height*0.5; width: 130; height: 40; text: qsTr("Латте") } Button { id: button3; onClicked: { _Handler.callFunc("Американо"); } x:panel1.width*0.1; y:panel1.height*0.7; width: 130; height: 40; text: qsTr("Американо") } Button { id: button4; onClicked: { _Handler.callFunc("Капучино"); } x:panel1.width*0.5; y:panel1.height*0.3; width: 130; height: 40; text: qsTr("Капучино") } Button { id: button5; onClicked: { _Handler.callFunc("Марокино"); } x:panel1.width*0.5; y:panel1.height*0.5; width: 130; height: 40; text: qsTr("Марокино") } Button { id: button6; onClicked: { _Handler.callFunc("Шоколад"); } x:panel1.width*0.5; y:panel1.height*0.7; width: 130; height: 40; text: qsTr("Шоколад") } } } }