import QtQuick 2.15
import FluentUI 1.0
import App.Network 1.0

FluPage {
    id: adminLoginView

    width: parent.width
    height: parent.height

    // 登录请求处理
    NetworkManager {
        id: loginHandler

        // 请求成功处理
        onRequestSuccess: function(data) {
            if (data['code'] === 200) {
                console.log("管理员登录成功，token:", data['data']['token'])
                // 先设置身份为管理员，再保存token
                userContext.isAdmin = true
                // 保存token到用户上下文，触发自动解析机制
                userContext.myToken = data['data']['token']
                
                // 登录成功提示
                console.log("UI提示: 管理员登录成功！")
                
                // 【删除或注释这两行】不要在这里直接跳转
                // admin_nav_view.push("qrc:/qt/FlightSystem/ui/views/FlightInfoEdit.qml")
                // admin_nav_view.setCurrentIndex(0)
            } else {
                console.error("管理员登录失败:", data['message'])
                console.error("UI提示: " + data['message'])
            }
        }

        // 请求失败处理
        onRequestFailed: function(error) {
            console.log(JSON.stringify(error))
            console.error("UI提示: " + error)
        }
    }

    // 监听 C++ 发出的“数据就绪”信号
    Connections {
        target: userContext
        function onUserInfoReady() {
            if (adminLoginView.visible) {
                console.log("收到数据就绪信号，执行跳转...")
                Qt.callLater(function(){
                    // 【核心修改】只调用这一行！
                    // 不要调用 admin_nav_view.push(...)
                    // setCurrentIndex(0) 会触发 MainWindow 的 onTap，自动完成 push
                    admin_nav_view.setCurrentIndex(0)
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
            id: accountBox
            placeholderText: "管理员账号"
            width: parent.width * 0.8
            focus: true
        }

        FluTextBox {
            anchors.horizontalCenter: parent.horizontalCenter  // 确保水平居中
            id: passwordBox
            placeholderText: "密码"
            width: parent.width * 0.8
            echoMode: TextInput.Password
        }

        FluButton {
            anchors.horizontalCenter: parent.horizontalCenter  // 确保水平居中
            text: "管理员登录"
            width: parent.width * 0.8
            enabled: accountBox.text.length>0 && passwordBox.text.length>0
            onClicked: {
                console.log("管理员登录按钮点击，开始登录请求")
                // 管理员登录接口：/api/admin/login，使用POST方法
                loginHandler.request("/api/admin/login", NetworkManager.POST, {
                    account: accountBox.text,
                    password: passwordBox.text
                })
            }
        }
    }
}
