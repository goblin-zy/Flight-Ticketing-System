import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0
  
// 航班筛选栏组件，包含筛选模型
FluControlBackground {
    id: filterBar
    width: parent.width
    height: 80
    
    // 直接在筛选栏组件中定义出发城市列表，包含csv中的所有城市
    ListModel {
        id: departureCityModel
        ListElement { name: "全部" }
        ListElement { name: "北京" }
        ListElement { name: "成都" }
        ListElement { name: "福州" }
        ListElement { name: "广州" }
        ListElement { name: "杭州" }
        ListElement { name: "合肥" }
        ListElement { name: "昆明" }
        ListElement { name: "南京" }
        ListElement { name: "青岛" }
        ListElement { name: "厦门" }
        ListElement { name: "上海" }
        ListElement { name: "深圳" }
        ListElement { name: "沈阳" }
        ListElement { name: "天津" }
        ListElement { name: "武汉" }
        ListElement { name: "西安" }
        ListElement { name: "长沙" }
        ListElement { name: "郑州" }
        ListElement { name: "重庆" }
    }
    
    // 直接在筛选栏组件中定义到达城市列表，包含csv中的所有城市
    ListModel {
        id: arrivalCityModel
        ListElement { name: "全部"   }
        ListElement { name: "北京" }
        ListElement { name: "成都" }
        ListElement { name: "福州" }
        ListElement { name: "广州" }
        ListElement { name: "杭州" }
        ListElement { name: "合肥" }
        ListElement { name: "昆明" }
        ListElement { name: "南京" }
        ListElement { name: "青岛" }
        ListElement { name: "厦门" }
        ListElement { name: "上海" }
        ListElement { name: "深圳" }
        ListElement { name: "沈阳" }
        ListElement { name: "天津" }
        ListElement { name: "武汉" }
        ListElement { name: "西安" }
        ListElement { name: "长沙" }
        ListElement { name: "郑州" }
        ListElement { name: "重庆" }
    }
    
    // 直接在筛选栏组件中定义日期列表，包含12月到二月底的所有日期
    ListModel {
        id: flightDateModel
        ListElement { name: "全部" }
        // 2025年12月
        ListElement { name: "2025-12-01" }
        ListElement { name: "2025-12-02" }
        ListElement { name: "2025-12-03" }
        ListElement { name: "2025-12-04" }
        ListElement { name: "2025-12-05" }
        ListElement { name: "2025-12-06" }
        ListElement { name: "2025-12-07" }
        ListElement { name: "2025-12-08" }
        ListElement { name: "2025-12-09" }
        ListElement { name: "2025-12-10" }
        ListElement { name: "2025-12-11" }
        ListElement { name: "2025-12-12" }
        ListElement { name: "2025-12-13" }
        ListElement { name: "2025-12-14" }
        ListElement { name: "2025-12-15" }
        ListElement { name: "2025-12-16" }
        ListElement { name: "2025-12-17" }
        ListElement { name: "2025-12-18" }
        ListElement { name: "2025-12-19" }
        ListElement { name: "2025-12-20" }
        ListElement { name: "2025-12-21" }
        ListElement { name: "2025-12-22" }
        ListElement { name: "2025-12-23" }
        ListElement { name: "2025-12-24" }
        ListElement { name: "2025-12-25" }
        ListElement { name: "2025-12-26" }
        ListElement { name: "2025-12-27" }
        ListElement { name: "2025-12-28" }
        ListElement { name: "2025-12-29" }
        ListElement { name: "2025-12-30" }
        ListElement { name: "2025-12-31" }
        // 2026年1月
        ListElement { name: "2026-01-01" }
        ListElement { name: "2026-01-02" }
        ListElement { name: "2026-01-03" }
        ListElement { name: "2026-01-04" }
        ListElement { name: "2026-01-05" }
        ListElement { name: "2026-01-06" }
        ListElement { name: "2026-01-07" }
        ListElement { name: "2026-01-08" }
        ListElement { name: "2026-01-09" }
        ListElement { name: "2026-01-10" }
        ListElement { name: "2026-01-11" }
        ListElement { name: "2026-01-12" }
        ListElement { name: "2026-01-13" }
        ListElement { name: "2026-01-14" }
        ListElement { name: "2026-01-15" }
        ListElement { name: "2026-01-16" }
        ListElement { name: "2026-01-17" }
        ListElement { name: "2026-01-18" }
        ListElement { name: "2026-01-19" }
        ListElement { name: "2026-01-20" }
        ListElement { name: "2026-01-21" }
        ListElement { name: "2026-01-22" }
        ListElement { name: "2026-01-23" }
        ListElement { name: "2026-01-24" }
        ListElement { name: "2026-01-25" }
        ListElement { name: "2026-01-26" }
        ListElement { name: "2026-01-27" }
        ListElement { name: "2026-01-28" }
        ListElement { name: "2026-01-29" }
        ListElement { name: "2026-01-30" }
        ListElement { name: "2026-01-31" }
        // 2026年2月
        ListElement { name: "2026-02-01" }
        ListElement { name: "2026-02-02" }
        ListElement { name: "2026-02-03" }
        ListElement { name: "2026-02-04" }
        ListElement { name: "2026-02-05" }
        ListElement { name: "2026-02-06" }
        ListElement { name: "2026-02-07" }
        ListElement { name: "2026-02-08" }
        ListElement { name: "2026-02-09" }
        ListElement { name: "2026-02-10" }
        ListElement { name: "2026-02-11" }
        ListElement { name: "2026-02-12" }
        ListElement { name: "2026-02-13" }
        ListElement { name: "2026-02-14" }
        ListElement { name: "2026-02-15" }
        ListElement { name: "2026-02-16" }
        ListElement { name: "2026-02-17" }
        ListElement { name: "2026-02-18" }
        ListElement { name: "2026-02-19" }
        ListElement { name: "2026-02-20" }
        ListElement { name: "2026-02-21" }
        ListElement { name: "2026-02-22" }
        ListElement { name: "2026-02-23" }
        ListElement { name: "2026-02-24" }
        ListElement { name: "2026-02-25" }
        ListElement { name: "2026-02-26" }
        ListElement { name: "2026-02-27" }
        ListElement { name: "2026-02-28" }
    }
    
    // 直接在筛选栏组件中定义航空公司列表，包含csv中的所有航空公司
    ListModel {
        id: airlineModel
        ListElement { name: "全部" }
        ListElement { name: "奥凯航空" }
        ListElement { name: "成都航空" }
        ListElement { name: "东方航空" }
        ListElement { name: "东海航空" }
        ListElement { name: "海航｜北部湾航空" }
        ListElement { name: "海航｜福州航空" }
        ListElement { name: "海航｜海南航空" }
        ListElement { name: "海航｜金鹏航空" }
        ListElement { name: "海航｜首都航空" }
        ListElement { name: "海航｜天津航空" }
        ListElement { name: "海航｜乌鲁木齐航空" }
        ListElement { name: "海航｜西部航空" }
        ListElement { name: "海航｜祥鹏航空" }
        ListElement { name: "海航｜长安航空" }
        ListElement { name: "河北航空" }
        ListElement { name: "湖南航空" }
        ListElement { name: "华夏航空" }
        ListElement { name: "吉祥航空" }
        ListElement { name: "江西航空" }
        ListElement { name: "昆明航空" }
        ListElement { name: "南方航空" }
        ListElement { name: "青岛航空" }
        ListElement { name: "厦门航空" }
        ListElement { name: "山东航空" }
        ListElement { name: "上海航空" }
        ListElement { name: "深圳航空" }
        ListElement { name: "四川航空" }
        ListElement { name: "苏南瑞丽航空" }
        ListElement { name: "西藏航空" }
        ListElement { name: "长龙航空" }
        ListElement { name: "中国国航" }
        ListElement { name: "中国联合航空" }
        ListElement { name: "重庆航空" }
    }
    
    // 属性定义
    property int departureCityIndex: 0
    property int arrivalCityIndex: 0
    property int flightDateIndex: 0
    property int airlineIndex: 0
    
    // 暴露模型为属性，供外部访问
    property alias departureCityModel: departureCityModel
    property alias arrivalCityModel: arrivalCityModel
    property alias flightDateModel: flightDateModel
    property alias airlineModel: airlineModel
    
    // 信号定义
    signal searchTriggered
    
    GridLayout {
        columns: 5
        columnSpacing: 10
        rowSpacing: 5
        anchors.fill: parent
        anchors.margins: 10

        FluComboBox {
            id: departureCityComboBox
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            model: departureCityModel
            textRole: "name"
            displayText: currentIndex >= 0 ? departureCityModel.get(currentIndex).name : "全部"
            onCurrentIndexChanged: {
                console.log("出发城市选中索引: " + currentIndex);
                filterBar.departureCityIndex = currentIndex;
            }
        }

        FluComboBox {
            id: arrivalCityComboBox
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            model: arrivalCityModel
            textRole: "name"
            displayText: currentIndex >= 0 ? arrivalCityModel.get(currentIndex).name : "全部"
            onCurrentIndexChanged: {
                console.log("到达城市选中索引: " + currentIndex);
                filterBar.arrivalCityIndex = currentIndex;
            }
        }

        FluComboBox {
            id: flightDateComboBox
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            model: flightDateModel
            textRole: "name"
            displayText: currentIndex >= 0 ? flightDateModel.get(currentIndex).name : "全部"
            onCurrentIndexChanged: {
                console.log("日期选中索引: " + currentIndex);
                filterBar.flightDateIndex = currentIndex;
            }
        }

        FluComboBox {
            id: airlineComboBox
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            model: airlineModel
            textRole: "name"
            displayText: currentIndex >= 0 ? airlineModel.get(currentIndex).name : "全部"
            onCurrentIndexChanged: {
                console.log("航空公司选中索引: " + currentIndex);
                filterBar.airlineIndex = currentIndex;
            }
        }

        FluButton {
            text: "查询"
            Layout.alignment: Qt.AlignVCenter
            onClicked: {
                searchTriggered();
            }
        }
    }
}
