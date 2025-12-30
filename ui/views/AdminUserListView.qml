import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import App.Network 1.0
import "../component"

FluPage {
    id: adminUserListView
    title: qsTr("用户列表管理")

    property int totalUsers: 0
    property int currentPage: 1
    property int pageSize: 10
    property string searchUserId: ""
    property string searchUsername: ""
    property string searchTelephone: ""

    NetworkManager {
        id: networkManager
        onRequestSuccess: function(data) {
            console.log("\n 用户列表请求成功 --------------------")
            console.log("完整返回数据: ", JSON.stringify(data, null, 4))
            console.log("-----------------------------------\n")
            if (data['code'] === 200) {
                userListModel.clear()
                if (data['data'] && data['data']['list']) {
                    var users = data['data']['list']
                    for (var i = 0; i < users.length; i++) {
                        var user = users[i]
                        userListModel.append({
                            userId: user['userId'] ? user['userId'].toString() : '',
                            userName: user['username'] || '',
                            telephone: user['telephone'] || ''
                        })
                    }
                }
                totalUsers = data['data']['total'] || 0
            } else {
                console.error(" API业务错误 --------------------")
                console.error("错误代码: " + data['code'] + " | 错误信息: " + (data['message'] || "未知错误"))
                console.error("-----------------------------------\n")
            }
        }
        onRequestFailed: function(error) {
            console.error("\n 用户列表请求失败 --------------------")
            console.error("失败原因: " + error)
            console.error("-----------------------------------\n")
        }
    }

    ListModel { id: userListModel }

    function fetchUserList() {
        var params = { pageNum: currentPage, pageSize: pageSize }
        if (searchUserId.trim() !== "") params.userId = searchUserId.trim()
        if (searchUsername.trim() !== "") params.username = searchUsername.trim()
        if (searchTelephone.trim() !== "") params.telephone = searchTelephone.trim()
        console.log("12-28修改：正在发送用户列表请求，路径：/api/admin/user/all，参数：", JSON.stringify(params))
        networkManager.request("/api/admin/user/all", NetworkManager.GET, params, userContext.myToken)
    }

    Component.onCompleted: fetchUserList()

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        FluFrame {
            Layout.fillWidth: true
            padding: 16
            color: FluTheme.windowBackgroundColor || "#f0f2f5"
            RowLayout {
                spacing: 12
                Layout.fillWidth: true
                FluTextBox {
                    id: userIdSearch
                    placeholderText: "输入用户ID"
                    Layout.fillWidth: true
                    onTextChanged: { searchUserId = text }
                }
                FluTextBox {
                    id: usernameSearch
                    placeholderText: "输入用户名"
                    Layout.fillWidth: true
                    onTextChanged: { searchUsername = text }
                }
                FluTextBox {
                    id: telephoneSearch
                    placeholderText: "输入电话号码"
                    Layout.fillWidth: true
                    onTextChanged: { searchTelephone = text }
                }
                FluFilledButton {
                    text: "搜索"
                    onClicked: { currentPage = 1; fetchUserList() }
                }
                FluButton {
                    text: "重置"
                    onClicked: {
                        userIdSearch.text = ""; usernameSearch.text = ""; telephoneSearch.text = ""
                        searchUserId = ""; searchUsername = ""; searchTelephone = ""
                        currentPage = 1; fetchUserList()
                    }
                }
            }
        }

        FluFrame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            padding: 16
            color: FluTheme.windowBackgroundColor || "#f0f2f5"
            ListView {
                id: userListView
                anchors.fill: parent
                spacing: 12
                clip: true
                model: userListModel
                delegate: AdminUserCard {
                    width: userListView.width
                    userId: model.userId
                    userName: model.userName
                    telephone: model.telephone

                    // 12-28修改：使用回调属性替代信号
                    onCardClicked: function() {
                        console.log("12-28修改：点击用户，正在跳转至订单页，参数电话：" + model.telephone)
                        admin_nav_view.push("qrc:/qt/FlightSystem/ui/views/AdminUserOrdersView.qml",
                                           { "targetTelephone": model.telephone })
                    }
                }
                footer: FluText {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    text: userListModel.count === 0 ? "暂无匹配用户" : ""
                    color: FluTheme.secondaryTextColor || "#666666"
                    padding: 20
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Item { Layout.fillWidth: true }
            FlightPagination {
                currentPage: adminUserListView.currentPage
                totalPages: Math.max(1, Math.ceil(totalUsers / pageSize))
                totalRecords: totalUsers
                onPageChanged: function(page) {
                    adminUserListView.currentPage = page
                    fetchUserList()
                }
            }
            Item { Layout.fillWidth: true }
        }
    }
}
