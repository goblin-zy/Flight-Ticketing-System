import QtQuick 2.15
import FluentUI 1.0
import App.Network 1.0

FluPage {
    id: loginView

    width: parent.width
    height: parent.height

    // 添加错误对话框
    FluContentDialog {
        id: errorDialog
        negativeText: "确定"
        // 设置按钮类型，这是FluContentDialog正常显示的必要属性
        buttonFlags: FluContentDialogType.NegativeButton
        // 使用contentDelegate自定义内容，确保消息能正确显示
        contentDelegate: Component {
            Column {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 15
                FluText {
                    text: errorDialog.title
                    font.bold: true
                    font.pixelSize: 16
                }
                FluText {
                    text: errorDialog.message
                    font.pixelSize: 14
                }
            }
        }
    }

    // 登录请求处理
    NetworkManager {
        id: loginHandler

        // 请求成功处理
        onRequestSuccess: function(data) {
            if (data['code'] === 200) {
                console.log("登录成功，token:", data['data']['token'])
                // 先设置身份为普通用户，再保存token
                userContext.isAdmin = false
                userContext.userId = data['data']['userId'].toString()
                userContext.userName = data['data']['username']
                userContext.myTelephone = data['data']['telephone']
                // 从API返回数据中获取余额并设置到userContext
                userContext.myMoney = parseInt(data['data']['balance'])
                // 保存token到用户上下文
                userContext.myToken = data['data']['token']

                // 【删除或注释这两行】不要在这里直接跳转
                // user_nav_view.push("qrc:/qt/FlightSystem/ui/views/homeView.qml")
                // user_nav_view.setCurrentIndex(2)
            } else {
                console.error("登录失败:", data['message'])
                errorDialog.title = data['message']
                errorDialog.open()
            }
        }

        // 请求失败处理
        onRequestFailed: function(error) {
            console.log(JSON.stringify(error))
            errorDialog.title = "网络连接错误"
            errorDialog.open()
        }
    }

    // 监听 C++ 发出的“数据就绪”信号
    Connections {
        target: userContext
        function onUserInfoReady() {
            if (loginView.visible) {
                console.log("收到数据就绪信号，执行跳转...")
                Qt.callLater(function(){
                    // 【核心修改】只调用这一行！
                    // 不要调用 user_nav_view.push(...)
                    // setCurrentIndex(2) 会触发 MainWindow 的 onTap，自动完成 push
                    user_nav_view.setCurrentIndex(2)
                })
            }
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
            id: passwordField
            placeholderText: "密码"
            width: parent.width * 0.8
            echoMode: TextInput.Password
        }

        FluButton {
            anchors.horizontalCenter: parent.horizontalCenter  // 确保水平居中
            text: "登录"
            width: parent.width * 0.8
            enabled: phoneField.text.length>0 && passwordField.text.length>0
            onClicked: {
                console.log("登录按钮点击，开始登录请求")
                // 根据新API文档，登录接口应该是/api/user/login，使用POST方法
                loginHandler.request("/api/user/login", NetworkManager.POST, {
                    telephone: phoneField.text,
                    password: passwordField.text
                })
            }
        }

        // 注册选项
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width * 0.8
            height: 30
            color: "transparent"

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    // 跳转到注册界面
                    user_nav_view.push("qrc:/qt/FlightSystem/ui/views/registerview.qml")
                }
            }

            Text {
                anchors.centerIn: parent
                text: "还没有账号？立即注册"
                font.pixelSize: 13
                color: FluTheme.primaryColor
                font.underline: true
            }
        }
    }
}
