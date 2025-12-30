import QtQuick 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0

// 航班分页组件
FluControlBackground {
    id: pagination
    width: parent.width
    height: 60
    
    // 属性定义
    property int currentPage: 1
    property int totalPages: 1
    property int totalRecords: 0
    
    // 信号定义
    signal pageChanged(int page)
    
    // 确保属性变化时能够更新UI
    onCurrentPageChanged: {
        console.log("分页组件内部currentPage变化: " + currentPage);
    }
    
    onTotalPagesChanged: {
        console.log("总页数变化: " + totalPages);
    }
    
    onTotalRecordsChanged: {
        console.log("总记录数变化: " + totalRecords);
    }
    
    RowLayout {
        anchors.centerIn: parent
        spacing: 20
        
        FluButton {
            text: "上一页"
            enabled: currentPage > 1
            onClicked: {
                var newPage = currentPage - 1;
                pageChanged(newPage);
            }
        }
        
        FluText {
            text: "第 " + currentPage + " / " + totalPages + " 页"
            Layout.alignment: Qt.AlignVCenter
        }
        
        FluButton {
            text: "下一页"
            enabled: currentPage < totalPages
            onClicked: {
                var newPage = currentPage + 1;
                pageChanged(newPage);
            }
        }
        
        FluText {
            text: "共 " + totalRecords + " 条记录"
            Layout.alignment: Qt.AlignVCenter
        }
    }
}
