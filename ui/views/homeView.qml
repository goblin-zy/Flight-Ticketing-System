import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import App.Network 1.0

FluPage {
    id: homeView
    title: "个人中心"

    // ----------------------------------------------------------
    // 【业务逻辑函数】 - 定义为独立函数，避开 QML 自动绑定引擎
    // ----------------------------------------------------------

    function handleLogout() {
        console.log("执行退出登录...")
        userContext.myToken = ""
        userContext.isAdmin = false
        Qt.callLater(function(){
            user_nav_view.push("qrc:/qt/FlightSystem/ui/views/LoginView.qml")
        })
    }

    function handleRecharge() {
        var amount = parseInt(rechargeDialog.amountText)
        if (amount > 0) {
            var rechargeAmount = amount * 100
            console.log("正在充值，本次充值金额(分): " + rechargeAmount)

            // 重要：如果这里持续报 404，请务必核对后端 Controller 的映射路径
            networkManager.request("/api/user/info/update", NetworkManager.POST, {
                // 向后端发送每次增加的值，而不是总值
                balance : rechargeAmount
            }, userContext.myToken)

            rechargeDialog.amountText = ""
        } else {
            console.error("UI提示: 请输入有效金额")
        }
    }

    function handleUpdateInfo() {
        var params = {}
        var newU = updateInfoDialog.newUsername.trim()
        if (newU !== "" && newU !== userContext.userName) params.username = newU

        if (updateInfoDialog.newPassword !== "") {
            if (updateInfoDialog.oldPassword === "") {
                console.error("UI提示: 缺少旧密码")
                return
            }
            params.password = updateInfoDialog.newPassword
            params.oldPassword = updateInfoDialog.oldPassword
        }

        if (Object.keys(params).length > 0) {
            networkManager.request("/api/user/info/update", NetworkManager.POST, params, userContext.myToken)
            updateInfoDialog.newPassword = ""
            updateInfoDialog.oldPassword = ""
        }
    }

    // 添加提示对话框
    FluContentDialog {
        id: infoDialog
        negativeText: "确定"
        buttonFlags: FluContentDialogType.NegativeButton
        // 2秒后自动关闭成功提示
        Component.onCompleted: {
            if (infoDialog.title === "操作成功") {
                Qt.callLater(function() {
                    infoDialog.close()
                }, 2000)
            }
        }
    }

    // ----------------------------------------------------------
    // 【初始化】 - 在这里进行手动赋值，解决所有语法和循环问题
    // ----------------------------------------------------------
    Component.onCompleted: {
        // 使用命令式赋值给属性，不会产生 Binding Loop，IDE也不会报错
        logoutDialog.onPositiveClickListener = handleLogout
        rechargeDialog.onPositiveClickListener = handleRecharge
        updateInfoDialog.onPositiveClickListener = handleUpdateInfo

        // 新增逻辑：进入页面时自动同步
        if (userContext.myToken.length > 0) {
            console.log("检测到登录状态，正在自动同步用户信息...")
            userContext.updateUserInfo()
        }
    }

    NetworkManager {
        id: networkManager
        onRequestSuccess: function(data) {
            if (data['code'] === 200) {
                console.log(" API 请求成功 --------------------")
                console.log("完整数据: \n" + JSON.stringify(data, null, 4))
                console.log("-----------------------------------\n")
                userContext.updateUserInfo()
                // 成功时关闭充值和修改信息对话框
                rechargeDialog.close()
                updateInfoDialog.close()
                // 显示成功提示
                infoDialog.title = "操作成功"
                infoDialog.message = "操作成功"
                infoDialog.open()
            } else {
                console.error(" API 业务失败: " + data['message'])
                // 失败时显示错误提示
                infoDialog.title = "操作失败"
                infoDialog.message = data['message']
                infoDialog.open()
            }
        }
        onRequestFailed: function(error) {
            console.error("\n🌐 网络错误详情: " + error)
            // 网络错误时显示提示
            infoDialog.title = "网络错误"
            infoDialog.message = "网络连接失败，请重试"
            infoDialog.open()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        FluFrame {
            Layout.fillWidth: true; Layout.preferredHeight: 200; radius: 12
            RowLayout {
                anchors.fill: parent; anchors.margins: 20; spacing: 20
                ColumnLayout {
                    spacing: 8; Layout.alignment: Qt.AlignVCenter
                    FluText { text: userContext.userName || "未登录"; font.pixelSize: 24; font.bold: true }
                    FluText { text: "ID: " + (userContext.userId || ""); font.pixelSize: 14; color: FluTheme.secondaryTextColor || "#666666" }
                    FluText { text: "电话: " + (userContext.myTelephone || ""); font.pixelSize: 16 }
                    FluText { text: "余额: ¥ " + (userContext.myMoney / 100).toFixed(2); font.pixelSize: 18; color: FluTheme.primaryColor || "#0078d4"; font.bold: true }
                }
            }
        }

        FluFrame {
            Layout.fillWidth: true; Layout.preferredHeight: 120; radius: 12
            RowLayout {
                anchors.fill: parent; anchors.margins: 20; spacing: 15
                FluFilledButton { text: "账户充值"; Layout.fillWidth: true; Layout.preferredHeight: 40; onClicked: rechargeDialog.open() }
                FluFilledButton { text: "修改信息"; Layout.fillWidth: true; Layout.preferredHeight: 40; onClicked: updateInfoDialog.open() }
                FluFilledButton { text: "我的订单"; Layout.fillWidth: true; Layout.preferredHeight: 40; onClicked: user_nav_view.push("qrc:/qt/FlightSystem/ui/views/OrdersView.qml") }
            }
        }

        FluFrame {
            Layout.fillWidth: true; Layout.preferredHeight: 100; radius: 12
            RowLayout {
                anchors.fill: parent; anchors.margins: 20
                Loader {
                    Layout.fillWidth: true; Layout.preferredHeight: 40
                    sourceComponent: userContext.myToken ? logoutButton : loginButton
                }
            }
        }
    }

    FluContentDialog {
        id: logoutDialog
        title: "确认退出登录?"
        message: "退出后需要重新登录才能使用所有功能"
        positiveText: "确认"
        negativeText: "取消"
        // 此处不再写 onPositiveClickListener，由 Component.onCompleted 赋值
    }

    FluContentDialog {
        id: rechargeDialog
        title: "账户充值"
        positiveText: "确认充值"
        negativeText: "取消"
        property string amountText: ""
        contentDelegate: Component {
            Rectangle {
                implicitWidth: 400; implicitHeight: 200; radius: 8
                color: FluTheme.windowBackgroundColor || "#ffffff"
                ColumnLayout {
                    anchors.fill: parent; anchors.margins: 24; spacing: 15
                    FluText { text: "充值金额 (¥)"; font.pixelSize: 14 }
                    FluTextBox {
                        placeholderText: "请输入整数金额"; Layout.fillWidth: true
                        validator: IntValidator { bottom: 1; top: 10000 }
                        onTextChanged: rechargeDialog.amountText = text
                    }
                }
            }
        }
    }

    FluContentDialog {
        id: updateInfoDialog
        title: "修改个人信息"
        positiveText: "确认修改"
        negativeText: "取消"
        property string newUsername: ""
        property string newPassword: ""
        property string oldPassword: ""
        contentDelegate: Component {
            Rectangle {
                implicitWidth: 400; implicitHeight: layout_update.implicitHeight + 48; radius: 8
                color: FluTheme.windowBackgroundColor || "#ffffff"
                ColumnLayout {
                    id: layout_update; anchors.fill: parent; anchors.margins: 24; spacing: 15
                    FluTextBox { text: userContext.userName; Layout.fillWidth: true; onTextChanged: updateInfoDialog.newUsername = text }
                    FluTextBox { placeholderText: "新密码"; echoMode: TextInput.Password; Layout.fillWidth: true; onTextChanged: updateInfoDialog.newPassword = text }
                    FluTextBox { placeholderText: "旧密码"; echoMode: TextInput.Password; Layout.fillWidth: true; onTextChanged: updateInfoDialog.oldPassword = text }
                }
            }
        }
    }

    Component { id: logoutButton; FluFilledButton { text: "退出登录"; normalColor: FluTheme.errorColor || "#ff4d4f"; onClicked: logoutDialog.open() } }
    Component { id: loginButton; FluFilledButton { text: "立即登录/注册"; normalColor: FluTheme.primaryColor || "#0078d4"; onClicked: Qt.callLater(function(){ user_nav_view.push("qrc:/qt/FlightSystem/ui/views/LoginView.qml") }) } }
}
