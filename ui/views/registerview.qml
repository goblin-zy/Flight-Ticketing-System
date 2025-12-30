import QtQuick 2.15
import FluentUI 1.0
import App.Network 1.0

FluPage {
    id: registerView

    width: parent.width
    height: parent.height

    // 添加提示对话框
    FluContentDialog {
        id: infoDialog
        negativeText: "确定"
        buttonFlags: FluContentDialogType.NegativeButton
    }

    // 注册请求处理
    NetworkManager {
        id: registerHandler

        // 请求成功处理
        onRequestSuccess: function(data) {
            if (data['code'] === 200) {
                console.log("注册成功，开始自动登录")
                // 注册成功后自动登录
                loginHandler.request("/api/user/login", NetworkManager.POST, {
                    telephone: phoneField.text,
                    password: passwordField.text
                })
            } else {
                console.error("注册失败:", data['message'])
                infoDialog.title = data['message']
                //infoDialog.message = data['message']
                infoDialog.open()
            }
        }

        // 请求失败处理
        onRequestFailed: function(error) {
            console.log(JSON.stringify(error))
            infoDialog.title = "网络连接错误"
          //  infoDialog.message = "网络连接错误"
            infoDialog.open()
        }
    }

    // 注册成功后自动登录的处理
    NetworkManager {
        id: loginHandler

        // 请求成功处理
        onRequestSuccess: function(data) {
            if (data['code'] === 200) {
                console.log("注册后自动登录成功，token:", data['data']['token'])
                // 先设置身份为普通用户，再保存token
                userContext.isAdmin = false
                // 保存token到用户上下文
                userContext.myToken = data['data']['token']
                // 直接从返回数据中赋值用户信息
                userContext.userId = data['data']['userId'].toString()
                userContext.userName = data['data']['username']
                userContext.myTelephone = data['data']['telephone']

                // 返回登录界面
                // 修改后
                console.log("注册并登录成功，跳转首页")
                user_nav_view.push("qrc:/qt/FlightSystem/ui/views/homeView.qml")
            } else {
                console.error("注册后自动登录失败:", data['message'])
                infoDialog.title = "自动登录失败，请手动登录"
                //infoDialog.message = "自动登录失败，请手动登录"
                infoDialog.open()
            }
        }

        // 请求失败处理
        onRequestFailed: function(error) {
            console.log(JSON.stringify(error))
            infoDialog.title = "网络连接错误"
            //infoDialog.message = "网络连接错误"
            infoDialog.open()
        }
    }

    Column {
        spacing: 20
        anchors.centerIn: parent
        width: parent.width
        height: parent.height

        Rectangle{
            anchors.horizontalCenter: parent.horizontalCenter  // 确保水平居中
            width:280
            height:150
            radius: 45
            clip: true
            Image {
                anchors.fill: parent
                source: "../../assets/LoginImage.png"
            }
        }

        FluTextBox {
            anchors.horizontalCenter: parent.horizontalCenter  // 确保水平居中
            id: phoneField
            placeholderText: "电话号码"
            width: parent.width * 0.8
            focus: true
        }

        FluTextBox {
            anchors.horizontalCenter: parent.horizontalCenter  // 确保水平居中
            id: userNameField
            placeholderText: "用户名"
            width: parent.width * 0.8
        }

        FluTextBox {
            anchors.horizontalCenter: parent.horizontalCenter  // 确保水平居中
            id: passwordField
            placeholderText: "密码"
            width: parent.width * 0.8
            echoMode: TextInput.Password
        }

        FluTextBox {
            anchors.horizontalCenter: parent.horizontalCenter  // 确保水平居中
            id: confirmPasswordField
            placeholderText: "确认密码"
            width: parent.width * 0.8
            echoMode: TextInput.Password
        }

        FluButton {
            anchors.horizontalCenter: parent.horizontalCenter  // 确保水平居中
            text: "注册"
            width: parent.width * 0.8
            enabled: phoneField.text.length>0 && userNameField.text.length>0 && passwordField.text.length>0 && passwordField.text === confirmPasswordField.text
            onClicked: {
                console.log("注册按钮点击，开始注册请求")
                // 根据新API文档，注册接口应该是/api/user/register，使用POST方法
                registerHandler.request("/api/user/register", NetworkManager.POST, {
                    telephone: phoneField.text,
                    username: userNameField.text,
                    password: passwordField.text
                })
            }
        }

        // 登录选项
        MouseArea {
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width * 0.8
            height: 30
            onClicked: {
                // 跳转到登录界面
                user_nav_view.push("qrc:/qt/FlightSystem/ui/views/LoginView.qml")
            }
            
            Text {
                anchors.centerIn: parent
                text: "已有账号？立即登录"
                font.pixelSize: 13
                color: FluTheme.primaryColor
                font.underline: true
            }
        }
    }
}
