# 1. 引入Qt核心模块+数据库模块（必须）
QT += core sql

# 2. 启用C++11（兼容Qt语法）
CONFIG += c++11

# 3. 项目编译后的可执行文件名称（自定义，比如flight_booking）
TARGET = flight_booking

# 4. 项目类型：控制台程序（不需要UI，所以用app+控制台模式）
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle  # 屏蔽macOS的app bundle（Windows/Linux可忽略）

# 5. 源文件：只保留main.cpp（所有Model/DAO都是纯头文件，无需.cpp）
SOURCES += main.cpp

# 6. 头文件：列出所有你创建的Model/DAO/DBManager头文件（按实际文件名写，大小写一致）
HEADERS += Admin.h \
           DBManager.h \
           Flight.h \
           FlightDao.h \
           OrderChange.h \
           OrderInfo.h \
           OrderInfoDao.h \
           Passenger.h \
           User.h \
           UserDao.h

# 7. 翻译文件（如果不需要多语言，可删除下面2行）
TRANSLATIONS += untitled_zh_CN.ts
CONFIG += lrelease embed_translations
