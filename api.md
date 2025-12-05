[api updated.md](https://github.com/user-attachments/files/23953376/api.updated.md)

```markdown
1. 用户主要功能模块
	1.1 用户登录
	1.2 用户注册
	1.3 获取/修改用户信息（个人中心）
	1.4 航班查询（特定条件+分页）
	1.5 用户个人中心（个人订单）
	1.6 订票
	1.7 订单支付
	1.8 订单退签（针对已付款）
	1.9 删除订单（针对未付款）
	1.10 改签

2. 管理员主要功能模块
	2.1 管理员登录
	2.2 管理员注册
	2.3 航班信息增删改
	2.4 管理员查询航班
	2.5 管理员查看所有客户信息
	2.6 管理员查看所有订单信息

### 1. 用户相关接口
#### 1.1 用户登录接口
- URL：`/api/user/login`
- 请求方式：POST
- 传入 JSON（客户端→服务端）：
```json
{
  "telephone": "string",  // 用户用手机号码注册账号
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

#### 1.2 用户注册接口
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

#### 1.3 获取/修改用户信息接口（个人中心）
##### 1.3.1 获取用户信息
- URL：`/api/user/info/get`
- 请求方式：GET
- 传入参数（URL 拼接）：
```json
{
  "telephone": "string",  // 登录用户的手机号
  "token": "string"       // 登录凭证（身份验证）
}
```
- 成功返回 JSON（服务端→客户端）：
```json
{
  "code": 200,
  "message": "操作成功",
  "data": {
    "telephone": "string",  // 用户手机号
    "username": "string",   // 用户名
    "registerTime": "string"// 注册时间（格式：YYYY-MM-DD HH:MM:SS）
  }
}
```
- 失败返回 JSON（服务端→客户端）：
```json
{
  "code": 500,
  "message": "失败原因（如：用户不存在、token 失效）",
  "data": null
}
```

##### 1.3.2 修改用户信息
- URL：`/api/user/info/update`
- 请求方式：POST
- 传入 JSON（客户端→服务端）：
```json
{
  "telephone": "string",  // 登录用户的手机号
  "token": "string",      // 登录凭证（身份验证）
  "username": "string",   // 新用户名（可选，不修改则不填）
  "password": "string"    // 新密码（可选，不修改则不填）
}
```
- 成功返回 JSON（服务端→客户端）：
```json
{
  "code": 200,
  "message": "信息修改成功",
  "data": {
    "telephone": "string",  // 用户手机号
    "username": "string"    // 修改后的用户名（未修改则返回原用户名）
  }
}
```
- 失败返回 JSON（服务端→客户端）：
```json
{
  "code": 500,
  "message": "失败原因（如：token 失效、参数为空）",
  "data": null
}
```

#### 1.4 航班查询接口（特定条件+分页）
- URL：`/api/flight/search`
- 请求方式：GET
- 传入参数（URL 拼接）：
```json
{
  "departureCity": "string",  // 出发城市（可选，为空则不限）
  "arrivalCity": "string",    // 目的城市（可选，为空则不限）
  "date": "string",           // 航班日期（可选，格式：YYYY-MM-DD，为空则不限）
  "airlineCompany": "string", // 航空公司（可选，为空则不限）
  "minPrice": "number",       // 最低价格（可选，为空则不限）
  "maxPrice": "number",       // 最高价格（可选，为空则不限）
  "pageNum": "number",        // 页码（必填，从 1 开始）
  "pageSize": "number"        // 每页条数（必填，如 10、20）
}
```
- URL 示例：`/api/flight/search?departureCity=北京&arrivalCity=上海&date=2025-12-01&pageNum=1&pageSize=10`
- 成功返回 JSON（服务端→客户端）：
```json
{
  "code": 200,
  "message": "操作成功",
  "data": {
    "total": "number",        // 总航班数
    "pageNum": "number",      // 当前页码
    "pageSize": "number",     // 每页条数
    "list": [
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
  }
}
```
- 失败返回 JSON（服务端→客户端）：
```json
{
  "code": 500,
  "message": "失败原因（如：无匹配航班、页码格式错误）",
  "data": null
}
```

#### 1.5 订单查询接口（个人中心）
- URL：`/api/order/query`
- 请求方式：GET
- 传入参数（URL 拼接）：
```json
{
  "telephone": "string",  // 登录用户的手机号
  "token": "string",      // 登录凭证（身份验证）
  "status": "string"      // 订单状态（可选，如：待支付、已完成、已退签，为空则查询所有）
}
```
- URL 示例：`/api/order/query?telephone=13800138000&token=xxx&status=待支付`
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
      "status": "string",         // 订单状态（如：待支付、已完成）
      "createTime": "string"      // 订单创建时间（格式：YYYY-MM-DD HH:MM:SS）
    }
  ]
}
```
- 失败返回 JSON（服务端→客户端）：
```json
{
  "code": 500,
  "message": "失败原因（如：无订单记录、token 失效）",
  "data": null
}
```

#### 1.6 机票预订接口
- URL：`/api/order/book`
- 请求方式：POST
- 传入 JSON（客户端→服务端）：
```json
{
  "telephone": "string",         // 预订用户的手机号
  "token": "string",             // 登录凭证（身份验证）
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
  "message": "失败原因（如：航班余票不足、token 失效）",
  "data": null
}
```

#### 1.7 订单支付接口
- URL：`/api/order/pay`
- 请求方式：POST
- 传入 JSON（客户端→服务端）：
```json
{
  "telephone": "string",  // 登录用户的手机号
  "token": "string",      // 登录凭证（身份验证）
  "orderId": "string"     // 要支付的订单ID
}
```
- 成功返回 JSON（服务端→客户端）：
```json
{
  "code": 200,
  "message": "支付成功",
  "data": {
    "orderId": "string",  // 支付的订单ID
    "status": "string",   // 订单状态（已支付）
    "payTime": "string"   // 支付时间（格式：YYYY-MM-DD HH:MM:SS）
  }
}
```
- 失败返回 JSON（服务端→客户端）：
```json
{
  "code": 500,
  "message": "失败原因（如：订单不存在、订单已支付、token 失效）",
  "data": null
}
```

#### 1.8 订单退签接口（针对已付款）
- URL：`/api/order/refund`
- 请求方式：POST
- 传入 JSON（客户端→服务端）：
```json
{
  "telephone": "string",  // 登录用户的手机号
  "token": "string",      // 登录凭证（身份验证）
  "orderId": "string"     // 要退签的订单ID
}
```
- 成功返回 JSON（服务端→客户端）：
```json
{
  "code": 200,
  "message": "退签成功",
  "data": {
    "orderId": "string",  // 退签的订单ID
    "status": "string",   // 订单状态（已退签）
    "refundTime": "string"// 退签时间（格式：YYYY-MM-DD HH:MM:SS）
  }
}
```
- 失败返回 JSON（服务端→客户端）：
```json
{
  "code": 500,
  "message": "失败原因（如：订单不存在、订单未支付、已过退签时间、token 失效）",
  "data": null
}
```

#### 1.9 删除订单接口（针对未付款）
- URL：`/api/order/delete`
- 请求方式：POST
- 传入 JSON（客户端→服务端）：
```json
{
  "telephone": "string",  // 登录用户的手机号
  "token": "string",      // 登录凭证（身份验证）
  "orderId": "string"     // 要删除的订单ID
}
```
- 成功返回 JSON（服务端→客户端）：
```json
{
  "code": 200,
  "message": "订单删除成功",
  "data": {
    "orderId": "string"  // 已删除的订单ID
  }
}
```
- 失败返回 JSON（服务端→客户端）：
```json
{
  "code": 500,
  "message": "失败原因（如：订单不存在、订单已支付、token 失效）",
  "data": null
}
```

#### 1.10 机票改签接口
- URL：`/api/order/change`
- 请求方式：POST
- 传入 JSON（客户端→服务端）：
```json
{
  "telephone": "string",    // 登录用户的手机号
  "token": "string",        // 登录凭证（身份验证）
  "orderId": "string",      // 要改签的订单ID
  "newFlightId": "number"   // 新选择的航班ID
}
```
- 成功返回 JSON（服务端→客户端）：
```json
{
  "code": 200,
  "message": "改签成功",
  "data": {
    "orderId": "string",        // 改签后的订单ID
    "newFlightNumber": "string" // 新航班号
  }
}
```
- 失败返回 JSON（服务端→客户端）：
```json
{
  "code": 500,
  "message": "失败原因（如：新航班无余票、订单不存在、token 失效）",
  "data": null
}
```

### 2. 管理员相关接口
#### 2.1 管理员登录接口
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
  "message": "失败原因（如：管理员账号不存在、密码错误）",
  "data": null
}
```

