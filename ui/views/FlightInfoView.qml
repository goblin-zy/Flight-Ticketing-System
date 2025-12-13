import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0

FluPage {
    id: flightInfoPage
    title: "航班信息"
    visible: true
    launchMode: FluPageType.Standard
    
    ColumnLayout {
        anchors.centerIn: parent
        spacing: 20
        
        FluIcon {
            anchors.horizontalCenter: parent.horizontalCenter
            icon: FluentIcons.Airplane
            size: 64
            color: FluTheme.primaryColor
        }
        
        FluText {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "航班信息页面"
            fontSize: FluTextStyle.Title
        }
        
        FluText {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "页面正在开发中..."
            fontSize: FluTextStyle.Subtitle
            color: FluTheme.secondaryTextColor
        }
    }
}