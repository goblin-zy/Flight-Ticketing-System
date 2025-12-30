import QtQuick
import FluentUI

FluLauncher {
    id: app

    Component.onCompleted: {
        // 1. 初始化框架
        FluApp.init(app)

        // 2. 全局基础配置
        FluTheme.darkMode = FluThemeType.Light
        FluTheme.primaryColor = FluColors.Blue
        FluTheme.nativeText = true
        FluTheme.animationEnabled = true
        FluApp.useSystemAppBar = false

        // ------------------------------------------------------------------
        // 【核心修改】 使用 FluRouter (路由管理器) 而不是 FluApp 来配置跳转
        // ------------------------------------------------------------------

        // 3. 配置路由表
        FluRouter.routes = {
            // 根路径指向主窗口
            "/": "qrc:/qt/FlightSystem/ui/MainWindow.qml",
        }

        // 4. 执行跳转 (替代原来的 FluApp.run())
        FluRouter.navigate("/")
    }
}