#### 2.2 管理员注册接口
- URL：`/api/admin/register`
- 请求方式：POST
- 传入 JSON（客户端→服务端）：
```json
{
  "adminAccount": "string",  // 管理员账号
  "adminPassword": "string", // 管理员密码
  "adminName": "string"      // 管理员姓名
}
```
- 成功返回 JSON（服务端→客户端）：
```json
{
  "code": 200,
  "message": "注册成功",
  "data": {
    "adminAccount": "string"  // 注册成功的管理员账号
  }
}
```
- 失败返回 JSON（服务端→客户端）：
```json
{
  "code": 500,
  "message": "失败原因（如：管理员账号已被注册）",
  "data": null
}
```

#### 2.3 航班信息增删改接口
##### 2.3.1 增加航班
- URL：`/api/admin/flight/add`
- 请求方式：POST
- 传入 JSON（客户端→服务端）：
```json
{
  "adminAccount": "string",   // 管理员账号
  "token": "string",          // 管理员登录凭证
  "flightNumber": "string",   // 航班号
  "departureCity": "string",  // 出发城市
  "arrivalCity": "string",    // 目的城市
  "date": "string",           // 航班日期（格式：YYYY-MM-DD）
  "departureTime": "string",  // 起飞时间（格式：HH:MM）
  "arrivalTime": "string",    // 到达时间（格式：HH:MM）
  "departureAirport": "string",// 出发机场
  "arrivalAirport": "string",  // 到达机场
  "checkinStartTime": "string",// 值机开始时间
  "checkinEndTime": "string",  // 值机结束时间
  "price": "number",           // 航班价格
  "airlineCompany": "string",  // 航空公司
  "status": "string",          // 航班状态（如：正常）
  "leftTickets": "number"      // 初始票数
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
  "message": "失败原因（如：航班号已存在、token 失效）",
  "data": null
}
```

