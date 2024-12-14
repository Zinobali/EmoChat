const path = require('path')
const grpc = require('@grpc/grpc-js')
const protoLoader = require('@grpc/proto-loader')

const PROTO_PATH = path.join(__dirname, 'message.proto')
const packageDefinition = protoLoader.loadSync(PROTO_PATH, {
    keepCase: true, // 保持字段名的原始大小写，避免自动转换为驼峰命名。
    longs: String,  //将 Protocol Buffers 中的 long 类型字段表示为 JavaScript 的字符串。
    enums: String,  //将枚举类型表示为字符串，而非数字。
    defaults: true, //自动填充未赋值字段的默认值。
    oneofs: true    // 为 oneof 字段创建额外的属性，用于检查哪个字段被设置。
})
const protoDescriptor = grpc.loadPackageDefinition(packageDefinition)
const message_proto = protoDescriptor.message

module.exports = message_proto

