// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.symbian 1.1    // Symbian components
import "UI" 1.0 as UI

Rectangle {
    id: menuBase
    width: 640
    height: 360
    gradient: Gradient {
        GradientStop { position: 0; color: "#69acdb" }
        GradientStop { position: 1; color: "#0b1220" }
    }
    objectName: "mainMenu"
    property bool active: true
    signal romLoaded

    function updateimgs() { menuBase.romLoaded() }  // slot
    property string romdir: "E:/PSX"
    property string savedir: "E:/PSX"
    property int activeTab: 1

    Button {
        text: activeTab == 1 ? "X" : "<"
        font.pixelSize: 20
        z: 100;
        onClicked: activeTab == 1 ? emu.exitEmu() : activeTab = 1;
    }
    Item {
        id: tab1
        x: 0
        y: -parent.height
        width: parent.width
        height: parent.height
        state: "active"
        Column {
            x: 40; y: 30
            spacing: 15
            Button {
                text: "Load New Rom"; width: 220
                onClicked: emu.getRomFile()
            }
            Button {
                text: "Continue Game"; width: 220
                onClicked: emu.emuContinue()
            }
            Button {
                text: "Reset"; width: 220
                onClicked: emu.resetEmu()
            }
            Button {
                text: "Configuration"; width: 220
                onClicked: menuBase.activeTab = 2
            }
            Button {
                text: "About"; width: 220
                onClicked: menuBase.activeTab = 3
            }
        }
        Column {
            x: 536; y: 96
            spacing: 150
            Button {
                id: btnLoadState
                text: "Load"
                z: 1
                onClicked: emu.loadState()
            }

            Button {
                id: btnSaveState
                text: "Save"
                z: 1
                onClicked: emu.saveState()
            }
        }

        PathView {
            id: path_view1
            objectName: "savePathView"
            x: 338; y: 32
            width: 276; height: 261
            path: Path {
                startX: 230; startY: 0
                PathAttribute { name: "iconScale"; value: 0.5 }
                PathAttribute { name: "iconOpacity"; value: 0.5 }
                PathAttribute { name: "iconZ"; value: -1 }
                PathQuad {
                    x: 100; y: 65
                    controlX: 65; controlY: 30
                }
                PathAttribute { name: "iconScale"; value: 1.0 }
                PathAttribute { name: "iconOpacity"; value: 1.0 }
                PathAttribute { name: "iconZ"; value: 1 }
                PathQuad {
                    x: 230; y: 261
                    controlX: 65; controlY: 195
                }
                PathQuad {
                    x: 230; y: 0
                    controlX: 230; controlY: 130
                }
            }
            delegate: Component {
                Item{
                    width: 220; height: 220
                    scale: PathView.iconScale
                    opacity: PathView.iconOpacity
                    z: PathView.iconZ
                    Column {
                        spacing: 1
                        Text { text: "Slot " + (currentImage.realIndex + 1); font.pointSize: 16 }
                        Rectangle {
                            width: 210; height: 210
                            color: colorCode
                            anchors.horizontalCenter: parent.horizontalCenter
                            Image{
                                id: currentImage
                                property int realIndex: (index >= 2 ? index - 2 : index + 4)
                                cache: false;
                                anchors.centerIn: parent;
                                width: 200;
                                height: 200;
                                source: "image://saves/" + currentImage.realIndex;
                                Connections {
                                    target: menuBase
                                    onRomLoaded:{ //update images
                                        currentImage.source = "";
                                        currentImage.source = "image://saves/" + currentImage.realIndex;
                                    }

                                }
                            }

                        }
                    }
                }
            }
            model: ListModel {
                ListElement { colorCode: "white" }
                ListElement { colorCode: "red" }
                ListElement { colorCode: "blue" }
                ListElement { colorCode: "green" }
                ListElement { colorCode: "gray" }
                ListElement { colorCode: "yellow" }
            }
            onMovementEnded: { emu.setGameState( currentIndex ); romLoaded(); }
        }

        Rectangle {
            id: rectangle1
            x: 343
            y: 26
            width: 255
            height: 48
            color: "#64362d2d"
            radius: 20
            border.width: 2
            border.color: "#2f2b2b"

            Text {
                x: 13; y: 2
                width: 229; height: 46
                text: qsTr("State Selection")
                font.italic: false
                verticalAlignment: Text.AlignVCenter
                styleColor: "#f5e8e8"
                z: 6
                font.pixelSize: 35
                style: Text.Outline
                font.underline: false
                font.bold: true
                font.family: "Times New Roman"
                horizontalAlignment: Text.AlignHCenter
            }
        }
        states:
            State {
            name: "active"; when: menuBase.activeTab == 1;
            PropertyChanges { target: tab1; x: 0; y: 0 }
        }
        transitions:
            Transition {
            NumberAnimation { properties: "x,y"; duration: 400; easing.type: Easing.InOutQuad }
        }
    }

    Item {
        id: tab2
        x: parent.width*2
        width: parent.width; height: parent.height
        Button {
            id: buttonconfig1
            x: 271; y: 190; z: 10
            width: 100; height: 43
            text: "Save"
            onClicked: {
                menuBase.activeTab = 1
                emu.saveVirtualKeys()
            }
        }

        UI.ButtonConfig {
            objectName: "menubtn"
            x: 280; y: 10
            width: 80; height: 40
            image: "mbutton.png"
            stretch: true
        }
        UI.ButtonConfig {
            objectName: "ybtn"
            x: 450; y: 240
            width: 60; height: 60
            image: "buttonS.png"
        }
        UI.ButtonConfig {
            objectName: "bbtn"
            x: 510; y: 290
            width: 60; height: 60
            image: "buttonX.png"
        }
        UI.ButtonConfig {
            objectName: "xbtn"
            x: 510; y: 190
            width: 60; height: 60
            image: "buttonT.png"
        }
        UI.ButtonConfig {
            objectName: "abtn"
            x: 570; y: 240
            width: 60; height: 60
            image: "buttonC.png"
        }
        UI.ButtonConfig {
            objectName: "lbtn"
            x: 10; y: 10
            width: 70; height: 30
            image: "lbutton.png"
            stretch: true
        }

        Rectangle {
            id: padrect
            x: 5; y: 170
            width: 180; height: 180
            color: "transparent"
            radius: 4
            border.color: "black"
            objectName: "dpadbtn"
            MouseArea {
                anchors.fill: parent
                drag {
                    target: padrect
                    axis: Drag.XandYAxis
                    minimumX: 0; maximumX: padrect.parent.width - padrect.width
                    minimumY: 0; maximumY: padrect.parent.height - padrect.height
                }
            }
            Rectangle {
                id: downrect
                x: 0; y: 120
                width: 180; height: 60
                color: "transparent"
                objectName:  "downbtn"
                Image {
                    id: image1
                    y: 120
                    width: 107; height: 82
                    anchors.fill: parent
                    source: "setaDOWN.png"
                    fillMode: Image.PreserveAspectFit
                }
            }

            Rectangle {
                id: rightrect
                x: 120; y: 0
                width: 60; height: 180
                color: "transparent"
                objectName:  "rightbtn"
                Image {
                    id: image3
                    anchors.fill: parent
                    source: "setaRIGHT.png"
                    fillMode: Image.PreserveAspectFit
                }
            }

            Rectangle {
                id: uprect
                width: 180
                height: 60
                color: "#00000000"
                objectName:  "upbtn"
                Image {
                    id: image4
                    anchors.fill: parent
                    source: "setaUP.png"
                    fillMode: Image.PreserveAspectFit
                }
            }

            Rectangle {
                id: leftrect
                width: 60
                height: 180
                color: "#00000000"
                objectName:  "leftbtn"
                visible: true
                Image {
                    id: image5
                    anchors.fill: parent
                    source: "setaLEFT.png"
                    fillMode: Image.PreserveAspectFit
                }
            }
        }
        UI.ButtonConfig {
            objectName: "stbtn"
            x: 320; y: 300
            width: 70; height: 20
            image: "stbutton.png"
            stretch: true
        }
        UI.ButtonConfig {
            objectName: "slbtn"
            x: 230; y: 300
            width: 70; height: 20
            image: "slbutton.png"
            stretch: true
        }
        UI.ButtonConfig {
            objectName: "rbtn"
            x: 560; y: 10
            width: 70; height: 30
            image: "rbutton.png"
            stretch: true
        }

        states:
            State {
            name: "active"; when: menuBase.activeTab == 7;
            PropertyChanges { target: tab2; x: 0; y: 0 }
            //PropertyChanges { target: statusbar1; visible: false }
        }
        transitions:
            Transition {
            NumberAnimation { properties: "x,y"; duration: 400; easing.type: Easing.InOutQuad }
        }
    }
    Item {
        id: tab3
        y: parent.height
        width: parent.width; height: parent.height

        Text {
            id: textabout
            x: 31; y: 67
            width: 578; height: 272
            font.pixelSize: 23
            font.family: "Times New Roman"
            text: "This is an open source emulator built with Qt Toolkit.\n
                Revived by Andre Botelho.\n
                http://libproductions.wordpress.com/emisnes\n
                Fixed by Sacha Refshauge"
        }
        states:
            State {
            name: "active"; when: menuBase.activeTab == 3;
            PropertyChanges { target: tab3; x: 0; y: 0 }
        }
        transitions:
            Transition {
            NumberAnimation { properties: "x,y"; duration: 400; easing.type: Easing.InOutQuad }
        }
    }

    Item {
        id: tab4
        x: parent.width
        width: parent.width; height: parent.height

        Column {
            x: 40; y: 30
            spacing: 20
            Button {
                text: "Config Audio"
                platformAutoRepeat: false
                onClicked: menuBase.activeTab = 6
            }
            Button {
                text: "Config Video"
                onClicked: menuBase.activeTab = 5
            }
            Button {
                text: "Config Virtual Control"
                onClicked: menuBase.activeTab = 7
            }
            Button {
                text: "Save Configuration"
                onClicked: {
                    menuBase.activeTab = 1
                    emu.saveConfig()
                }
            }
        }

        Rectangle{
            id: warnRect
            width: 320
            height: 180
            visible: false
            radius: 30
            color: "#f0cccccc"
            border.width: 5
            border.color: "#f0000000"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            property string texto: "Press a Key for Up"
            property int counter: 0
            x: 154; y: 85
            z: 1
            Text{
                text: warnRect.texto
                anchors.fill: parent
                font.bold: false
                font.pointSize: 8
                font.family: "Tahoma"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            Text{
                text: "Click to Skip"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10
                font.bold: false
                font.pointSize: 8
                font.family: "Tahoma"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            Keys.onPressed: {
                emu.setKeys(counter, event.key)
                texto = "Press a key for ";
                counter++;
                switch(counter)
                {
                case 1: texto += "Down"; break;
                case 2: texto += "Left"; break;
                case 3: texto += "Right"; break;
                case 4: texto += "A"; break;
                case 5: texto += "B"; break;
                case 6: texto += "X"; break;
                case 7: texto += "Y"; break;
                case 8: texto += "L"; break;
                case 9: texto += "R"; break;
                case 10: texto += "Start"; break;
                case 11: texto += "Select"; break;
                default: warnRect.visible = false; warnRect.focus = false;
                    texto += "Up"; counter = 0; break;
                }
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    warnRect.counter++
                    switch(warnRect.counter)
                    {
                    case 1: texto += "Down"; break;
                    case 2: texto += "Left"; break;
                    case 3: texto += "Right"; break;
                    case 4: texto += "A"; break;
                    case 5: texto += "B"; break;
                    case 6: texto += "X"; break;
                    case 7: texto += "Y"; break;
                    case 8: texto += "L"; break;
                    case 9: texto += "R"; break;
                    case 10: texto += "Start"; break;
                    case 11: texto += "Select"; break;
                    default: warnRect.visible = false; warnRect.focus = false;
                        texto += "Up"; counter = 0; break;
                    }
                }
            }

        }
        Button {
            id: buttonkeyconfig
            x: 324; y: 296
            width: 217; height: 43
            text: "Key Config"
            property int i;
            onClicked:{
                warnRect.visible = true;
                warnRect.focus = true;
            }
        }

        Text {
            id: text9
            x: 275
            y: 15
            width: 171
            height: 25
            color: "#ffffff"
            text: "Rom Directory"
            font.pixelSize: 18
            font.family: "Times New Roman"

            Button {
                id: buttonromdir
                x: -13
                y: 36
                width: 317
                height: 50
                text: menuBase.romdir
                transformOrigin: Item.Center
                clip: false
                rotation: 0
                platformAutoRepeat: false
                opacity: 1
                onClicked: emu.chooseDir(0);
            }
        }

        Text {
            id: text10
            x: 275
            y: 113
            width: 171
            height: 25
            color: "#ffffff"
            text: "Save Directory"
            font.pixelSize: 18
            font.family: "Times New Roman"

            Button {
                id: buttonsavedir
                x: -13; y: 31
                width: 317; height: 50
                text: menuBase.savedir
                platformAutoRepeat: false
                onClicked: emu.chooseDir(1)
            }
        }

        Text {
            id: text1
            x: 310; y: 209
            width: 226; height: 34
            color: "white"
            text: "Virtual Control Opacity"
            font.pixelSize: 21
            Slider {
                id: vtransSlider
                x: -7
                y: 41
                width: 205
                height: 36
                minimumValue: 0
                orientation: 1
                value: 7
                maximumValue: 9
                stepSize: 1
                valueIndicatorVisible: false
                objectName: "vctranslider"
            }
            font.family: "Times New Roman"
        }
        states: [
            State {
                name: "active"
                PropertyChanges {
                    target: tab4
                    x: "0"
                    y: "0"
                }
                when: menuBase.activeTab == 2
            }]
        transitions: [
            Transition {
                NumberAnimation {
                    properties: "x,y"
                    easing.type: "InOutQuad"
                    duration: 400
                }
            }]
    }

    Item {
        id: tabconfigvideo
        x: parent.width; y: -parent.height
        width: parent.width; height: parent.height

        Text {
            id: text4
            x: 126
            y: 22
            width: 342
            height: 42
            text: qsTr("Video Configuration")
            font.pixelSize: 30
            style: Text.Raised
            font.underline: false
            font.bold: false
            font.family: "Times New Roman"
            horizontalAlignment: Text.AlignHCenter
        }

        CheckBox {
            id: frameBox
            x: 58; y: 267
            text: "Show Framerate"
            objectName: "frameBox"
        }

        CheckBox {
            id: mdecBox
            x: 472; y: 73
            text: "Mdec Enable"
            objectName: "mdecBox"
        }

        CheckBox {
            id: sblitBox
            x: 325; y: 190
            text: "Skip Blit(spd hack)"
            objectName: "sblitBox"
        }

        Text {
            id: text5
            x: 40
            y: 85
            width: 103
            height: 30
            color: "#ffffff"
            text: qsTr("Image Size")
            font.pixelSize: 23
            SelectionListItem {
                id: imSize
                x: 107
                y: -15
                width: 172
                height: 61
                SelectionDialog {
                    id: isizedial
                    x: 6
                    y: 34
                    width: 352
                    height: 180
                    titleText: "Select image size:"
                    model: ListModel {
                        ListElement {
                            name: "Normal"
                        }

                        ListElement {
                            name: "FullScreen"
                        }
                    }
                    objectName: "isizedial"
                    selectedIndex: 0
                }
                title: isizedial.model.get(isizedial.selectedIndex).name
                objectName: "imSize"
                onClicked: isizedial.open()
            }
            font.family: "Times New Roman"
        }

        Text {
            id: text6
            x: 40
            y: 149
            width: 103
            height: 30
            color: "#ffffff"
            text: qsTr("Frameskip")
            font.pixelSize: 23
            font.family: "Times New Roman"

            SelectionListItem {
                id: framerate
                x: 108; y: -16
                width: 172; height: 61
                SelectionDialog {
                    id: framedial
                    x: -37; y: -128
                    width: 322; height: 227
                    titleText: "Select Frameskip:"
                    model: ListModel {
                        ListElement { name: "Auto" }
                        ListElement { name: "0" }
                        ListElement { name: "1" }
                        ListElement { name: "2" }
                        ListElement { name: "3" }
                        ListElement { name: "4" }
                    }
                    objectName: "fskipdial"
                    selectedIndex: 0
                }
                title: framedial.model.get(framedial.selectedIndex).name
                objectName: "framerate"
                onClicked: framedial.open()
            }
        }

        CheckBox {
            id: smoothBox
            x: 325
            y: 137
            text: "Smooth Image"
            objectName: "smoothBox"
        }

        CheckBox {
            id: sgpuBox
            x: 325
            y: 73
            text: "Skip GPU"
            objectName: "sgpuBox"
        }

        CheckBox {
            id: lightBox
            x: 325
            y: 245
            text: "Light"
            objectName: "lightBox"
        }

        CheckBox {
            id: blendBox
            x: 480
            y: 245
            text: "Blend"
            objectName: "blendBox"
        }
        states: [
            State {
                name: "active"
                PropertyChanges {
                    target: tabconfigvideo
                    x: "0"
                    y: "0"
                }
                when: menuBase.activeTab == 5
            }]
        transitions: [
            Transition {
                NumberAnimation {
                    properties: "x,y"
                    easing.type: "InOutQuad"
                    duration: 400
                }
            }]
    }

    Item {
        id: tabconfigaudio
        x: parent.width; y: parent.height
        width: parent.width; height: parent.height

        Text {
            id: text2
            x: 333; y: 112
            width: 135; height: 30
            color: "white"
            text: qsTr("Volume")
            font.bold: false
            font.pixelSize: 22
            font.family: "Times New Roman"

            Slider {
                id: volumeSlider
                x: 77; y: -3
                width: 220; height: 36
                orientation: 1
                minimumValue: 0
                value: 70
                maximumValue: 100
                stepSize: 1
                valueIndicatorVisible: false
                objectName: "volumeSlider"
                onValueChanged: {
                    if( !volumeSlider.pressed) emu.changeVolume(value)
                }
            }
        }

        CheckBox {
            id: senaBox
            x: 65; y: 124
            text: "Enable Sound"
            objectName: "senaBox"
        }

        CheckBox {
            id: cdaBox
            x: 65; y: 176
            text: "CDDA"
            objectName: "cdaBox"
        }

        CheckBox {
            id: xaBox
            x: 183; y: 176
            text: "XA"
            objectName: "xaBox"
        }

        Text {
            id: text3
            x: 166; y: 70
            width: 309; height: 42
            text: qsTr("Audio & Misc")
            font.pixelSize: 30
            style: Text.Raised
            font.underline: false
            font.bold: false
            font.family: "Times New Roman"
            horizontalAlignment: Text.AlignHCenter
        }

        Text {
            id: text7
            x: 314; y: 165
            width: 116; height: 30
            color: "#f5f2f2"
            text: qsTr("Sound  Rate")
            font.pixelSize: 23
            font.family: "Times New Roman"

            SelectionListItem {
                id: sndRate
                x: 131
                y: -16
                width: 185
                height: 61
                SelectionDialog {
                    id: sratedial
                    x: -309; y: -128
                    width: 338; height: 290
                    visible: false
                    titleText: "Select Sound Rate:"
                    model: ListModel {
                        ListElement { name: "8000" }
                        ListElement { name: "11025" }
                        ListElement { name: "22050" }
                        ListElement { name: "44100" }
                    }
                    objectName: "sratedial"
                    selectedIndex: 0
                }
                title: sratedial.model.get(sratedial.selectedIndex).name
                objectName: "sndRate"
                onClicked: sratedial.open()
            }
        }

        Text {
            id: text13
            x: 262; y: 296
            width: 116; height: 30
            color: "#f5f2f2"
            text: qsTr("BIAS")
            font.pixelSize: 23
            SelectionListItem {
                id: bias
                x: 57; y: -15
                width: 103
                height: 61
                SelectionDialog {
                    id: biasDial
                    x: -309; y: -248
                    width: 338; height: 290
                    titleText: "Select BIAS Value:"
                    visible: false
                    model: ListModel {
                        ListElement { name: "Auto" }
                        ListElement { name: "1" }
                        ListElement { name: "2" }
                        ListElement { name: "3" }
                        ListElement { name: "4" }
                        ListElement { name: "5" }
                    }
                    objectName: "biasDial"
                    selectedIndex: 2
                }
                title: biasDial.model.get(biasDial.selectedIndex).name
                objectName: "cpu"
                onClicked: biasDial.open()
            }
            font.family: "Times New Roman"
        }

        CheckBox {
            id: flimBox
            x: 65; y: 284
            text: "Frame Limit"
            objectName: "flimBox"
        }

        CheckBox {
            id: biosBox
            x: 65; y: 231
            text: "Emulate PSX Bios"
            objectName: "biosBox"
        }

        Text {
            id: text14
            x: 424; y: 296
            width: 116; height: 30
            color: "#f5f2f2"
            text: qsTr("Clock")
            font.pixelSize: 23
            SelectionListItem {
                id: clocklist
                x: 57; y: -15
                width: 157; height: 61
                title: clockDial.model.get(clockDial.selectedIndex).name
                SelectionDialog {
                    id: clockDial
                    x: -309
                    y: -248
                    width: 338
                    height: 290
                    titleText: "Select Clock Percentage:"
                    visible: false
                    model: ListModel {
                        ListElement { name: "100" }
                        ListElement { name: "50" }
                        ListElement { name: "33" }
                        ListElement { name: "25" }
                        ListElement { name: "20" }
                    }
                    objectName: "clockDial"
                    selectedIndex: 2
                }
                objectName: "clock"
                onClicked: clockDial.open()
            }
            font.family: "Times New Roman"
        }
        states: [
            State {
                name: "active"
                PropertyChanges {
                    target: tabconfigaudio
                    x: "0"; y: "0"
                }
                when: menuBase.activeTab == 6
            }]
        transitions: [
            Transition {
                NumberAnimation {
                    properties: "x,y"
                    easing.type: "InOutQuad"
                    duration: 300
                }
            }]
    }

}
