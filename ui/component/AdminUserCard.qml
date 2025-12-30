import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0

FluFrame {
    id: adminUserCard
    // 使用回调属性替代信号
    property var onCardClicked: null
    
    property string userId: ""
    property string userName: ""
    property string telephone: ""

    radius: 8
    padding: 16
    // 【修复】增加回退色
    color: FluTheme.surfaceColor || "#ffffff"
    implicitHeight: 60

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true
        // 【修复】增加回退色
        onEntered: adminUserCard.color = FluTheme.hoverColor || "#f5f5f5"
        onExited: adminUserCard.color = FluTheme.surfaceColor || "#ffffff"
        // 当点击时调用回调函数
        onClicked: {
            if (adminUserCard.onCardClicked) {
                adminUserCard.onCardClicked()
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 20
        FluIcon {
            iconSource: FluentIcons.Contact
            iconSize: 32
            color: FluTheme.primaryColor || "#0078d4"
        }
        ColumnLayout {
            spacing: 4
            Layout.fillWidth: true
            FluText {
                text: userName
                font.pixelSize: 16
                font.bold: true
                color: FluTheme.primaryTextColor || "#000000"
                Layout.fillWidth: true
            }
            RowLayout {
                spacing: 15
                FluText {
                    text: "用户ID: " + userId
                    font.pixelSize: 12
                    color: FluTheme.secondaryTextColor || "#666666"
                }
                FluText {
                    text: "联系电话: " + (telephone || "未填写")
                    font.pixelSize: 12
                    color: FluTheme.secondaryTextColor || "#666666"
                }
            }
        }
        FluIcon {
            iconSource: FluentIcons.ChevronRight
            iconSize: 12
            color: FluTheme.disableColor || "#cccccc"
        }
    }
}
