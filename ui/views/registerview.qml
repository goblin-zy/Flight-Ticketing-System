import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import FluentUI 1.0

FluPage {
    id: registerPage
    title: "用户注册"
    backgroundColor: FluTheme.backgroundColor

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

        // 注册表单
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
                    id: regPhoneInput
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
                    id: regPasswordInput
                    width: parent.width
                    placeholderText: "请输入密码（至少6位）"
                }
            }

            // 确认密码输入框
            Column {
                spacing: 8

                FluTextBlock {
                    text: "确认密码"
                    fontSize: 14
                    color: FluTheme.primaryTextColor
                }

                FluPasswordBox {
                    id: regConfirmPasswordInput
                    width: parent.width
                    placeholderText: "请再次输入密码"

                    // 确认密码输入完成后按回车注册
                    onAccepted: {
                        if (registerButton.enabled) {
                            registerButton.clicked()
                        }
                    }
                }
            }

            // 注册按钮
            FluButton {
                id: registerButton
                width: parent.width
                text: "注册"
                isDefaultButton: true
                enabled: (regPhoneInput.text.length === 11 && 
                           regPasswordInput.text.length >= 6 && 
                           regPasswordInput.text === regConfirmPasswordInput.text)

                onClicked: {
                    // 注册逻辑，暂时只做界面跳转
                    console.log("注册按钮点击")
                    // 注册成功后返回登录界面
                    nav_view.pop()
                }
            }

            // 登录选项
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 8

                Text {
                    text: "已有账号？"
                    font.pixelSize: 13
                    color: FluTheme.secondaryTextColor
                }

                Text {
                    text: "立即登录"
                    font.pixelSize: 13
                    color: FluTheme.primaryColor
                    font.underline: true
                    cursorShape: Qt.PointingHandCursor

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            // 返回登录界面
                            nav_view.pop()
                        }
                    }
                }
            }
        }
    }
}