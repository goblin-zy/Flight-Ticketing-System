import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import FluentUI 1.0
// 使用相对路径导入组件
import "../component"
// 导入NetworkManager组件
import App.Network 1.0

FluPage {
    id: flightInfoPage
    title: "航班信息"
    visible: true
    launchMode: FluPageType.Standard

    // ==================== 属性定义 ====================
    // 分页属性
    property int currentPage: 1
    property int totalPages: 1
    property int pageSize: 10
    property int totalRecords: 0 // 新增：总记录数，用于分页计算

    // 筛选状态保存属性
    property int savedDepartureCityIndex: 0
    property int savedArrivalCityIndex: 0
    property int savedFlightDateIndex: 0
    property int savedAirlineIndex: 0
    property int savedCurrentPage: 1

    // ==================== 数据模型 ====================
    // 当前页显示的航班模型
    ListModel {
        id: currentPageModel
    }

    // 航班模型，存储从API获取的航班数据
    ListModel {
        id: flightModel
    }

    // 筛选后的航班数据模型
    ListModel {
        id: filteredFlightModel
    }

    // ==================== 组件生命周期 ====================
    // 组件加载时执行
    Component.onCompleted: {
        console.log("FlightInfoView 组件已加载");
        // 从API加载航班数据
        loadFlightsFromApi();
    }
    // ==================== 本地功能函数 ====================
    // 更新当前页数据
    function updateCurrentPageData() {
        currentPageModel.clear();

        console.log("更新当前页数据: 使用API返回的当前页数据，共" + filteredFlightModel.count + "条");

        // 直接使用filteredFlightModel中的数据，因为API已经返回了当前页的数据
        for (var i = 0; i < filteredFlightModel.count; i++) {
            var flight = filteredFlightModel.get(i);
            currentPageModel.append(flight);
        }
    }

    // 更新分页信息
    function updatePagination() {
        totalPages = Math.ceil(totalRecords / pageSize);
        if (currentPage > totalPages) {
            currentPage = Math.max(1, totalPages);
        }
        console.log("分页信息: 当前页=" + currentPage + ", 每页=" + pageSize + ", 总页数=" + totalPages + ", 总记录数=" + totalRecords);
    }

    // 检查航班是否已预定
    function isFlightBooked(flightNumber) {
        // 只使用API获取订单数据，不使用全局变量
        // 简单返回false，实际应用中可以从API获取已预订的航班列表
        return false;
    }

    // 筛选航班
    function filterFlights() {
        console.log("开始筛选航班");
        // 重置为第一页
        currentPage = 1;
        savedCurrentPage = 1;
        // 直接从API重新获取数据，使用当前筛选条件
        flightPagination.currentPage = 1;
        loadFlightsFromApi();
    }

    // ==================== UI布局 ====================
    Column {
        anchors.fill: parent
        spacing: 10
        leftPadding: 10
        rightPadding: 10
        topPadding: 10
        bottomPadding: 10

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

                // 执行筛选
                filterFlights();
            }
        }

        // 航班列表
        ScrollView {
            height: parent.height - 200
            width: parent.width

            // 确保内容宽度适应
            clip: true

            Column {
                width: parent.width
                spacing: 10

                Repeater {
                    model: currentPageModel

                    FlightItemCard {
                        // 航班信息属性 - 添加null检查
                        flightId: model ? parseInt(model.flight_id) : -1
                        flightNumber: model ? model.flight_number : ""
                        airlineCompany: model ? model.airline_company : ""
                        departureCity: model ? model.departure_city : ""
                        departureAirport: model ? model.departure_airport : ""
                        departureTime: model ? model.departure_time : ""
                        arrivalCity: model ? model.arrival_city : ""
                        arrivalAirport: model ? model.arrival_airport : ""
                        arrivalTime: model ? model.arrival_time : ""
                        price: model ? model.price : 0
                        status: model ? model.status : ""
                        isBooked: model ? isFlightBooked(model.flight_number) : false

                        onFlightSelected: {
                            if (model) {
                                console.log("选择航班: " + model.flight_number);
                                console.log("航班信息: ");
                                console.log("航班号: " + model.flight_number);
                                console.log("出发城市: " + model.departure_city);
                                console.log("到达城市: " + model.arrival_city);
                                console.log("出发时间: " + model.departure_time);
                                console.log("到达时间: " + model.arrival_time);
                                console.log("价格: " + model.price);
                                console.log("航班ID: " + model.flight_id + "，类型: " + typeof model.flight_id);

                                // 设置支付弹窗的航班信息
                                paymentDialog.flightId = parseInt(model.flight_id);
                                paymentDialog.flightNumber = model.flight_number;
                                paymentDialog.departureCity = model.departure_city;
                                paymentDialog.arrivalCity = model.arrival_city;
                                paymentDialog.departureTime = model.departure_time;
                                paymentDialog.arrivalTime = model.arrival_time;
                                paymentDialog.price = model.price;

                                // 显示预定弹窗
                                console.log("显示预定弹窗");
                                paymentDialog.open();
                            }
                        }
                    }
                }
            }
        }

        // 分页控件
        FlightPagination {
            id: flightPagination
            currentPage: savedCurrentPage
            totalPages: totalPages
            totalRecords: totalRecords // 使用totalRecords而不是filteredFlightModel.count

            onPageChanged: function(page) {
                console.log("页码变化信号触发: 新页码=" + page);
                console.log("变化前currentPage=" + currentPage);
                // 更新当前页码
                currentPage = page;
                console.log("变化后currentPage=" + currentPage);
                // 保存当前页码，避免页面重新显示时被重置
                savedCurrentPage = page;
                console.log("保存的当前页码savedCurrentPage=" + savedCurrentPage);
                // 当页码改变时，从API重新获取数据
                loadFlightsFromApi();
            }
        }

        // 预定弹窗
        BookingDialog {
            id: paymentDialog

            // 监听预定完成信号
            onBookCompleted: {
                console.log("预定完成，更新航班列表");
                // 更新当前页数据，显示最新的已预定状态
                updateCurrentPageData();
            }
        }
    }

    // ==================== API配置与网络 ====================
    // API配置
    ApiConfig {
        id: apiConfig
    }

    // 网络请求管理器
    NetworkManager {
        id: networkManager
        onRequestSuccess: function(response) {
            if (response.code === 200) {
                console.log("=== API请求成功 ===");
                console.log("API响应数据:", JSON.stringify(response.data));
                if (response.data && response.data.list) {
                    // 传递分页相关字段，始终使用当前页码，忽略API返回的pageNum
                    updateFlightsFromApi(
                        response.data.list,
                        response.data.total || 0,
                        currentPage,
                        response.data.pageSize || pageSize
                    );
                }
            } else {
                console.error("API请求失败: " + response.message);
            }
        }
        onRequestFailed: function(errorMessage) {
            console.error("网络请求失败: " + errorMessage);
        }
    }

    // ==================== API相关函数 ====================
    // 从API加载航班数据
    function loadFlightsFromApi() {
        console.log("开始从API加载航班数据");
        console.log("当前currentPage=" + currentPage + ", 类型: " + typeof currentPage);

        // 明确转换为整数，确保pageNum是数字类型
        var pageNumToRequest = parseInt(savedCurrentPage);
        console.log("请求的页码pageNumToRequest=" + pageNumToRequest);

        // 构建API请求参数
        var params = {
            departureCity: flightFilterBar.departureCityIndex >= 0 && flightFilterBar.departureCityIndex < flightFilterBar.departureCityModel.count && flightFilterBar.departureCityModel.get(flightFilterBar.departureCityIndex).name !== "全部" ? flightFilterBar.departureCityModel.get(flightFilterBar.departureCityIndex).name : "",
            arrivalCity: flightFilterBar.arrivalCityIndex >= 0 && flightFilterBar.arrivalCityIndex < flightFilterBar.arrivalCityModel.count && flightFilterBar.arrivalCityModel.get(flightFilterBar.arrivalCityIndex).name !== "全部" ? flightFilterBar.arrivalCityModel.get(flightFilterBar.arrivalCityIndex).name : "",
            date: flightFilterBar.flightDateIndex >= 0 && flightFilterBar.flightDateIndex < flightFilterBar.flightDateModel.count && flightFilterBar.flightDateModel.get(flightFilterBar.flightDateIndex).name !== "全部" ? flightFilterBar.flightDateModel.get(flightFilterBar.flightDateIndex).name : "",
            airlineCompany: flightFilterBar.airlineIndex >= 0 && flightFilterBar.airlineIndex < flightFilterBar.airlineModel.count && flightFilterBar.airlineModel.get(flightFilterBar.airlineIndex).name !== "全部" ? flightFilterBar.airlineModel.get(flightFilterBar.airlineIndex).name : "",
            minPrice: "",
            maxPrice: "",
            pageNum: pageNumToRequest,  // 使用转换后的页码
            pageSize: pageSize
        };

        // 打印完整的请求参数，以便调试
        console.log("API请求参数:", JSON.stringify(params));

        // 调用API获取航班数据
        networkManager.request(
            apiConfig.buildUrl(apiConfig.searchFlightsUrl),
            NetworkManager.GET,
            params
        );
    }

    // 从API响应更新航班数据
    function updateFlightsFromApi(flights, total, pageNum, pageSize) {
        console.log("=== updateFlightsFromApi() 开始 ===");
        console.log("API返回的航班数据:", JSON.stringify(flights));
        console.log("分页信息: total=" + total + ", pageNum=" + pageNum + ", pageSize=" + pageSize);

        // 清空现有数据
        flightModel.clear();
        filteredFlightModel.clear();

        // 添加API返回的航班数据
        for (var i = 0; i < flights.length; i++) {
            var flight = flights[i];
            // 修复：不依赖date字段，只使用departureTime和arrivalTime
            // 或者添加默认日期，根据实际需求调整
            var fullDepartureTime = flight.departureTime;
            var fullArrivalTime = flight.arrivalTime;

            // 添加到模型，价格从分转换为元
            var flightData = {
                "flight_id": parseInt(flight.flightId) || -1,
                "flight_number": flight.flightNumber || "",
                "departure_city": flight.departureCity || "",
                "arrival_city": flight.arrivalCity || "",
                "departure_time": fullDepartureTime,
                "arrival_time": fullArrivalTime,
                "price": Math.round(flight.price || 0) / 100,
                "departure_airport": flight.departureAirport || "",
                "arrival_airport": flight.arrivalAirport || "",
                "airline_company": flight.airlineCompany || "",
                "checkin_start_time": flight.checkinStartTime || "",
                "checkin_end_time": flight.checkinEndTime || "",
                "status": flight.status || ""
            };

            flightModel.append(flightData);
            filteredFlightModel.append(flightData);
        }

        console.log("Loaded " + flightModel.count + " flights from API");

        // 使用API返回的分页信息更新
        updatePaginationWithApiData(total, pageNum, pageSize);

        // 更新当前页数据
        updateCurrentPageData();
    }

    // 使用API返回的分页信息更新
    function updatePaginationWithApiData(total, pageNum, pageSize) {
        console.log("使用API返回的分页信息更新: total=" + total + ", pageNum=" + pageNum + ", pageSize=" + pageSize);
        console.log("进入函数前currentPage=" + currentPage);

        // 确保total是有效的数字
        var validTotal = parseInt(total) || 0;
        var validPageSize = parseInt(pageSize) || 10;

        console.log("解析后的分页信息: total=" + validTotal + ", pageSize=" + validPageSize);

        // 更新总记录数
        totalRecords = validTotal;
        console.log("更新后的totalRecords: " + totalRecords + "，类型: " + typeof totalRecords);

        // 更新每页条数
        pageSize = validPageSize;

        // 保存当前页码，避免被updatePagination重置
        //var savedPage = currentPage;
        //console.log("保存的当前页码savedPage=" + savedPage);

        // 更新分页信息
        //updatePagination();

        // 强制恢复保存的页码，确保用户选择的页码不会丢失
       // currentPage = savedPage;
        //console.log("强制恢复保存的页码，currentPage=" + currentPage);
        // ★ 关键：以 API 返回的页码为准
        currentPage = parseInt(pageNum) || currentPage;

        // 再更新分页
        updatePagination();

        // 强制更新分页控件的属性，确保显示正确的页码
        flightPagination.totalRecords = validTotal;
        flightPagination.totalPages = totalPages;
        //flightPagination.currentPage = currentPage;
        console.log("强制更新分页控件: totalRecords=" + flightPagination.totalRecords + ", totalPages=" + flightPagination.totalPages + ", currentPage=" + flightPagination.currentPage);
    }
}
