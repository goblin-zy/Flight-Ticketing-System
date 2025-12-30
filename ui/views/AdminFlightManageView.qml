import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import FluentUI 1.0
// 使用相对路径导入组件
import "../component"
// 导入NetworkManager组件
import App.Network 1.0

FluPage {
    id: adminFlightManagePage
    title: "航班管理"
    visible: true
    launchMode: FluPageType.Standard

    // ==================== 属性定义 ====================
    // 分页属性
    property int currentPage: 1
    property int totalPages: 1
    property int pageSize: 10
    property int totalRecords: 0

    // 筛选状态保存属性
    property int savedDepartureCityIndex: 0
    property int savedArrivalCityIndex: 0
    property int savedFlightDateIndex: 0
    property int savedAirlineIndex: 0
    property int savedCurrentPage: 1

    // 编辑状态
    property bool isEditing: false
    property var currentFlight: null

    // 表格列定义
    property var columns: [
        { dataIndex: "flightNumber", title: "航班号", width: 100 },
        { dataIndex: "departureCity", title: "出发城市", width: 100 },
        { dataIndex: "arrivalCity", title: "到达城市", width: 100 },
        { dataIndex: "date", title: "日期", width: 120 },
        { dataIndex: "departureTime", title: "起飞时间", width: 100 },
        { dataIndex: "arrivalTime", title: "到达时间", width: 100 },
        { dataIndex: "price", title: "价格", width: 80 },
        { dataIndex: "airlineCompany", title: "航空公司", width: 120 },
        { dataIndex: "status", title: "状态", width: 80 },
        { dataIndex: "leftTickets", title: "剩余票数", width: 80 }
    ]

    // ==================== 数据模型 ====================
    ListModel {
        id: flightModel
    }

    // ==================== 组件 ====================
    // 网络请求管理器
    NetworkManager {
        id: networkManager
        onRequestSuccess: function(response) {
            if (response.code === 200) {
            console.log("API请求成功: " + response.message);
            console.log("API响应数据:", JSON.stringify(response.data));

            if (response.data && response.data.list) {
                // 更新航班列表
                console.log("API返回的list长度: " + response.data.list.length);
                console.log("API返回的total: " + response.data.total);
                updateFlightList(response.data.list, response.data.total || response.data.list.length);

            } else if (response.data && response.data.flightId) {
                // 航班添加/修改/删除成功
                console.log("操作成功，刷新航班列表");
                resetForm();
                loadFlights();
            } else if (Array.isArray(response.data)) {
                // 兼容直接返回数组的情况
                updateFlightList(response.data, response.data.length);
            }
        } else {
                console.error("API请求失败: " + response.message);
            }
        }
        onRequestFailed: function(errorMessage) {
            console.error("网络请求失败: " + errorMessage);
        }
    }

    // API配置
    ApiConfig {
        id: apiConfig
    }

    // ==================== 功能函数 ====================
    // 加载航班数据
    function loadFlights() {
        console.log("开始加载航班数据");
        console.log("当前页码: " + currentPage);
        console.log("保存的当前页码: " + savedCurrentPage);

        // 明确转换为整数，确保pageNum是数字类型
        var pageNumToRequest = parseInt(savedCurrentPage);
        console.log("请求的页码pageNumToRequest=" + pageNumToRequest);

        // 构建请求参数
        var params = {
            departureCity: flightFilterBar.departureCityIndex >= 0 && flightFilterBar.departureCityIndex < flightFilterBar.departureCityModel.count && flightFilterBar.departureCityModel.get(flightFilterBar.departureCityIndex).name !== "全部" ? flightFilterBar.departureCityModel.get(flightFilterBar.departureCityIndex).name : "",
            arrivalCity: flightFilterBar.arrivalCityIndex >= 0 && flightFilterBar.arrivalCityIndex < flightFilterBar.arrivalCityModel.count && flightFilterBar.arrivalCityModel.get(flightFilterBar.arrivalCityIndex).name !== "全部" ? flightFilterBar.arrivalCityModel.get(flightFilterBar.arrivalCityIndex).name : "",
            date: flightFilterBar.flightDateIndex >= 0 && flightFilterBar.flightDateIndex < flightFilterBar.flightDateModel.count && flightFilterBar.flightDateModel.get(flightFilterBar.flightDateIndex).name !== "全部" ? flightFilterBar.flightDateModel.get(flightFilterBar.flightDateIndex).name : "",
            pageNum: pageNumToRequest,  // 使用转换后的页码
            pageSize: pageSize
        };

        console.log("请求参数:", JSON.stringify(params));

        // 调用API - 将token作为第四个参数传递，NetworkManager会自动添加到请求头
        networkManager.request(
            apiConfig.buildUrl(apiConfig.adminSearchFlightsUrl),
            NetworkManager.GET,
            params,
            userContext ? userContext.myToken : ""
        );
    }

    // 更新航班列表
    function updateFlightList(flights, total) {
        console.log("更新航班列表，共" + flights.length + "条记录");

        // 清空现有数据
        flightModel.clear();

        // 添加新数据
        for (var i = 0; i < flights.length; i++) {
            var flight = flights[i];
            //console.log("添加航班到模型: " + JSON.stringify(flight));
            console.log("航班ID字段: " + flight.flightId + ", 类型: " + typeof flight.flightId);
            flightModel.append(flight);
        }

        // 更新分页信息
        // 确保total是有效的数字
        var validTotal = parseInt(total) || 0;
        totalRecords = validTotal;
        totalPages = Math.ceil(validTotal / pageSize);
        console.log("更新分页，共" + totalPages + "页");
        console.log("更新分页，共" + totalRecords + "条记录");

        // 强制更新分页控件的属性，确保显示正确的分页信息
        flightPagination.totalRecords = validTotal;
        flightPagination.totalPages = totalPages;
    }

    // 新增航班
    function addFlight() {
        // 注意：这个函数现在不会被直接调用，而是通过对话框的positiveClicked信号调用
        // 但是由于flightForm是在对话框内部动态创建的，我们无法直接访问它
        // 我们需要修改FlightForm组件，让它能够在提交时发送信号并传递表单数据
    }

    // 编辑航班
    function editFlight(flight) {
        console.log("编辑航班: 航班号=" + flight.flightNumber + ", 航班ID=" + flight.flightId);
        isEditing = true;
        currentFlight = flight;

        // 移除直接填充表单数据的代码，因为flightForm是在对话框内部动态创建的
        // 表单数据将在对话框打开时填充
    }

    // 更新航班
    function updateFlight() {
        // 注意：这个函数现在不会被直接调用，而是通过对话框的positiveClicked信号调用
        // 但是由于flightForm是在对话框内部动态创建的，我们无法直接访问它
        // 我们需要修改FlightForm组件，让它能够在提交时发送信号并传递表单数据
    }

    // 删除航班
    function deleteFlight(flightId) {
        console.log("删除航班，ID: " + flightId);

        // 构建请求参数
        var params = {
            flightId: flightId
        };

        // 调用API - 将token作为第四个参数传递，NetworkManager会自动添加到请求头
        networkManager.request(
            apiConfig.buildUrl(apiConfig.adminDeleteFlightUrl),
            NetworkManager.POST,
            params,
            userContext ? userContext.myToken : ""
        );
    }

    // 重置表单
    function resetForm() {
        isEditing = false;
        currentFlight = null;
        // 移除对flightForm.reset()的调用，因为flightForm是在对话框内部动态创建的
        // flightForm.reset();
    }
    // ==================== UI布局 ====================
    ColumnLayout {
        anchors.fill: parent
        spacing: 5
        anchors.margins: 20



        // 筛选栏
        FlightFilterBar {
            id: flightFilterBar
            departureCityIndex: savedDepartureCityIndex
            arrivalCityIndex: savedArrivalCityIndex
            flightDateIndex: savedFlightDateIndex
            airlineIndex: savedAirlineIndex

            onSearchTriggered: {
                // 保存筛选状态
                savedDepartureCityIndex = flightFilterBar.departureCityIndex;
                savedArrivalCityIndex = flightFilterBar.arrivalCityIndex;
                savedFlightDateIndex = flightFilterBar.flightDateIndex;
                savedAirlineIndex = flightFilterBar.airlineIndex;

                // 重置为第一页
                currentPage = 1;
                savedCurrentPage = 1;
                flightPagination.currentPage = 1;

                // 执行筛选
                loadFlights();
            }
        }

        // 添加航班按钮
        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight
            spacing: 10

            FluFilledButton {
                text: "添加航班"
                onClicked: {
                    resetForm();
                    flightDialog.open();
                }
            }
        }

        // 航班列表和分页
        FluControlBackground {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                spacing: 10
                anchors.fill: parent
                anchors.margins: 15

                // 表格标题
                FluText {
                    text: "航班列表"
                    font: FluTextStyle.Title
                    Layout.alignment: Qt.AlignHCenter
                }

                // 航班列表 - 使用ScrollView和自定义卡片（与用户端FlightItemCard一致）
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.maximumHeight: parent.height // 为分页栏和标题预留更多空间
                    clip: true // 确保内容正确显示，不溢出

                    Column {
                        width: parent.width
                        spacing: 10
                        //padding: 10 // 为内容添加内边距，确保最后一个元素完整显示

                        Repeater {
                            model: flightModel

                            FluControlBackground {
                                width: parent.width
                                height: 120

                                GridLayout {
                                    columns: 5
                                    columnSpacing: 10
                                    rowSpacing: 5
                                    anchors.fill: parent
                                    anchors.margins: 10

                                    // 1. 航班基本信息列
                                    Column {
                                        Layout.preferredWidth: 120
                                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                        spacing: 5

                                        FluText {
                                            text: model.flightNumber
                                            font: FluTextStyle.Title
                                            width: parent.width
                                        }
                                        FluText {
                                            text: "航班ID: " + model.flightId
                                            font: FluTextStyle.Body
                                            color: FluTheme.dark ? Qt.rgba(128/255, 128/255, 128/255, 1) : Qt.rgba(160/255, 160/255, 160/255, 1)
                                            width: parent.width
                                        }
                                        FluText {
                                            text: model.airlineCompany
                                            font: FluTextStyle.Body
                                            width: parent.width
                                        }
                                    }

                                    // 2. 出发信息列
                                    Column {
                                        Layout.preferredWidth: 170
                                        Layout.alignment: Qt.AlignCenter
                                        spacing: 5

                                        FluText {
                                            text: model.departureCity
                                            font: FluTextStyle.Title
                                            horizontalAlignment: Text.AlignHCenter
                                            width: parent.width
                                        }
                                        FluText {
                                            text: model.departureAirport
                                            font: FluTextStyle.Body
                                            horizontalAlignment: Text.AlignHCenter
                                            width: parent.width
                                        }
                                        FluText {
                                            text: model.departureTime
                                            font: FluTextStyle.Body
                                            horizontalAlignment: Text.AlignHCenter
                                            width: parent.width
                                        }
                                    }

                                    // 3. 航线图标列
                                    Item {
                                        Layout.preferredWidth: 50
                                        Layout.alignment: Qt.AlignVCenter

                                        FluText {
                                            anchors.centerIn: parent
                                            text: "→"
                                            font.pixelSize: 20
                                        }
                                    }

                                    // 4. 到达信息列
                                    Column {
                                        Layout.preferredWidth: 170
                                        Layout.alignment: Qt.AlignCenter
                                        spacing: 5

                                        FluText {
                                            text: model.arrivalCity
                                            font: FluTextStyle.Title
                                            horizontalAlignment: Text.AlignHCenter
                                            width: parent.width
                                        }
                                        FluText {
                                            text: model.arrivalAirport
                                            font: FluTextStyle.Body
                                            horizontalAlignment: Text.AlignHCenter
                                            width: parent.width
                                        }
                                        FluText {
                                            text: model.arrivalTime
                                            font: FluTextStyle.Body
                                            horizontalAlignment: Text.AlignHCenter
                                            width: parent.width
                                        }
                                    }

                                    // 5. 价格和操作列 - 简化布局，确保按钮完整显示
                                    Column {
                                        Layout.fillWidth: true
                                        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                                        spacing: 5

                                        FluText {
                                            text: "¥" + (model.price / 100)
                                            font: FluTextStyle.Title
                                            horizontalAlignment: Text.AlignRight
                                        }

                                        FluText {
                                            text: model.status
                                            font: FluTextStyle.Body
                                            horizontalAlignment: Text.AlignRight
                                        }

                                        // 剩余票数显示
                                        FluText {
                                            text: "剩余票数: " + (model.leftTickets || 0)
                                            font.pixelSize: 10
                                            horizontalAlignment: Text.AlignRight
                                        }

                                        // 操作按钮 - 简化水平布局
                                        Row {
                                            anchors.right: parent.right
                                            spacing: 5

                                            FluFilledButton {
                                                text: "修改"
                                                onClicked: {
                                                    editFlight(model);
                                                    flightDialog.open();
                                                }
                                                height: 28
                                                font.pixelSize: 10
                                            }

                                            FluButton {
                                                text: "删除"
                                                onClicked: {
                                                    deleteFlight(model.flightId);
                                                }
                                                height: 28
                                                font.pixelSize: 10
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // 分页
                FlightPagination {
                    id: flightPagination
                    Layout.fillWidth: true
                    currentPage: savedCurrentPage
                    totalPages: totalPages
                    totalRecords: totalRecords

                    onPageChanged: function(page) {
                        console.log("页码变化: " + page);
                        currentPage = page;
                        savedCurrentPage = page;
                        loadFlights();
                    }
                }
            }
        }

        // 航班操作弹窗 - 同时用于添加和编辑
        FluContentDialog {
            id: flightDialog
            //title: isEditing ? "编辑航班" : "新增航班"
            // 移除重复的对话框按钮，使用表单内部的按钮
            buttonFlags: FluContentDialogType.NoButton

            // 表单容器，确保表单能正确显示
            FluFrame {
                anchors.fill: parent
                padding: 10

                // 添加Flickable，实现竖向滚轮，使用FluentUI的滚动条样式
                Flickable {
                    width: parent.width - 20
                    height: parent.height - 20
                    clip: true // 确保内容超出部分被裁剪
                    boundsBehavior: Flickable.StopAtBounds
                    contentHeight: flightForm.height // 确保内容高度正确设置

                    // 添加FluentUI的滚动条样式
                    ScrollBar.vertical: FluScrollBar {}

                    // 确保Flickable是可交互的，支持鼠标滚轮
                    interactive: true

                    FlightForm {
                        id: flightForm
                        width: parent.width
                        // 不设置固定高度，让表单内容自然展开
                        // 传递isEditing属性给FlightForm
                        isEditing: adminFlightManagePage.isEditing

                        // 确保表单可见
                        visible: true

                        // 传递currentFlight给FlightForm的属性
                        property var flightData: adminFlightManagePage.currentFlight

                        // 使用onIsEditingChanged和onFlightDataChanged信号来触发fillForm
                        // 这样可以确保在属性变化时正确更新表单
                        onIsEditingChanged: {

                            if (isEditing && flightData) {
                                fillForm(flightData);
                            } else {
                                reset();
                            }
                        }
                        
                        onFlightDataChanged: {

                            if (isEditing && flightData) {
                                fillForm(flightData);
                            }
                        }
                        
                        // 表单填充和重置
                        Component.onCompleted: {

                            if (isEditing && flightData) {
                                fillForm(flightData);
                            } else {
                                reset();
                            }
                        }

                        // 连接submit信号到对话框的关闭操作
                        onSubmit: {
                            var params;
                            
                            if (isEditing) {
                                // 修改航班API参数：需要flightId，不需要airlineCompany
                                params = {
                                    flightId: flightForm.flightId,
                                    flightNumber: flightForm.flightNumber || "",
                                    departureCity: flightForm.departureCity || "",
                                    arrivalCity: flightForm.arrivalCity || "",
                                    date: flightForm.date || "",
                                    departureTime: flightForm.departureTime || "",
                                    arrivalTime: flightForm.arrivalTime || "",
                                    departureAirport: flightForm.departureAirport || "",
                                    arrivalAirport: flightForm.arrivalAirport || "",
                                    checkinStartTime: flightForm.checkinStartTime || "",
                                    checkinEndTime: flightForm.checkinEndTime || "",
                                    price: flightForm.price ? parseInt(flightForm.price * 100) : 0, // 转换为分
                                    status: flightForm.status || "正常",
                                    leftTickets: flightForm.leftTickets ? parseInt(flightForm.leftTickets) : 0
                                };
                            } else {
                                // 新增航班API参数：不需要flightId，需要airlineCompany
                                params = {
                                    flightNumber: flightForm.flightNumber || "",
                                    departureCity: flightForm.departureCity || "",
                                    arrivalCity: flightForm.arrivalCity || "",
                                    date: flightForm.date || "",
                                    departureTime: flightForm.departureTime || "",
                                    arrivalTime: flightForm.arrivalTime || "",
                                    departureAirport: flightForm.departureAirport || "",
                                    arrivalAirport: flightForm.arrivalAirport || "",
                                    checkinStartTime: flightForm.checkinStartTime || "",
                                    checkinEndTime: flightForm.checkinEndTime || "",
                                    price: flightForm.price ? parseInt(flightForm.price * 100) : 0, // 转换为分
                                    airlineCompany: flightForm.airlineCompany || "",
                                    status: flightForm.status || "正常",
                                    leftTickets: flightForm.leftTickets ? parseInt(flightForm.leftTickets) : 0
                                };
                            }



                            // 直接访问父级上下文的apiConfig和networkManager，不需要通过adminFlightManagePage
                            var url = isEditing ? apiConfig.buildUrl(apiConfig.adminUpdateFlightUrl) : apiConfig.buildUrl(apiConfig.adminAddFlightUrl);
                            networkManager.request(
                                url,
                                NetworkManager.POST,
                                params,
                                userContext ? userContext.myToken : ""
                            );

                            // 提交后关闭对话框
                            flightDialog.close();

                        }

                        // 连接cancel信号到对话框的关闭操作
                        onCancel: {
                            flightDialog.close();
                        }
                    }
                }
            }

            // 设置对话框的默认大小
            width: 700
            height: 600 // 适当调整高度，配合滚动视图
        }
    }

    // 页面显示时加载数据
    onVisibleChanged: {
        if (visible) {
            loadFlights();
        }
    }
}
