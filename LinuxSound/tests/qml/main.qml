
import Qt 4.7
import LinuxSound 1.0

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
        text: "Ready to play"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                sound.play()
            }
        }
    }

    LinuxSound {
        id: sound
        source: "../files/PCM_16bit_44100Hz.wav"
        rate: 44100
        onPlayStarted: {
            console.debug("play started");
        }
        onPlayFinished: {
            console.debug("play finished");
        }
    }
}
