import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0

ColumnLayout {
    id: flightForm
    width: parent.width
    
    // ==================== 属性定义 ====================
    property string title: "添加修改"
    // 添加isEditing属性，用于区分新增和修改模式
    property bool isEditing: false
    
    // 表单字段
    property int flightId: -1 // 使用var类型，更灵活处理不同类型的值
    property string flightNumber: ""
    property string departureCity: ""
    property string arrivalCity: ""
    property string date: ""
    property string departureTime: ""
    property string arrivalTime: ""
    property string departureAirport: ""
    property string arrivalAirport: ""
    property string checkinStartTime: ""
    property string checkinEndTime: ""
    property string price: ""
    property string airlineCompany: ""
    property string status: "正常"
    property string leftTickets: ""
    
    // ==================== 信号定义 ====================
    signal submit
    signal cancel
    
    // ==================== 功能函数 ====================
    // 重置表单
    function reset() {
        flightId = -1;
        flightNumber = "";
        departureCity = "";
        arrivalCity = "";
        date = "";
        departureTime = "";
        arrivalTime = "";
        departureAirport = "";
        arrivalAirport = "";
        checkinStartTime = "";
        checkinEndTime = "";
        price = "";
        airlineCompany = "";
        status = "正常";
        leftTickets = "";
    }
    
    // 填充表单
    function fillForm(flight) {
        // flightId是整数类型，只有当flightId不存在或为undefined时才使用默认值-1
        // 0是有效的航班ID，不能被替换
        flightId = flight.flightId !== undefined ? flight.flightId : -1;
        
        flightNumber = flight.flightNumber || "";
        departureCity = flight.departureCity || "";
        arrivalCity = flight.arrivalCity || "";
        date = flight.date || "";
        departureTime = flight.departureTime || "";
        arrivalTime = flight.arrivalTime || "";
        departureAirport = flight.departureAirport || "";
        arrivalAirport = flight.arrivalAirport || "";
        checkinStartTime = flight.checkinStartTime || "";
        checkinEndTime = flight.checkinEndTime || "";
        // 价格可能是undefined或null，需要特殊处理
        price = flight.price ? (flight.price / 100).toString() : "";
        airlineCompany = flight.airlineCompany || "";
        status = flight.status || "正常";
        leftTickets = flight.leftTickets || "";
    }
    
    // ==================== UI布局 ====================
    spacing: 15
    
    // 标题
    FluText {
        text: title
        font: FluTextStyle.Title
    }
    
    // 表单内容
    GridLayout {
        columns: 2
        rowSpacing: 10
        columnSpacing: 20
        
        // 第一行
        // 航班ID - 仅在修改模式下显示，且只读
        FluText {
            text: "航班ID:"
            Layout.alignment: Qt.AlignVCenter
            // 仅在修改模式下显示
            visible: isEditing
        }
        FluTextBox {
            // 确保flightId被正确转换为字符串，并且仅在有效时显示
            // 处理各种情况：undefined, null, 0, 空字符串等
            text: flightId === undefined || flightId === null ? "" : flightId.toString()
            // 仅在修改模式下显示，且只读
            visible: isEditing
            readOnly: true
            placeholderText: "系统生成"
            Layout.fillWidth: true
        }
        
        // 航班号
        FluText {
            text: "航班号:"
            Layout.alignment: Qt.AlignVCenter
        }
        FluTextBox {
            text: flightNumber
            placeholderText: "请输入航班号"
            onTextChanged: {
                flightNumber = text;
            }
            Layout.fillWidth: true
        }
        
        FluText {
            text: "出发城市:"
            Layout.alignment: Qt.AlignVCenter
        }
        FluTextBox {
            text: departureCity
            placeholderText: "请输入出发城市"
            onTextChanged: {
                departureCity = text;
            }
            Layout.fillWidth: true
        }
        
        // 第二行
        FluText {
            text: "到达城市:"
            Layout.alignment: Qt.AlignVCenter
        }
        FluTextBox {
            text: arrivalCity
            placeholderText: "请输入到达城市"
            onTextChanged: {
                arrivalCity = text;
            }
            Layout.fillWidth: true
        }
        
        FluText {
            text: "航班日期:"
            Layout.alignment: Qt.AlignVCenter
        }
        FluTextBox {
            text: date
            placeholderText: "YYYY-MM-DD"
            onTextChanged: {
                date = text;
            }
            Layout.fillWidth: true
        }
        
        // 第三行
        FluText {
            text: "起飞时间:"
            Layout.alignment: Qt.AlignVCenter
        }
        FluTextBox {
            text: departureTime
            placeholderText: "HH:MM"
            onTextChanged: {
                departureTime = text;
            }
            Layout.fillWidth: true
        }
        
        FluText {
            text: "到达时间:"
            Layout.alignment: Qt.AlignVCenter
        }
        FluTextBox {
            text: arrivalTime
            placeholderText: "HH:MM"
            onTextChanged: {
                arrivalTime = text;
            }
            Layout.fillWidth: true
        }
        
        // 第四行
        FluText {
            text: "出发机场:"
            Layout.alignment: Qt.AlignVCenter
        }
        FluTextBox {
            text: departureAirport
            placeholderText: "请输入出发机场"
            onTextChanged: {
                departureAirport = text;
            }
            Layout.fillWidth: true
        }
        
        FluText {
            text: "到达机场:"
            Layout.alignment: Qt.AlignVCenter
        }
        FluTextBox {
            text: arrivalAirport
            placeholderText: "请输入到达机场"
            onTextChanged: {
                arrivalAirport = text;
            }
            Layout.fillWidth: true
        }
        
        // 第五行
        FluText {
            text: "值机开始时间:"
            Layout.alignment: Qt.AlignVCenter
        }
        FluTextBox {
            text: checkinStartTime
            placeholderText: "HH:MM"
            onTextChanged: {
                checkinStartTime = text;
            }
            Layout.fillWidth: true
        }
        
        FluText {
            text: "值机结束时间:"
            Layout.alignment: Qt.AlignVCenter
        }
        FluTextBox {
            text: checkinEndTime
            placeholderText: "HH:MM"
            onTextChanged: {
                checkinEndTime = text;
            }
            Layout.fillWidth: true
        }
        
        // 第六行
        FluText {
            text: "航班价格:"
            Layout.alignment: Qt.AlignVCenter
        }
        FluTextBox {
            text: price
            placeholderText: "请输入价格"
            onTextChanged: {
                price = text;
            }
            Layout.fillWidth: true
        }
        
        // 航空公司 - 仅在新增模式下显示
        FluText {
            text: "航空公司:"
            Layout.alignment: Qt.AlignVCenter
            // 仅在新增模式下显示
            visible: !isEditing
        }
        FluTextBox {
            text: airlineCompany
            placeholderText: "请输入航空公司"
            onTextChanged: {
                airlineCompany = text;
            }
            // 仅在新增模式下显示
            visible: !isEditing
            Layout.fillWidth: true
        }
        
        // 第七行
        FluText {
            text: "航班状态:"
            Layout.alignment: Qt.AlignVCenter
        }
        FluComboBox {
            currentIndex: status === "正常" ? 0 : 1
            onCurrentIndexChanged: {
                status = currentIndex === 0 ? "正常" : "延误";
            }
            Layout.fillWidth: true
            
            model: [
                "正常",
                "延误"
            ]
        }
        
        FluText {
            // 根据isEditing属性显示不同的标签
            text: isEditing ? "剩余票数:" : "初始票数:"
            Layout.alignment: Qt.AlignVCenter
        }
        FluTextBox {
            text: leftTickets
            placeholderText: isEditing ? "请输入剩余票数" : "请输入初始票数"
            onTextChanged: {
                leftTickets = text;
            }
            Layout.fillWidth: true
        }
    }
    
    // 按钮区域
    RowLayout {
        spacing: 10
        
        FluButton {
            text: "取消"
            onClicked: {
                cancel();
            }
        }
        
        FluFilledButton {
            text: "提交"
            onClicked: {
                submit();
            }
        }
    }
}
