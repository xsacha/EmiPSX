// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: buttonConfigObj
    color: "transparent"
    border.color: "black"
    property string image
    property bool stretch: false
    MouseArea {
        anchors.fill: parent
        drag {
            target: buttonConfigObj
            axis: Drag.XandYAxis
            minimumX: 0; maximumX: buttonConfigObj.parent.width - buttonConfigObj.width
            minimumY: 0; maximumY: buttonConfigObj.parent.height - buttonConfigObj.height
        }
    }
    Image {
        fillMode: stretch ? Image.Stretch : Image.PreserveAspectFit
        anchors.fill: parent
        source: "../"+image
    }
}
