import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import FluentUI 1.0

// 订单卡片组件
Item {
    // 定义组件属性
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
    
    // 解析乘车人数据为数组
    property var passengerList: {
        try {
            return JSON.parse(passengers)
        } catch (e) {
            return []
        }
    }
    
    // 倒计时相关属性
    property int hoursUntilDeparture: 0
    property int minutesUntilDeparture: 0
    
    // 定义信号
    signal payButtonClicked
    signal refundButtonClicked
    signal changeButtonClicked
    signal paymentRequest(string orderId, double price)
    signal refundRequest(string orderId)
    
    // 初始化倒计时
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
    
    // 定时器更新倒计时
    Timer {
        id: countdownTimer
        interval: 60000 // 每分钟更新一次
        repeat: true
        running: status === "已支付" && hoursUntilDeparture > 0
        onTriggered: {
            initCountdown();
        }
    }
    
    // 组件加载时初始化倒计时
    Component.onCompleted: {
        initCountdown();
    }
    
    width: parent.width - 40
    height: 180
    
    FluControlBackground {
        width: parent.width
        height: parent.height
        // 移除固定背景色，让FluControlBackground自动处理黑夜模式

        // 使用GridLayout来精确控制元素位置，与航班卡片保持一致
        GridLayout {
            id: contentGrid
            columns: 4
            columnSpacing: 15
            rowSpacing: 5
            anchors.fill: parent
            anchors.margins: 10

            // 1. 订单信息列（航班号、订单号、状态、创建时间）
            Column {
                Layout.preferredWidth: 160
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                spacing: 6

                FluText {
                    text: flightNumber
                    font: FluTextStyle.Title
                    width: parent.width
                }
                FluText {
                    text: "订单号: " + orderId
                    font: FluTextStyle.Body
                    width: parent.width
                    color: FluTheme.dark ? Qt.rgba(128/255, 128/255, 128/255, 1) : Qt.rgba(160/255, 160/255, 160/255, 1)
                }
                FluText {
                    text: status
                    font: FluTextStyle.Body
                    color: status === "已完成" ? "green" : (status === "待支付" ? "red" : "orange")
                    width: parent.width
                }
                FluText {
                    text: "创建时间: " + (createTime || "")
                    font.pixelSize: 11
                    width: parent.width
                    color: FluTheme.dark ? Qt.rgba(128/255, 128/255, 128/255, 1) : Qt.rgba(160/255, 160/255, 160/255, 1)
                    wrapMode: Text.WrapAnywhere
                }
            }

            // 2. 航班信息列（出发城市、箭头、到达城市）
            Column {
                Layout.preferredWidth: 260
                Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                spacing: 8

                Row {
                    width: parent.width
                    spacing: 12
                    anchors.horizontalCenter: parent.horizontalCenter

                    // 出发
                    Column {
                        id: leftCol
                        width: 90
                        spacing: 4

                        FluText {
                            text: departureCity
                            font: FluTextStyle.Title
                            width: parent.width
                            horizontalAlignment: Text.AlignHCenter
                            elide: Text.ElideRight
                        }

                        FluText {
                            text: departureTime || ""
                            font.pixelSize: 11
                            width: parent.width
                            horizontalAlignment: Text.AlignHCenter
                            wrapMode: Text.WrapAnywhere
                            color: FluTheme.dark ? Qt.rgba(200/255, 200/255, 200/255, 1) : Qt.rgba(160/255, 160/255, 160/255, 1)
                        }
                    }

                    // 箭头（强制居中）
                    Item {
                        width: 30
                        height: Math.max(leftCol.implicitHeight, rightCol.implicitHeight)

                        FluText {
                            text: "→"
                            font.pixelSize: 20
                            anchors.centerIn: parent
                            color: FluTheme.dark ? Qt.rgba(200/255, 200/255, 200/255, 1) : Qt.rgba(80/255, 80/255, 80/255, 1)
                        }
                    }

                    // 到达
                    Column {
                        id: rightCol
                        width: 90
                        spacing: 4

                        FluText {
                            text: arrivalCity
                            font: FluTextStyle.Title
                            width: parent.width
                            horizontalAlignment: Text.AlignHCenter
                            elide: Text.ElideRight
                        }

                        FluText {
                            text: arrivalTime || ""
                            font.pixelSize: 11
                            width: parent.width
                            horizontalAlignment: Text.AlignHCenter
                            wrapMode: Text.WrapAnywhere
                            color: FluTheme.dark ? Qt.rgba(200/255, 200/255, 200/255, 1) : Qt.rgba(160/255, 160/255, 160/255, 1)
                        }
                    }
                }
            }

            // 3. 乘车人信息列
            Column {
                Layout.preferredWidth: 100
                Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                spacing: 6

                FluText {
                    text: "乘车人"
                    font.pixelSize: 12
                    horizontalAlignment: Text.AlignHCenter
                    color: FluTheme.dark ? Qt.rgba(200/255, 200/255, 200/255, 1) : Qt.rgba(80/255, 80/255, 80/255, 1)
                }
                
                // 显示乘车人姓名
                Column {
                    spacing: 2
                    width: parent.width
                    
                    // 使用Repeater显示乘客列表，确保正确处理各种数据情况
                    Repeater {
                        model: passengerList
                        
                        FluText {
                            text: modelData.name || "未知乘客"
                            font.pixelSize: 11
                            horizontalAlignment: Text.AlignHCenter
                            color: FluTheme.dark ? Qt.rgba(200/255, 200/255, 200/255, 1) : Qt.rgba(80/255, 80/255, 80/255, 1)
                        }
                    }
                    
                    // 如果没有乘客，显示提示
                    FluText {
                    text: "无乘车人"
                    font.pixelSize: 11
                    color: FluTheme.dark ? Qt.rgba(128/255, 128/255, 128/255, 1) : Qt.rgba(160/255, 160/255, 160/255, 1)
                    horizontalAlignment: Text.AlignHCenter
                    visible: passengerList.length === 0
                }
                }
            }

            // 4. 价格和操作列（对应航班卡片的价格和选择按钮列）
            Column {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignRight | Qt.AlignTop
                spacing: 5

                FluText {
                    text: "¥" + (price || 0).toFixed(2)
                    font: FluTextStyle.Title
                    horizontalAlignment: Text.AlignRight
                }

                // 支付按钮
                FluButton {
                    text: "支付"
                    visible: status === "待支付"
                    width: 80
                    onClicked: {
                        payButtonClicked()
                    }
                }

                // 退票按钮
                FluButton {
                    text: "退票"
                    visible: status === "已完成" || status === "待支付" || status === "已支付"
                    width: 80
                    onClicked: {
                        refundButtonClicked()
                    }
                }

                // 改签按钮
                FluButton {
                    text: "改签"
                    visible: status === "待支付" || status === "已支付" || status === "已完成"
                    width: 80
                    onClicked: {
                        changeButtonClicked()
                    }
                }
            }
        }
    }
}