##### 2.3.2 删除航班
- URL：`/api/admin/flight/delete`
- 请求方式：POST
- 传入 JSON（客户端→服务端）：
```json
{
  "adminAccount": "string",  // 管理员账号
  "token": "string",         // 管理员登录凭证
  "flightId": "number"       // 要删除的航班ID
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
  "message": "失败原因（如：航班不存在、航班已关联订单、token 失效）",
  "data": null
}
```

##### 2.3.3 修改航班
- URL：`/api/admin/flight/update`
- 请求方式：POST
- 传入 JSON（客户端→服务端）：
```json
{
  "adminAccount": "string",   // 管理员账号
  "token": "string",          // 管理员登录凭证
  "flightId": "number",       // 要修改的航班ID（必传）
  "flightNumber": "string",   // 航班号（可选，若修改需校验唯一性）
  "departureCity": "string",  // 出发城市（可选）
  "arrivalCity": "string",    // 目的城市（可选）
  "date": "string",           // 航班日期（可选，格式：YYYY-MM-DD）
  "departureTime": "string",  // 起飞时间（可选，格式：HH:MM）
  "arrivalTime": "string",    // 到达时间（可选，格式：HH:MM）
  "price": "number",          // 价格（可选）
  "status": "string",         // 状态（可选，如：正常→延误）
  "leftTickets": "number"     // 剩余票数（可选）
}
```
- 成功返回 JSON（服务端→客户端）：
```json
{
  "code": 200,
  "message": "航班修改成功",
  "data": {
    "flightId": "number",     // 修改后的航班ID
    "flightNumber": "string"  // 修改后的航班号（若未修改则返回原航班号）
  }
}
```
- 失败返回 JSON（服务端→客户端）：
```json
{
  "code": 500,
  "message": "失败原因（如：航班不存在、修改后的航班号已存在、token 失效）",
  "data": null
}
```

