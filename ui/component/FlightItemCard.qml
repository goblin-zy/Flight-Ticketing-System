import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0

// 航班列表项组件
FluControlBackground {
    id: flightItem
    width: parent.width
    height: 120 // 增加卡片高度，确保所有内容能完整显示
    
    // 属性定义
    property string flightNumber: ""
    property int flightId: -1
    property string airlineCompany: ""
    property string departureCity: ""
    property string departureAirport: ""
    property string departureTime: ""
    property string arrivalCity: ""
    property string arrivalAirport: ""
    property string arrivalTime: ""
    property real price: 0
    property string status: ""
    property bool isBooked: false
    
    // 信号定义
    signal flightSelected
    
    GridLayout {
        columns: 5
        columnSpacing: 10
        rowSpacing: 5
        anchors.fill: parent
        anchors.margins: 10

        // 1. 航班基本信息列 - 增加宽度，确保航班号完整显示
        Column {
            Layout.preferredWidth: 120 // 增加宽度，确保航班号完整显示
            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            spacing: 5
            
            FluText {
                text: flightNumber
                font: FluTextStyle.Title
                // 移除elide，确保完整显示航班号
                width: parent.width
            }
            FluText {
                text: "航班ID: " + flightId
                font: FluTextStyle.Body
                color: FluTheme.dark ? Qt.rgba(128/255, 128/255, 128/255, 1) : Qt.rgba(160/255, 160/255, 160/255, 1)
                width: parent.width
            }
            FluText {
                text: airlineCompany
                font: FluTextStyle.Body
                width: parent.width
            }
        }

        // 2. 出发信息列 - 增加宽度，确保信息完整显示
        Column {
            Layout.preferredWidth: 170 // 增加宽度，确保信息完整显示
            Layout.alignment: Qt.AlignCenter
            spacing: 5
            
            FluText {
                text: departureCity
                font: FluTextStyle.Title
                horizontalAlignment: Text.AlignHCenter
                width: parent.width
            }
            FluText {
                text: departureAirport
                font: FluTextStyle.Body
                horizontalAlignment: Text.AlignHCenter
                width: parent.width
            }
            FluText {
                text: departureTime
                font: FluTextStyle.Body
                horizontalAlignment: Text.AlignHCenter
                width: parent.width
            }
        }

        // 3. 航线图标列 - 固定宽度，居中对齐
        Item {
            Layout.preferredWidth: 50
            Layout.alignment: Qt.AlignVCenter
            
            FluText {
                anchors.centerIn: parent
                text: "→"
                font.pixelSize: 20
            }
        }

        // 4. 到达信息列 - 增加宽度，确保信息完整显示
        Column {
            Layout.preferredWidth: 170 // 增加宽度，确保信息完整显示
            Layout.alignment: Qt.AlignCenter
            spacing: 5
            
            FluText {
                text: arrivalCity
                font: FluTextStyle.Title
                horizontalAlignment: Text.AlignHCenter
                width: parent.width
            }
            FluText {
                text: arrivalAirport
                font: FluTextStyle.Body
                horizontalAlignment: Text.AlignHCenter
                width: parent.width
            }
            FluText {
                text: arrivalTime
                font: FluTextStyle.Body
                horizontalAlignment: Text.AlignHCenter
                width: parent.width
            }
        }

        // 5. 价格和操作列 - 填充剩余宽度，右对齐
        Column {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            spacing: 5
            
            FluText {
                text: "¥" + price
                font: FluTextStyle.Title
                horizontalAlignment: Text.AlignRight
            }
            
            FluText {
                text: status
                font: FluTextStyle.Body
                horizontalAlignment: Text.AlignRight
            }
            
            // 已预定标记
            FluText {
                text: "已预定"
                visible: isBooked
                font: FluTextStyle.Body
                color: "green"
                horizontalAlignment: Text.AlignRight
            }
            
            FluButton {
                text: isBooked ? "已预定" : "选择"
                enabled: !isBooked
                onClicked: {
                    flightSelected();
                }
            }
        }
    }
}