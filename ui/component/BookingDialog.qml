import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import App.Network 1.0

// 预定弹窗组件
FluContentDialog {
    id: bookDialog

    // 属性定义
    property int flightId: -1
    property string flightNumber: ""
    property string departureCity: ""
    property string arrivalCity: ""
    property string departureTime: ""
    property string arrivalTime: ""
    property double price: 0
    property bool bookSuccess: false

    // 信号定义
    signal bookCompleted

    // FluContentDialog支持的属性
    title: "预定确认"
    message: "确认预定此航班吗？"
    negativeText: "取消"
    positiveText: "确认预定"
    buttonFlags: FluContentDialogType.NegativeButton | FluContentDialogType.PositiveButton

    // 乘客信息模型
    ListModel {
        id: passengersModel
        // 默认添加一个乘客
        Component.onCompleted: {
            append({name: "", idCard: ""});
        }
    }

    // 内容委托 - 包含航班信息和乘客输入
    contentDelegate: Component {
        Column {
            width: 500
            spacing: 15
            padding: 15

            // 航班信息摘要
            Column {
                spacing: 6

                FluText {
                    text: "航班号: " + flightNumber
                    font.bold: true
                }
                FluText {
                    text: "航线: " + departureCity + " → " + arrivalCity
                    font.bold: true
                }
                FluText {
                    text: "出发时间: " + departureTime
                    font.bold: true
                }
                FluText {
                    text: "到达时间: " + arrivalTime
                    font.bold: true
                }
                FluText {
                    text: "价格: ¥" + price.toFixed(2)
                    font: FluTextStyle.Title
                    color: Qt.rgba(250/255, 85/255, 85/255, 1)
                }
            }

            FluDivider {}

            // 乘客信息标题
            FluText {
                text: "乘客信息"
                // 使用自定义字体样式，结合Subtitle和bold
                font.pixelSize: FluTextStyle.Subtitle.pixelSize
                font.family: FluTextStyle.Subtitle.family
                font.bold: true
            }

            // 乘客列表，使用Flickable实现滚动，并动态计算contentHeight
            Flickable {
                width: 470
                height: 200
                contentWidth: 470
                // 动态计算内容高度，避免不必要的滚动条
                contentHeight: passengersColumn.height
                clip: true
                flickableDirection: Flickable.VerticalFlick

                Column {
                    id: passengersColumn
                    width: 470
                    spacing: 10

                    Repeater {
                        model: passengersModel
                        delegate: Column {
                            spacing: 3
                            width: 470

                            FluText {
                                text: "乘客 " + (index + 1)
                                font: FluTextStyle.Body
                            }
                            
                            // 姓名和身份证号并排显示
                            Row {
                                spacing: 8
                                width: 470

                                // 姓名输入框 - 无标签，直接显示输入框
                                FluTextBox {
                                    placeholderText: "姓名"
                                    text: name
                                    width: 160
                                    onTextChanged: {
                                        passengersModel.setProperty(index, "name", text);
                                    }
                                }

                                // 身份证号输入框 - 无标签，直接显示输入框
                                FluTextBox {
                                    placeholderText: "身份证号"
                                    text: idCard
                                    width: 200
                                    onTextChanged: {
                                        passengersModel.setProperty(index, "idCard", text);
                                    }
                                }
                            }
                        }
                    }

                    // 添加乘客按钮
                    FluButton {
                        text: "添加乘客"
                        width: 100
                        anchors.horizontalCenter: parent.horizontalCenter
                        onClicked: {
                            passengersModel.append({name: "", idCard: ""});
                        }
                    }
                }
            }
        }
    }

    // API配置
    ApiConfig {
        id: apiConfig
    }

    // 网络请求管理器
    NetworkManager {
        id: networkManager
        onRequestSuccess: function(response) {
            if (response.code === 200) {
                console.log("预定成功");
                bookSuccess = true;
                bookCompleted();
                bookDialog.close();
            } else {
                console.error("预定失败: " + response.message);
                bookSuccess = false;
                // 显示错误信息
                showErrorDialog(response.message);
            }
        }
        onRequestFailed: function(errorMessage) {
            console.error("网络请求失败: " + errorMessage);
            bookSuccess = false;
            // 显示错误信息
            showErrorDialog(errorMessage);
        }
    }

    // 显示错误对话框
    function showErrorDialog(message) {
        console.error("预定失败: " + message);
        // 使用console.error输出错误信息，避免创建子对话框的语法错误
        // 可以考虑使用其他方式显示错误，比如在界面上显示错误文本
    }

    // 确认预定逻辑
    onPositiveClicked: {
        console.log("确认预定航班: " + flightNumber + ", 价格: ¥" + price.toFixed(2));
        console.log("Booking 使用的 Token =", apiConfig.token)

        // 添加调试日志
        console.log("【下单调试】flightId =", flightId, "类型 =", typeof flightId)

        // 收集乘客信息
        var passengers = [];
        for (var i = 0; i < passengersModel.count; i++) {
            var passenger = passengersModel.get(i);
            // 只添加有效的乘客信息（姓名和身份证号都不为空）
            if (passenger.name && passenger.idCard) {
                passengers.push({
                    name: passenger.name,
                    idCard: passenger.idCard
                });
            }
        }

        // 验证是否有有效的乘客信息
        if (passengers.length === 0) {
            console.error("请至少填写一位乘客的信息");
            return;
        }

        // 准备API请求参数，根据新API文档，只需要flightId和passengers
        var bookParams = {
            flightId: flightId,
            passengers: passengers
        };

        console.log("【下单调试】乘客信息 =", JSON.stringify(passengers))
        console.log("【下单调试】API请求参数 =", JSON.stringify(bookParams))

        // 调用API的机票预定接口，token通过Authorization头传递
        networkManager.request(
            apiConfig.buildUrl(apiConfig.bookFlightUrl),
            NetworkManager.POST,
            bookParams,
            apiConfig.token
        );
    }

    // 关闭弹窗时重置状态
    onClosed: {
        bookSuccess = false;
        // 重置乘客模型，只保留一个默认乘客
        passengersModel.clear();
        passengersModel.append({name: "", idCard: ""});
    }
}
