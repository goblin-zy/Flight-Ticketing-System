import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0

FluFrame {
    id: adminOrderCard
    radius: 8
    padding: 16
    // 12-28修改：增加颜色回退，防止 undefined 报错
    color: FluTheme.surfaceColor || "#ffffff"

    property string orderId: ""
    property string flightNumber: ""
    property string passengerName: ""
    property string telephone: ""
    property string status: ""

    ColumnLayout {
        anchors.fill: parent
        spacing: 8

        RowLayout {
            Layout.fillWidth: true

            FluText {
                text: "订单 ID: " + orderId
                font.pixelSize: 14
                font.bold: true
                Layout.fillWidth: true
                // 设置文本颜色，根据主题自动调整
                textColor: FluTheme.fontPrimaryColor || FluTheme.primaryTextColor || "#000000"
            }

            FluText {
                text: status
                font.pixelSize: 12
                // 12-28修改：增加颜色回退
                color: status === "已完成" ? (FluTheme.successColor || "#52c41a") : (FluTheme.primaryColor || "#0078d4")
                font.bold: true
            }
        }

        // 12-28修改：优化信息展示布局
        RowLayout {
            spacing: 20
            FluText {
                text: "航班: " + flightNumber
                font.pixelSize: 12
                color: FluTheme.secondaryTextColor || "#666666"
            }
            FluText {
                text: "联系人: " + passengerName
                font.pixelSize: 12
                color: FluTheme.secondaryTextColor || "#666666"
            }
            FluText {
                text: "手机号: " + telephone
                font.pixelSize: 12
                color: FluTheme.secondaryTextColor || "#666666"
            }
        }

        RowLayout {
            spacing: 8
            Layout.alignment: Qt.AlignRight

            FluButton {
                text: "查看详情"
                onClicked: {
                    console.log("12-28修改：点击查看订单详情：" + orderId)
                }
            }

            FluButton {
                text: "取消订单"
                // 12-28修改：增加颜色回退
                normalColor: FluTheme.errorColor || "#ff4d4f"
                onClicked: {
                    console.log("12-28修改：管理员尝试取消订单：" + orderId)
                }
            }
        }
    }
}
