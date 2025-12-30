import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import FluentUI 1.0

// 【重构】：根节点改为原生 Rectangle，确保边框绝对可见
Rectangle {
    id: root

    // --- 1. 外观样式 (高对比度) ---
    color: FluTheme.surfaceColor || "#FFFFFF" // 卡片背景：白色
    radius: 8                                 // 圆角
    border.width: 1                           // 边框宽度：1px
    border.color: FluTheme.dark ? "#404040" : "#D4D4D4" // 边框颜色：深灰/浅灰

    // 添加阴影增加立体感 (如果你的FluentUI版本不支持 FluShadow，可删除此段)
    FluShadow {
        radius: 8
        anchors.fill: parent
        z: -1
    }

    // --- 2. 业务属性 (完全保留原有逻辑) ---
    property string orderId: ""
    property string flightNumber: ""
    property string departureCity: ""
    property string arrivalCity: ""
    property string departureTime: ""
    property string arrivalTime: ""
    property string status: ""
    property double price: 0
    property string createTime: ""
    property string passengers: "[]"
    property var passengerList: { try { return JSON.parse(passengers) } catch (e) { return [] } }

    property int hoursUntilDeparture: 0
    property int minutesUntilDeparture: 0

    signal payButtonClicked
    signal refundButtonClicked
    signal changeButtonClicked

    // --- 3. 倒计时逻辑 (完全保留) ---
    function initCountdown() {
        var now = new Date();
        var departure = new Date(departureTime);
        var diff = departure.getTime() - now.getTime();
        if (diff > 0) {
            hoursUntilDeparture = Math.floor(diff / (1000 * 60 * 60));
            minutesUntilDeparture = Math.floor((diff % (1000 * 60 * 60)) / (1000 * 60));
        } else {
            hoursUntilDeparture = 0;
            minutesUntilDeparture = 0;
        }
    }
    Timer {
        id: countdownTimer; interval: 60000; repeat: true
        running: status === "已支付" && hoursUntilDeparture > 0
        onTriggered: initCountdown()
    }
    Component.onCompleted: initCountdown()

    // --- 4. 内部布局 (优化版) ---
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        // 第一行：订单号与状态
        RowLayout {
            Layout.fillWidth: true
            FluText {
                text: "订单号: " + orderId
                color: "#808080"
                font.pixelSize: 12
            }
            Item { Layout.fillWidth: true } // 占位符
            FluText {
                text: status
                font.bold: true
                color: status === "已完成" ? "#52c41a" : (status === "待支付" ? "#ff4d4f" : "#faad14")
            }
        }

        // 第二行：分割线
        Rectangle { Layout.fillWidth: true; height: 1; color: "#EEEEEE" }

        // 第三行：核心航班信息
        RowLayout {
            Layout.fillWidth: true
            spacing: 20

            // 左：航班号与时间
            Column {
                FluText { text: flightNumber; font: FluTextStyle.BodyStrong }
                FluText { text: createTime; font.pixelSize: 10; color: "#999999" }
            }

            // 中：路线
            Item { Layout.fillWidth: true }
            Column {
                Layout.alignment: Qt.AlignHCenter
                FluText { text: departureCity + " → " + arrivalCity; font: FluTextStyle.Title }
                FluText { text: (departureTime || "") + " 起飞"; font.pixelSize: 12; color: "#666666" }
            }
            Item { Layout.fillWidth: true }
        }

        // 第四行：乘客信息摘要
        FluText {
            text: "乘客: " + (passengerList.length > 0 ? passengerList.map(p => p.name).join(", ") : "无")
            color: "#666666"
            font.pixelSize: 12
            elide: Text.ElideRight
            Layout.fillWidth: true
        }

        Item { Layout.fillHeight: true } // 底部弹簧，将按钮推到底部

        // 第五行：价格与按钮
        RowLayout {
            Layout.fillWidth: true
            FluText {
                text: "¥" + (price || 0).toFixed(2)
                font: FluTextStyle.Title
                color: FluTheme.primaryColor
            }
            Item { Layout.fillWidth: true }

            FluButton {
                text: "改签"
                visible: status === "待支付" || status === "已支付" || status === "已完成"
                onClicked: changeButtonClicked()
            }
            FluButton {
                text: "退票"
                visible: status === "已完成" || status === "待支付" || status === "已支付"
                onClicked: refundButtonClicked()
            }
            FluFilledButton {
                text: "去支付"
                visible: status === "待支付"
                onClicked: payButtonClicked()
            }
        }
    }
}
