import QtQuick 2.15
import FluentUI 1.0

FluPage {
    id: flightInfoEditView
    
    title: qsTr("航班管理 - 编辑")
    
    Column {
        spacing: 20
        anchors.centerIn: parent
        width: parent.width * 0.8
        
        FluText {
            text: qsTr("航班管理编辑页面")
            font.pixelSize: 24
            anchors.horizontalCenter: parent.horizontalCenter
        }
        
        FluText {
            text: qsTr("该页面用于管理员编辑和修改航班信息")
            font.pixelSize: 16
            anchors.horizontalCenter: parent.horizontalCenter
        }
        
        FluText {
            text: qsTr("功能：")
            font.pixelSize: 18
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
        }
        
        FluText {
            text: qsTr("1. 搜索航班信息 (API 2.2.1)")
            font.pixelSize: 16
            anchors.horizontalCenter: parent.horizontalCenter
        }
        
        FluText {
            text: qsTr("2. 修改航班信息 (API 2.2.3)")
            font.pixelSize: 16
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}