#### 2.4 管理员查询航班接口
- URL：`/api/admin/flight/search`
- 请求方式：GET
- 传入参数（URL 拼接）：
```json
{
  "adminAccount": "string",   // 管理员账号
  "token": "string",          // 管理员登录凭证
  "flightNumber": "string",   // 航班号（可选，为空则查询所有）
  "departureCity": "string",  // 出发城市（可选）
  "arrivalCity": "string",    // 目的城市（可选）
  "date": "string",           // 航班日期（可选，格式：YYYY-MM-DD）
  "pageNum": "number",        // 页码（可选，从 1 开始，为空则不分页）
  "pageSize": "number"        // 每页条数（可选，为空则不分页）
}
```
- URL 示例1：`/api/admin/flight/search?adminAccount=admin&token=xxx&flightNumber=CA1521`
- URL 示例2：`/api/admin/flight/search?adminAccount=admin&token=xxx&departureCity=北京&date=2025-12-01&pageNum=1&pageSize=20`
- 成功返回 JSON（服务端→客户端）：
```json
{
  "code": 200,
  "message": "操作成功",
  "data": {
    "total": "number",        // 总航班数（分页时返回）
    "pageNum": "number",      // 当前页码（分页时返回）
    "pageSize": "number",     // 每页条数（分页时返回）
    "list": [
      {
        "flightId": "number",       // 航班ID
        "flightNumber": "string",   // 航班号
        "departureCity": "string",  // 出发城市
        "arrivalCity": "string",    // 目的城市
        "date": "string",           // 航班日期（格式：YYYY-MM-DD）
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
}
```
- 失败返回 JSON（服务端→客户端）：
```json
{
  "code": 500,
  "message": "失败原因（如：无匹配航班、token 失效）",
  "data": null
}
```

#### 2.5 管理员查看所有客户信息接口
- URL：`/api/admin/user/all`
- 请求方式：GET
- 传入参数（URL 拼接）：
```json
{
  "adminAccount": "string",   // 管理员账号
  "token": "string",          // 管理员登录凭证
  "pageNum": "number",        // 页码（必填，从 1 开始）
  "pageSize": "number"        // 每页条数（必填，如 10、20）
}
```
- URL 示例：`/api/admin/user/all?adminAccount=admin&token=xxx&pageNum=1&pageSize=20`
- 成功返回 JSON（服务端→客户端）：
```json
{
  "code": 200,
  "message": "操作成功",
  "data": {
    "total": "number",        // 总客户数
    "pageNum": "number",      // 当前页码
    "pageSize": "number",     // 每页条数
    "list": [
      {
        "telephone": "string",  // 客户手机号
        "username": "string",   // 客户姓名
        "registerTime": "string"// 注册时间（格式：YYYY-MM-DD HH:MM:SS）
      }
    ]
  }
}
```
- 失败返回 JSON（服务端→客户端）：
```json
{
  "code": 500,
  "message": "失败原因（如：无客户信息、token 失效）",
  "data": null
}
```

#### 2.6 管理员查看所有订单信息接口
- URL：`/api/admin/order/all`
- 请求方式：GET
- 传入参数（URL 拼接）：
```json
{
  "adminAccount": "string",   // 管理员账号
  "token": "string",          // 管理员登录凭证
  "status": "string",         // 订单状态（可选，如：待支付、已完成，为空则查询所有）
  "startDate": "string",      // 订单创建开始日期（可选，格式：YYYY-MM-DD）
  "endDate": "string",        // 订单创建结束日期（可选，格式：YYYY-MM-DD）
  "pageNum": "number",        // 页码（必填，从 1 开始）
  "pageSize": "number"        // 每页条数（必填，如 10、20）
}
```
- URL 示例：`/api/admin/order/all?adminAccount=admin&token=xxx&status=已完成&startDate=2025-01-01&endDate=2025-12-31&pageNum=1&pageSize=20`
- 成功返回 JSON（服务端→客户端）：
```json
{
  "code": 200,
  "message": "操作成功",
  "data": {
    "total": "number",        // 总订单数
    "pageNum": "number",      // 当前页码
    "pageSize": "number",     // 每页条数
    "list": [
      {
        "orderId": "string",        // 订单ID
        "telephone": "string",      // 下单用户手机号
        "username": "string",       // 下单用户姓名
        "flightNumber": "string",   // 航班号
        "departureCity": "string",  // 出发城市
        "arrivalCity": "string",    // 目的城市
        "departureTime": "string",  // 起飞时间
        "createTime": "string",     // 订单创建时间（格式：YYYY-MM-DD HH:MM:SS）
        "status": "string",         // 订单状态
        "passengers": [             // 乘客列表
          {
            "name": "string",       // 乘客姓名
            "idCard": "string"      // 乘客身份证号
          }
        ]
      }
    ]
  }
}
```
- 失败返回 JSON（服务端→客户端）：
```json
{
  "code": 500,
  "message": "失败原因（如：无订单信息、token 失效、日期格式错误）",
  "data": null
}
```
