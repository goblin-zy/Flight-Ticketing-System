import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0

FluWindow {
    id: window
    visible: true
    width: 1024
    height: 640
    minimumWidth: 800
    minimumHeight: 500
    title: "Flight Management System"
    
    // 定义UI状态变量：控制外壳显隐
    property bool isDisplayingAdminMode: false

    // 顶部切换开关
    RowLayout {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.rightMargin: 40
        anchors.topMargin: 10
        spacing: 10
        z: 999 // 设置高层级，确保始终可见且可点击
        FluToggleSwitch {
            id: toggleSwitch
            checked: isDisplayingAdminMode // 双向绑定，确保开关状态与属性同步
            Layout.alignment: Qt.AlignVCenter
            text: isDisplayingAdminMode ? "切换为用户端" : "切换为管理员端"
            
            onCheckedChanged: {
                isDisplayingAdminMode = checked;
                console.log("当前模式: " + (isDisplayingAdminMode ? "管理员端" : "用户端"));
            }
        }
    }

    // 用户端视图
    FluNavigationView {
        id: user_nav_view
        anchors.fill: parent
        // 使用FluentUI自带的图标作为logo
        logo: FluentIcons.Airplane
        title: "FMS 系统 - 用户端"
        displayMode: FluNavigationViewType.Auto
        visible: !isDisplayingAdminMode

        // 【核心菜单区域】
        items: FluPaneItemExpander {
            title: "功能菜单"
            iconVisible: false // 隐藏图标，使其仅作为分组容器

            FluPaneItem {
                title: "航班"
                icon: FluentIcons.Airplane
                // 点击后加载 FlightInfoView.qml
                url: "qrc:/qt/FlightSystem/ui/views/FlightInfoView.qml"
                onTap: {
                    if (userContext.myToken.length === 0) {
                        showInfo("请先登录")
                        user_nav_view.push("qrc:/qt/FlightSystem/ui/views/LoginView.qml")
                    } else {
                        user_nav_view.push(url)
                    }
                }
            }

            FluPaneItem {
                title: "我的订单"
                icon: FluentIcons.ShoppingCart
                url: "qrc:/qt/FlightSystem/ui/views/OrdersView.qml"
                onTap: {
                    if (userContext.myToken.length === 0) {
                        showInfo("请先登录")
                        user_nav_view.push("qrc:/qt/FlightSystem/ui/views/LoginView.qml")
                    } else {
                        user_nav_view.push(url)
                    }
                }
            }

            FluPaneItem {
                title: "个人中心"
                icon: FluentIcons.Contact
                // 使用现有的homeView.qml文件
                url: "qrc:/qt/FlightSystem/ui/views/homeView.qml"
                onTap: {
                    if (userContext.myToken.length === 0) {
                        showInfo("请先登录")
                        user_nav_view.push("qrc:/qt/FlightSystem/ui/views/LoginView.qml")
                    } else {
                        user_nav_view.push(url)
                    }
                }
            }
        }

        // 【底部菜单区域】
        footerItems: FluPaneItemExpander {
            title: "设置"
            iconVisible: false

            FluPaneItem {
                // 根据当前主题动态显示文字
                title: FluTheme.dark ? "切换到日间模式" : "切换到夜间模式"
                // 根据当前主题动态显示图标
                icon: FluTheme.dark ? FluentIcons.Brightness : FluentIcons.QuietHours
                onTap: {
                    // 切换日夜间模式的逻辑
                    FluTheme.darkMode = FluTheme.dark ? FluThemeType.Light : FluThemeType.Dark
                }
            }

            // 分割线
            FluPaneItemSeparator {}

            FluPaneItem {
                title: "登录"
                icon: FluentIcons.SignIn
                url: "qrc:/qt/FlightSystem/ui/views/LoginView.qml"
                onTap: {
                    user_nav_view.push(url)
                }
            }
        }

        // 【初始化逻辑】
        Component.onCompleted: {
            user_nav_view.setCurrentIndex(0)
            // 强制设置默认主题为白天模式
            FluTheme.darkMode = FluThemeType.Light
            // 安全拦截：未登录状态下强制跳转登录页
            Qt.callLater(function() {
                if (userContext.myToken.length === 0) {
                     console.log("未登录状态，强制跳转登录页")
                    user_nav_view.push("qrc:/qt/FlightSystem/ui/views/LoginView.qml")
                } else {
                    user_nav_view.push("qrc:/qt/FlightSystem/ui/views/FlightInfoView.qml")
                }
            })
        }
    }
    








    // 管理员端视图
    FluNavigationView {
        id: admin_nav_view
        anchors.fill: parent
        // 使用FluentUI自带的图标作为logo
        logo: FluentIcons.Airplane
        title: "FMS 系统 - 管理员端"
        displayMode: FluNavigationViewType.Auto
        visible: isDisplayingAdminMode

        // 【核心菜单区域】
        items: FluPaneItemExpander {
            title: "功能菜单"
            iconVisible: false // 隐藏图标，使其仅作为分组容器

            FluPaneItem {
                title: qsTr("航班管理")
                icon: FluentIcons.Airplane
                // 对应API 2.2.1 (搜索) 和 2.2.3 (修改)
                url: "qrc:/qt/FlightSystem/ui/views/FlightInfoEdit.qml"
                onTap: {
                    // 拦截逻辑：检查管理员权限
                    if (userContext.myToken.length === 0 || !userContext.isAdmin) {
                        showError("需管理员权限，请登录管理员账号")
                        admin_nav_view.push("qrc:/qt/FlightSystem/ui/views/adminLogin.qml")
                    } else {
                        admin_nav_view.push(url)
                    }
                }
            }

            FluPaneItem {
                title: qsTr("用户列表管理")
                icon: FluentIcons.People
                // 管理员查看所有客户信息 (API 2.3.2)
                url: "qrc:/qt/FlightSystem/ui/views/AdminUserListView.qml"
                onTap: {
                     //拦截逻辑：检查管理员权限
                    if (userContext.myToken.length === 0 || !userContext.isAdmin) {
                        showError("需管理员权限，请登录管理员账号")
                        admin_nav_view.push("qrc:/qt/FlightSystem/ui/views/AdminUserListView.qml")
                   } else {
                        admin_nav_view.push(url)
                    }
                }
            }
            
            FluPaneItem {
                title: qsTr("订单列表管理")
                icon: FluentIcons.ShoppingCart
                // 管理员查看所有订单信息 (API 2.3.3)
                url: "qrc:/qt/FlightSystem/ui/views/AdminUserOrdersView.qml"
                onTap: {
                    // 拦截逻辑：检查管理员权限
                    if (userContext.myToken.length === 0 || !userContext.isAdmin) {
                        showError("需管理员权限，请登录管理员账号")
                        admin_nav_view.push("qrc:/qt/FlightSystem/ui/views/AdminUserOrderView.qml")
                    } else {
                        admin_nav_view.push(url)
                    }
                }
            }
        }

        // 【底部菜单区域】
        footerItems: FluPaneItemExpander {
            title: "设置"
            iconVisible: false

            FluPaneItem {
                // 根据当前主题动态显示文字
                title: FluTheme.dark ? "切换到日间模式" : "切换到夜间模式"
                // 根据当前主题动态显示图标
                icon: FluTheme.dark ? FluentIcons.Brightness : FluentIcons.QuietHours
                onTap: {
                    // 切换日夜间模式的逻辑
                    FluTheme.darkMode = FluTheme.dark ? FluThemeType.Light : FluThemeType.Dark
                }
            }

            // 分割线
            FluPaneItemSeparator {}

            FluPaneItem {
                title: qsTr("管理员登录")
                icon: FluentIcons.SignIn
                url: "qrc:/qt/FlightSystem/ui/views/adminLogin.qml"
                onTap: {
                    admin_nav_view.push(url)
                }
            }
        }

        // 【初始化逻辑】
        Component.onCompleted: {
            admin_nav_view.setCurrentIndex(0)
            // 安全拦截：未登录或非管理员状态下强制跳转管理员登录页
            Qt.callLater(function() {
                if (userContext.myToken.length === 0 || !userContext.isAdmin) {
                    admin_nav_view.push("qrc:/qt/FlightSystem/ui/views/adminLogin.qml")
                } else {
                    admin_nav_view.push("qrc:/qt/FlightSystem/ui/views/FlightInfoEdit.qml")
                }
            })
        }
    }
}
