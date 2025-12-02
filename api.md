1. 用户主要功能模块
	1.1 用户登录
	1.2 用户注册
	1.3 用户查询航班
	1.4 用户个人中心（个人订单）
	1.5 订票
	1.6 改签

2. 管理员主要功能模块
	2.1 管理员登录
	2.2 添加航班
	2.3 删除航班
	2.4 管理员查询航班

1.1 用户登录接口

- URL：`/api/user/login`

- 请求方式：POST

- 传入 JSON（客户端→服务端）：

```json
{
  "telephone": "string",  // 用户 用手机号码注册账号
  "password": "string"    // 用户密码
}
```

- 成功返回 JSON（服务端→客户端）：

```json
{
  "code": 200,
  "message": "操作成功",
  "data": {
    "telephone": "string",  // 登录成功的用户手机号
    "token": "string"       // 登录凭证（用于后续接口身份验证）
  }
}
```

- 失败返回 JSON（服务端→客户端）：

```json
{
  "code": 500,
  "message": "失败原因（如：账号或密码错误）",
  "data": null
}

```
1.2 用户注册接口

- URL：`/api/user/register`

- 请求方式：POST

- 传入 JSON（客户端→服务端）：

```json
{
  "telephone": "string",  // 注册手机号
  "password": "string",   // 注册密码
  "username": "string"    // 用户名
}
```

- 成功返回 JSON（服务端→客户端）：

```json
{
  "code": 200,
  "message": "注册成功",
  "data": {
    "telephone": "string"  // 注册成功的手机号
  }
}
```

- 失败返回 JSON（服务端→客户端）：

```json
{
  "code": 500,
  "message": "失败原因（如：手机号已被注册）",
  "data": null
}
```

1.3 航班查询接口

- URL：`/api/flight/search`

- 请求方式：GET

- 传入参数（URL 拼接）：

```json
{
  "departureCity": "string",  // 出发城市
  "arrivalCity": "string",    // 目的城市
  "date": "string"            // 航班日期（格式：YYYY-MM-DD）
}
```

  （URL 示例：/api/flight/search?departureCity=北京&arrivalCity=上海&date=2025-12-01）

- 成功返回 JSON（服务端→客户端）：

```json
`{
  "code": 200,
  "message": "操作成功",
  "data": [
    {
      "flightId": "number",       // 航班ID
      "flightNumber": "string",   // 航班号
      "departureCity": "string",  // 出发城市
      "arrivalCity": "string",    // 目的城市
      "departureTime": "string",  // 起飞时间（格式：HH:MM）
      "arrivalTime": "string",    // 到达时间（格式：HH:MM）
      "departureAirport": "string",// 出发机场
      "arrivalAirport": "string",  // 到达机场
      "checkinStartTime": "string",// 值机开始时间
      "checkinEndTime": "string",  // 值机结束时间
      "price": "number",           // 航班价格
      "airlineCompany": "string",  // 航空公司
      "status": "string"           // 航班状态（如：正常、延误）
    }
  ]
}`
```

- 失败返回 JSON（服务端→客户端）：

```json
{
  "code": 500,
  "message": "失败原因（如：无匹配航班）",
  "data": null
}

```
1.4 订单查询接口

- URL：`/api/order/query`

- 请求方式：GET

- 传入参数（URL 拼接）：

```json
{
  "telephone": "string"  // 登录用户的手机号
}
```

  （URL 示例：/api/order/query?telephone=13800138000）

- 成功返回 JSON（服务端→客户端）：

```json
{
  "code": 200,
  "message": "操作成功",
  "data": [
    {
      "orderId": "string",        // 订单ID
      "flightNumber": "string",   // 航班号
      "departureCity": "string",  // 出发城市
      "arrivalCity": "string",    // 目的城市
      "departureTime": "string",  // 起飞时间
      "passengers": [             // 乘客列表
        {
          "name": "string",       // 乘客姓名
          "idCard": "string"      // 乘客身份证号
        }
      ],
      "status": "string"          // 订单状态（如：待支付、已完成）
    }
  ]
}

```
- 失败返回 JSON（服务端→客户端）：

```json
{
  "code": 500,
  "message": "失败原因（如：无订单记录）",
  "data": null
}
```

1.5 机票预订接口

- URL：`/api/order/book`

- 请求方式：POST

- 传入 JSON（客户端→服务端）：

```json
{
  "telephone": "string",         // 预订用户的手机号
  "flightId": "number",          // 选择的航班ID
  "passengers": [                // 乘客信息列表
    {
      "name": "string",          // 乘客姓名
      "idCard": "string"         // 乘客身份证号
    }
  ]
}

```
- 成功返回 JSON（服务端→客户端）：

```json
{
  "code": 200,
  "message": "订票成功",
  "data": {
    "orderId": "string",  // 生成的订单ID
    "status": "string"    // 订单状态（如：待支付）
  }
}
```

- 失败返回 JSON（服务端→客户端）：

