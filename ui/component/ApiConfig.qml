  import QtQuick 2.15

// API配置文件，统一管理所有API端点
Item {
    id: apiConfig
    
    // API基础URL
    property string baseUrl: "http://172.17.24.186:8080"
    
    // 用户相关API
    property string loginUrl: "/api/user/login"
    property string registerUrl: "/api/user/register"
    property string getUserInfoUrl: "/api/user/info/get"
    property string updateUserInfoUrl: "/api/user/info/update"
    
    // 航班相关API
    property string searchFlightsUrl: "/api/flight/search"
    
    // 订单相关API
    property string queryOrdersUrl: "/api/order/query"
    property string bookFlightUrl: "/api/order/book"
    property string payOrderUrl: "/api/order/pay"
    property string refundOrderUrl: "/api/order/refund"
    property string deleteOrderUrl: "/api/order/delete"
    property string changeOrderUrl: "/api/order/change"
    
    // 从全局userContext获取Token
    property string token: userContext ? userContext.myToken : ""
    
    // 管理员相关API
    property string adminLoginUrl: "/api/admin/login"
    property string adminRegisterUrl: "/api/admin/register"
    property string adminAddFlightUrl: "/api/admin/flight/add"
    property string adminDeleteFlightUrl: "/api/admin/flight/delete"
    property string adminUpdateFlightUrl: "/api/admin/flight/update"
    property string adminSearchFlightsUrl: "/api/admin/flight/search"
    property string adminGetAllUsersUrl: "/api/admin/user/all"
    property string adminGetAllOrdersUrl: "/api/admin/order/all"
    
    // 构建完整URL
    function buildUrl(endpoint) {
        if (endpoint.startsWith("http")) {
            return endpoint;
        }
        return baseUrl + endpoint;
    }
}
