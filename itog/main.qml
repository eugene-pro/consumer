import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4


Window {
    visible: true
    width: 400
    height: 500
    title: qsTr("Окно ожидания")
    color: "gainsboro"


     Rectangle {

         id: panel1
         width: parent.width*0.9
         height: parent.height*0.9
         color: "white"
         anchors.centerIn: parent
         opacity: 0.8

         Text {
             id: message
             color: "black"
             text: "Ожидайте подключения устройства..."
             font.family: "Helvetica"
             font.pixelSize: 16
             anchors.centerIn: parent

         }
}
}