```json
{
  "code": 500,
  "message": "失败原因（如：航班余票不足）",
  "data": null
}

```
1.6 机票改签接口

- URL：`/api/order/change`

- 请求方式：POST

- 传入 JSON（客户端→服务端）：

```json
{
  "orderId": "string",    // 要改签的订单ID
  "newFlightId": "number" // 新选择的航班ID
}
```

- 成功返回 JSON（服务端→客户端）：

```json
{
  "code": 200,
  "message": "改签成功",
  "data": {
    "orderId": "string",  // 改签后的订单ID
    "newFlightNumber": "string" // 新航班号
  }
}
```

- 失败返回 JSON（服务端→客户端）：

```json
{
  "code": 500,
  "message": "失败原因（如：新航班无余票）",
  "data": null
}
```

2.1 管理员登录接口

- URL：`/api/admin/login`

- 请求方式：POST

- 传入 JSON（客户端→服务端）：

```json
{
  "adminAccount": "string",  // 管理员账号
  "adminPassword": "string"  // 管理员密码
}

```
- 成功返回 JSON（服务端→客户端）：

```json
{
  "code": 200,
  "message": "操作成功",
  "data": {
    "adminAccount": "string", // 登录成功的管理员账号
    "token": "string"         // 管理员登录凭证
  }
}
```

- 失败返回 JSON（服务端→客户端）：

```json
{
  "code": 500,
  "message": "失败原因（如：管理员账号不存在）",
  "data": null
}
```

2.2 管理员添加航班接口

- URL：`/api/admin/flight/add`

- 请求方式：POST

- 传入 JSON（客户端→服务端）：

```json
{
  "flightNumber": "string",   // 航班号
  "departureCity": "string",  // 出发城市
  "arrivalCity": "string",    // 目的城市
  "departureTime": "string",  // 起飞时间（格式：HH:MM）
  "arrivalTime": "string",    // 到达时间（格式：HH:MM）
  "departureAirport": "string",// 出发机场
  "arrivalAirport": "string",  // 到达机场
  "checkinStartTime": "string",// 值机开始时间
  "checkinEndTime": "string",  // 值机结束时间
  "price": "number",           // 航班价格
  "airlineCompany": "string",  // 航空公司
  "status": "string"           // 航班状态（如：正常）
}
```

- 成功返回 JSON（服务端→客户端）：

```json
{
  "code": 200,
  "message": "航班添加成功",
  "data": {
    "flightId": "number",     // 新增航班的ID
    "flightNumber": "string"  // 新增航班的航班号
  }
}
```

- 失败返回 JSON（服务端→客户端）：

```json
{
  "code": 500,
  "message": "失败原因（如：航班号已存在）",
  "data": null
}
```

2.3 管理员删除航班接口

- URL：`/api/admin/flight/delete`

- 请求方式：POST

- 传入 JSON（客户端→服务端）：

```json
{
  "flightId": "number"  // 要删除的航班ID
}
```

- 成功返回 JSON（服务端→客户端）：

```json
{
  "code": 200,
  "message": "航班删除成功",
  "data": {
    "flightId": "number"  // 已删除的航班ID
  }
}
```

- 失败返回 JSON（服务端→客户端）：

```json
{
  "code": 500,
  "message": "失败原因（如：航班不存在）",
  "data": null
}
```

2.4 管理员查询航班接口
- URL：`/api/admin/flight/search`

- 请求方式：GET

- 传入参数（URL 拼接）：

```json
{
  "flightNumber": "string",   // 航班号（可选，为空则查询所有）
  "departureCity": "string",  // 出发城市（可选）
  "arrivalCity": "string",    // 目的城市（可选）
  "date": "string"            // 航班日期（可选，格式：YYYY-MM-DD）
}
```

  （URL 示例1：/api/admin/flight/search?flightNumber=CA1521；示例2：/api/admin/flight/search?departureCity=北京&date=2025-12-01）

- 成功返回 JSON（服务端→客户端）：

```json
{
  "code": 200,
  "message": "操作成功",
  "data": [
    {
      "flightId": "number",       // 航班ID
      "flightNumber": "string",   // 航班号
      "departureCity": "string",  // 出发城市
      "arrivalCity": "string",    // 目的城市
      "departureTime": "string",  // 起飞时间（格式：HH:MM）
      "arrivalTime": "string",    // 到达时间（格式：HH:MM）
      "departureAirport": "string",// 出发机场
      "arrivalAirport": "string",  // 到达机场
      "checkinStartTime": "string",// 值机开始时间
      "checkinEndTime": "string",  // 值机结束时间
      "price": "number",           // 航班价格
      "airlineCompany": "string",  // 航空公司
      "status": "string",          // 航班状态（如：正常、延误）
      "leftTickets": "number"      // 剩余票数（管理员专属字段）
    }
  ]
}
```

- 失败返回 JSON（服务端→客户端）：

```json
{
  "code": 500,
  "message": "失败原因（如：无匹配航班）",
  "data": null
}
```

