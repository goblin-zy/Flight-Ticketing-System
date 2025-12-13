#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

// 引入头文件 (保持你原有的引用方式)
#include "NetworkManager.h"
#include "UserContext.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // 1. 创建核心 C++ 对象
    // 必须先创建 NetworkManager，因为 UserContext 依赖它
    NetworkManager *networkManager = new NetworkManager(&app);

    // 【关键修改】初始化 UserContext 并传入 networkManager
    // 这样 UserContext 内部就可以调用 m_manager->request(...) 来自动更新数据了
    UserContext *userContext = new UserContext(networkManager, &app);

    // 【建议增加】设置初始默认状态 (防止刚启动时界面显示空白)
    userContext->setUserName("旅客");
    userContext->setMyMoney(0);
    // 注意：请确保你项目 qrc 中有对应的默认头像图片，如果没有可以先注释掉下面这行
    // userContext->setMyAvatar("qrc:/qt/FlightSystem/figures/default_avatar.png");

    // 2. 注入到 QML 全局上下文
    // 这样 QML 里可以直接通过 id 访问这两个 C++ 对象
    engine.rootContext()->setContextProperty("networkManager", networkManager);
    engine.rootContext()->setContextProperty("userContext", userContext);

    // 3. 注册类型
    // 允许在 QML 中使用 NetworkManager { ... } 进行独立实例化
    qmlRegisterType<NetworkManager>("App.Network", 1, 0, "NetworkManager");

    // 4. 加载主界面 (保持你原有的路径)
    const QUrl url(QStringLiteral("qrc:/qt/FlightSystem/ui/Main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
