import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import FluentUI 1.0
import App.Network 1.0
import "../component"

FluPage {
    id: adminUserOrdersView
    title: qsTr("订单列表管理")

    // 12-28修改：新增 targetTelephone 用于接收用户列表传来的搜索条件
    property string targetTelephone: ""
    property int totalOrders: 0
    property int currentPage: 1
    property int pageSize: 10
    
    // 新增订单ID搜索属性
    property string searchOrderId: ""
    // 新增订单状态搜索属性
    property string searchStatus: ""

    // 12-28修改：添加网络管理器处理真实数据请求
    NetworkManager {
        id: networkManager
        onRequestSuccess: function(data) {
            console.log("订单列表请求成功 --------------------")
            console.log("完整返回数据: ", JSON.stringify(data, null, 4))
            console.log("-----------------------------------\n")
            if (data['code'] === 200) {
                orderListModel.clear()
                if (data['data'] && data['data']['list']) {
                    var list = data['data']['list']
                    for (var i = 0; i < list.length; i++) {
                        orderListModel.append(list[i])
                    }
                }
                totalOrders = data['data']['total'] || 0
                console.log(" 获取订单列表成功，共 " + totalOrders + " 条")
            } else {
                console.error(" API业务错误 --------------------")
                console.error("错误代码: " + data['code'] + " | 错误信息: " + (data['message'] || "未知错误"))
                console.error("-----------------------------------\n")
            }
        }
        onRequestFailed: function(error) {
            console.error("订单列表请求失败 --------------------")
            console.error("失败原因: " + error)
            console.error("-----------------------------------\n")
        }
    }

    ListModel { id: orderListModel }

    // 12-28修改：封装查询函数，支持按电话号码、订单ID和状态过滤
    function fetchOrderList() {
        var params = {
            pageNum: currentPage,
            pageSize: pageSize
        }
        
        // 12-28修复：确保电话号码能正确添加到参数中
        var finalPhone = ""
        
        // 优先检查搜索框是否存在且有文本
        if (typeof searchField !== "undefined" && searchField.text !== "") {
            finalPhone = searchField.text
        } 
        // 否则使用跳转传入的电话
        else if (targetTelephone !== "") {
            finalPhone = targetTelephone
        }
        
        if (finalPhone !== "") {
            params.telephone = finalPhone
        }
        
        // 新增：添加订单ID搜索参数
        if (typeof orderIdSearch !== "undefined" && orderIdSearch.text !== "") {
            params.orderId = orderIdSearch.text
        }
        
        // 新增：添加订单状态搜索参数
        if (searchStatus !== "") {
            params.status = searchStatus
        }

        console.log("12-28修改：🔍 [订单查询] 参数: " + JSON.stringify(params))
        networkManager.request("/api/admin/order/all", NetworkManager.GET, params, userContext.myToken)
    }

    Component.onCompleted: {
        // 12-28修复：使用Qt.callLater确保searchField已创建
        Qt.callLater(function() {
            // 如果传了电话号码，同步到搜索框中
            if (targetTelephone !== "") {
                searchField.text = targetTelephone
            }
            fetchOrderList()
        })
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 20
        anchors.margins: 20

        // 搜索栏
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 12
            
            // 第一行：三个搜索框
            RowLayout {
                Layout.fillWidth: true
                spacing: 12
                
                // 手机号搜索
                FluTextBox {
                    id: searchField
                    placeholderText: "输入下单手机号搜索..."
                    Layout.fillWidth: true
                    iconSource: FluentIcons.Search
                    onTextChanged: {
                        // 12-28修改：手动输入时清除之前跳转带来的特定电话限制逻辑
                        if (text !== targetTelephone) targetTelephone = ""
                    }
                }
                
                // 订单ID搜索
                FluTextBox {
                    id: orderIdSearch
                    placeholderText: "输入订单ID搜索..."
                    Layout.fillWidth: true
                    iconSource: FluentIcons.Search
                    onTextChanged: {
                        searchOrderId = text
                    }
                }
                
                // 订单状态筛选 - 调整为不占用整排
                FluTextBox {
                    id: statusSearch
                    Layout.preferredWidth: 180
                    Layout.preferredHeight: 40
                    placeholderText: "点击选择订单状态..."
                    iconSource: FluentIcons.Option
                    readOnly: true
                    text: searchStatus !== "" ? searchStatus : ""
                    // 设置文本颜色，根据主题自动调整
                    normalColor: FluTheme.fontPrimaryColor || (FluTheme.dark ? "#ffffff" : "#000000")
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: {
                            statusPopup.open()
                        }
                    }
                }
            }
            
            // 第二行：操作按钮
            RowLayout {
                Layout.fillWidth: true
                spacing: 12
                Layout.alignment: Qt.AlignRight
                
                // 搜索按钮
                FluFilledButton {
                    text: "搜索"
                    onClicked: {
                        adminUserOrdersView.currentPage = 1
                        fetchOrderList()
                    }
                }
                
                // 重置按钮
                FluButton {
                    text: "重置/全部"
                    onClicked: {
                        searchField.text = ""
                        orderIdSearch.text = ""
                        searchStatus = ""
                        statusSearch.text = ""
                        targetTelephone = ""
                        adminUserOrdersView.currentPage = 1
                        fetchOrderList()
                    }
                }
            }
        }

        // 订单列表容器
        FluFrame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            padding: 10
            color: FluTheme.windowBackgroundColor || "#f0f2f5"

            ListView {
                id: orderListView
                anchors.fill: parent
                spacing: 12
                clip: true
                model: orderListModel

                delegate: AdminOrderCard {
                    width: orderListView.width - 20
                    orderId: model.orderId
                    flightNumber: model.flightNumber
                    passengerName: "用户ID: " + model.userId // 订单列表API返回的是userId
                    telephone: model.telephone
                    status: model.status
                }

                footer: FluText {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    text: orderListModel.count === 0 ? "暂无订单数据" : ""
                    color: FluTheme.secondaryTextColor || "#666666"
                    padding: 20
                }
            }
        }

        // 12-28修改：添加分页器
        RowLayout {
            Layout.fillWidth: true
            Item { Layout.fillWidth: true }
            FlightPagination {
                currentPage: adminUserOrdersView.currentPage
                totalPages: Math.max(1, Math.ceil(totalOrders / pageSize))
                totalRecords: totalOrders
                onPageChanged: function(page) {
                    adminUserOrdersView.currentPage = page
                    fetchOrderList()
                }
            }
            Item { Layout.fillWidth: true }
        }
    }
    
    // 订单状态下拉弹窗 - 移到ColumnLayout外面，作为FluPage的直接子组件
    FluContentDialog {
        id: statusPopup
        title: "选择订单状态"
        positiveText: "确定"
        negativeText: "取消"
        
        contentDelegate: Component {
            Rectangle {
                implicitWidth: 400
                implicitHeight: 250
                color: FluTheme.windowBackgroundColor || "#ffffff"
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 24
                    spacing: 12
                    
                    // 订单状态选项
                    FluButton {
                        text: "待支付"
                        Layout.fillWidth: true
                        onClicked: {
                            searchStatus = "待支付"
                            statusSearch.text = "待支付"
                            statusPopup.close()
                        }
                    }
                    
                    FluButton {
                        text: "已支付"
                        Layout.fillWidth: true
                        onClicked: {
                            searchStatus = "已支付"
                            statusSearch.text = "已支付"
                            statusPopup.close()
                        }
                    }
                    
                    FluButton {
                        text: "已退签"
                        Layout.fillWidth: true
                        onClicked: {
                            searchStatus = "已退签"
                            statusSearch.text = "已退签"
                            statusPopup.close()
                        }
                    }
                    
                    FluButton {
                        text: "已取消"
                        Layout.fillWidth: true
                        onClicked: {
                            searchStatus = "已取消"
                            statusSearch.text = "已取消"
                            statusPopup.close()
                        }
                    }
                    
                    FluButton {
                        text: "改签完成"
                        Layout.fillWidth: true
                        onClicked: {
                            searchStatus = "改签完成"
                            statusSearch.text = "改签完成"
                            statusPopup.close()
                        }
                    }
                }
            }
        }
        
        onPositiveClickListener: {
            statusPopup.close()
        }
    }
}
