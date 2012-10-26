
import Qt 4.7
import LinuxInput 1.0

Rectangle {
    id : background

    color: "black"
    width: 400
    height: 400

    Text {
        id: display
        anchors.centerIn: parent
        font.pixelSize: 30
        color: "white"
        text: "Ready to get input"
    }

    LinuxInput {
        id: input
        devName: "/dev/input/event2"
        onVolumDownPressed: {
            display.text = "down"
        }
        onVolumUpPressed: {
            display.text = "up"
        }
    }
}
