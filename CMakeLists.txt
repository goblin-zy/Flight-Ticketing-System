cmake_minimum_required(VERSION 3.16)

# 项目名称
project(FlightSystem VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------
# 【步骤 1】 先配置 Qt 环境 (顺序非常重要！)
# ---------------------------------------------------------
find_package(Qt6 6.8 REQUIRED COMPONENTS Quick Network Svg Core5Compat Concurrent)

qt_standard_project_setup(REQUIRES 6.8)

# ---------------------------------------------------------
# 【步骤 2】 再引入 FluentUI (必须在 Qt 环境配置之后)
# ---------------------------------------------------------
# 1. 强制关闭示例和测试
set(FLUENTUI_BUILD_EXAMPLES OFF CACHE BOOL "Disable FluentUI examples" FORCE)
set(FLUENTUI_BUILD_TESTS OFF CACHE BOOL "Disable FluentUI tests" FORCE)

# 2. 开启静态编译
set(FLUENTUI_BUILD_STATIC_LIB ON CACHE BOOL "Build static library" FORCE)

# 3. 引入本地源码 (移到这里！)
add_subdirectory(FluentUI)

# ---------------------------------------------------------
# 【步骤 3】 项目源文件配置
# ---------------------------------------------------------
set(QML_FILES
    ui/Main.qml
    ui/MainWindow.qml
    ui/views/LoginView.qml
    ui/views/registerview.qml
    ui/views/FlightInfoView.qml
    ui/views/OrdersView.qml
    ui/views/homeView.qml
)

set(SOURCE_FILES
    main.cpp
    src/cpp/NetworkManager.h
    src/cpp/NetworkManager.cpp
    src/cpp/UserContext.h
    src/cpp/UserContext.cpp
)

qt_add_executable(${PROJECT_NAME}
    ${SOURCE_FILES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cpp
)

qt_add_qml_module(${PROJECT_NAME}
    URI FlightSystem
    VERSION 1.0
    RESOURCE_PREFIX /qt
    QML_FILES ${QML_FILES}
)

# ---------------------------------------------------------
# 【步骤 4】 链接库
# ---------------------------------------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Quick
    Qt6::Network
    Qt6::Svg
    Qt6::Core5Compat
    Qt6::Concurrent
    ${CMAKE_BINARY_DIR}/FluentUI/src/fluentui.a
)
