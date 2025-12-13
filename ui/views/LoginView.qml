import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import FluentUI 1.0

FluPage {
    id: loginPage
    title: "用户登录"
    backgroundColor: FluTheme.backgroundColor
    visible: true
    launchMode: FluPageType.Standard

    Column {
        anchors.centerIn: parent
        spacing: 30
        width: parent.width * 0.3
        minimumWidth: 400

        // Logo区域
        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 15

            FluIcon {
                icon: FluentIcons.Airplane
                size: 64
                color: FluTheme.primaryColor
            }

            Text {
                text: "航班管理系统"
                font.pixelSize: 24
                font.bold: true
                color: FluTheme.primaryTextColor
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        // 登录表单
        Column {
            width: parent.width
            spacing: 20

            // 电话号码输入框
            Column {
                spacing: 8

                FluTextBlock {
                    text: "电话号码"
                    fontSize: 14
                    color: FluTheme.primaryTextColor
                }

                FluTextBox {
                    id: phoneInput
                    width: parent.width
                    placeholderText: "请输入电话号码"
                    focus: true

                    // 简单的电话号码验证
                    validator: RegExpValidator {
                        regExp: /^1[3-9]\d{9}$/
                    }
                }
            }

            // 密码输入框
            Column {
                spacing: 8

                FluTextBlock {
                    text: "密码"
                    fontSize: 14
                    color: FluTheme.primaryTextColor
                }

                FluPasswordBox {
                    id: passwordInput
                    width: parent.width
                    placeholderText: "请输入密码"

                    // 密码输入完成后按回车登录
                    onAccepted: {
                        if (loginButton.enabled) {
                            loginButton.clicked()
                        }
                    }
                }
            }

            // 登录按钮
            FluButton {
                id: loginButton
                width: parent.width
                text: "登录"
                isDefaultButton: true
                enabled: (phoneInput.text.length === 11 && passwordInput.text.length >= 6)

                onClicked: {
                    // 登录成功逻辑
                    console.log("登录按钮点击")
                    showSuccess(qsTr("登录成功"))
                    // 登录成功后跳转到个人中心页面
                    nav_view.push("qrc:/qt/FlightSystem/ui/views/homeView.qml")
                    nav_view.setCurrentIndex(2) // 设置个人中心为当前选中菜单
                }
            }

            // 注册选项
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 8

                Text {
                    text: "还没有账号？"
                    font.pixelSize: 13
                    color: FluTheme.secondaryTextColor
                }

                Text {
                    text: "立即注册"
                    font.pixelSize: 13
                    color: FluTheme.primaryColor
                    font.underline: true
                    cursorShape: Qt.PointingHandCursor

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            // 跳转到注册界面
                            nav_view.push("qrc:/qt/FlightSystem/ui/views/registerview.qml")
                        }
                    }
                }
            }
        }
    }
}