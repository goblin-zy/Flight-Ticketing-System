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

    FluNavigationView {
        id: nav_view
        anchors.fill: parent
        // 使用FluentUI自带的图标作为logo
        logo: FluentIcons.Airplane
        title: "FMS 系统"
        displayMode: FluNavigationViewType.Auto

        // 【核心菜单区域】
        items: FluPaneItemExpander {
            title: "功能菜单"
            iconVisible: false // 隐藏图标，使其仅作为分组容器

            FluPaneItem {
                title: "航班"
                icon: FluentIcons.Airplane
                // 点击后加载 FlightInfoView.qml
                url: "qrc:/qt/FlightSystem/ui/views/FlightInfoView.qml"
                onTap: { nav_view.push(url) }
            }

            FluPaneItem {
                title: "我的订单"
                icon: FluentIcons.ShoppingCart
                url: "qrc:/qt/FlightSystem/ui/views/OrdersView.qml"
                onTap: { nav_view.push(url) }
            }

            FluPaneItem {
                title: "个人中心"
                icon: FluentIcons.Contact
                // 使用现有的homeView.qml文件
                url: "qrc:/qt/FlightSystem/ui/views/homeView.qml"
                onTap: { nav_view.push(url) }
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
                    nav_view.push(url)
                }
            }
        }

        // 【初始化逻辑】
        Component.onCompleted: {
            nav_view.setCurrentIndex(0)
            // 默认加载航班信息页面作为首页
            nav_view.push("qrc:/qt/FlightSystem/ui/views/FlightInfoView.qml")
        }
    }
}
