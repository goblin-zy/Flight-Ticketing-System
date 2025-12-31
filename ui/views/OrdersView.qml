import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import "../component"
import App.Network 1.0

FluPage {
    id: ordersPage
    title: "我的订单"
    visible: true
    launchMode: FluPageType.Standard

    // 【关键修改】：独立背景层，与列表分离，确保不会被遮挡或布局塌缩
    Rectangle {
        anchors.fill: parent
        color: FluTheme.windowBackgroundColor || "#F2F2F2" // 浅灰色背景
        z: -1
    }

    ListModel { id: ordersModel }

    onVisibleChanged: { if (visible) loadOrdersFromApi(); }

    // ==========================================
    // 1. 对话框逻辑 (完全复制原代码，不做任何修改)
    // ==========================================
    FluContentDialog {
        id: confirmDialog
        property string orderId: ""; property string orderStatus: "待支付"
        title: (orderStatus === "已完成" || orderStatus === "已支付") ? "退签确认" : "删除订单确认"
        message: (orderStatus === "已完成" || orderStatus === "已支付") ? "您确定要退签吗？" : "您确定要删除该订单吗？"
        negativeText: "取消"; positiveText: "确认"
        onPositiveClicked: { refundOrder(orderId, orderStatus); confirmDialog.close(); }
    }

    FluContentDialog {
        id: changeDialog
        property string orderId: ""; property string newFlightId: ""
        title: "改签确认"; message: "请输入新的航班ID"
        contentDelegate: Component {
            Column {
                width: 350; height: 100; spacing: 15; padding: 15
                FluTextBox { placeholderText: "输入新航班ID"; width: parent.width - 30; onTextChanged: changeDialog.newFlightId = text }
            }
        }
        onPositiveClicked: { if (newFlightId.trim()) { changeOrder(orderId, newFlightId.trim()); changeDialog.close(); } }
    }

    // 余额不足提示弹窗
    FluContentDialog {
        id: insufficientBalanceDialog
        title: "余额不足"
        message: "您的账户余额不足，请先充值"
        positiveText: "确定"
    }

    FluContentDialog {
        id: priceDiffDialog
        property string orderId: ""; property string newFlightNumber: ""; property int priceDifference: 0
        title: "差价支付"; message: "改签需支付差价"+("¥"+priceDifference/100)
        onPositiveClicked: {
            // 检查余额是否足够
            var requiredAmount = priceDifference;
            if (userContext && userContext.myMoney < requiredAmount) {
                insufficientBalanceDialog.open();
                return;
            }
            priceDiffDialog.close();
        }
    }

    FluContentDialog {
        id: paymentDialog
        property string orderId: ""; property string flightNumber: ""; property double price: 0
        title: "支付确认"
        message: "确认支付订单 " + orderId + " (¥" + price + ")?"
        onPositiveClicked: {
            // 检查余额是否足够
            var requiredAmount = Math.round(price * 100); // 转换为分
            if (userContext && userContext.myMoney < requiredAmount) {
                insufficientBalanceDialog.open();
                return;
            }
            var payParams = { telephone: userContext ? userContext.myTelephone : "", orderId: orderId };
            networkManager.request("/api/order/pay", NetworkManager.POST, payParams, userContext.myToken);
        }
    }

    // ==========================================
    // 2. 核心布局 (改用 ListView)
    // ==========================================
    ColumnLayout {
        anchors.fill: parent
        spacing: 0 // 无间距，完全由内部控制

        // 顶部空隙或标题栏（可选）
        Item { Layout.fillWidth: true; Layout.preferredHeight: 10 }

        // 使用 ListView 替换 ScrollView+Column，性能更好，布局更稳
        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: ordersModel
            spacing: 15 // 卡片之间的间距

            // 设置边距，让卡片不要贴边
            leftMargin: 20
            rightMargin: 20
            topMargin: 10
            bottomMargin: 20

            // 空状态提示
            FluText {
                text: "暂无订单数据"
                visible: ordersModel.count === 0
                anchors.centerIn: parent
                font: FluTextStyle.Title
                color: "#999999"
            }

            delegate: OrderInfoCard {
                // 宽度填满 ListView（减去 margin）
                width: ListView.view.width - 40
                height: 180 // 固定高度，或者改为自适应

                // 数据绑定
                orderId: model.orderId; flightNumber: model.flightNumber
                departureCity: model.departureCity; arrivalCity: model.arrivalCity
                departureTime: model.departureTime; arrivalTime: model.arrivalTime
                status: model.status; price: model.price
                createTime: model.createTime; passengers: model.passengers

                // 信号连接
                onPayButtonClicked: {
                    paymentDialog.orderId = model.orderId; paymentDialog.price = model.price
                    paymentDialog.open();
                }
                onRefundButtonClicked: {
                    confirmDialog.orderId = model.orderId; confirmDialog.orderStatus = model.status; confirmDialog.open();
                }
                onChangeButtonClicked: {
                    changeDialog.orderId = model.orderId; changeDialog.open();
                }
            }
        }
    }

    // ==========================================
    // 3. 网络通信 (完全复制)
    // ==========================================
    NetworkManager {
        id: networkManager
        onRequestSuccess: function(response) {
            if (response.code === 200) {
                if (response.data && response.data.status === "改签完成" && response.data.priceDifference > 0) {
                    priceDiffDialog.priceDifference = response.data.priceDifference;
                    priceDiffDialog.open();
                } else {
                    ordersModel.clear();
                    if (Array.isArray(response.data)) updateOrdersFromApi(response.data);
                    else if (response.data && response.data.list) updateOrdersFromApi(response.data.list);
                    else Qt.callLater(loadOrdersFromApi);
                }
            }
        }
    }

    function loadOrdersFromApi() { networkManager.request("/api/order/query", NetworkManager.GET, {status: ""}, userContext.myToken); }

    function updateOrdersFromApi(orders) {
        ordersModel.clear();
        for (var i = 0; i < orders.length; i++) {
            var order = orders[i];
            ordersModel.append({
                orderId: order.orderId || "", flightNumber: order.flightNumber || "",
                departureCity: order.departureCity || "", arrivalCity: order.arrivalCity || "",
                departureTime: order.departureTime || "", arrivalTime: order.arrivalTime || "",
                price: (order.totalPrice || 0) / 100, status: order.status || "待支付",
                passengers: JSON.stringify(order.passengers || []), createTime: order.createTime || ""
            });
        }
    }

    function refundOrder(id, status) {
        var url = (status === "已完成" || status === "已支付") ? "/api/order/refund" : "/api/order/delete";
        networkManager.request(url, NetworkManager.POST, {orderId: id}, userContext.myToken);
    }

    function changeOrder(id, flightId) {
        networkManager.request("/api/order/change", NetworkManager.POST, {orderId: id, newFlightId: parseInt(flightId)}, userContext.myToken);
    }
}
