const emailModule = require('./email');
const { v4: uuidv4 } = require('uuid');
const message_proto = require('./proto');
const constModule = require('./const')
const grpc = require('@grpc/grpc-js')
const configModule = require('./config')

async function GetVerifyCode(call, callback) {
    console.log("email is ", call.request.email)
    try {

        let uniqueId = uuidv4();
        console.log("uniqueId is ", uniqueId);
        let text_str = '您的验证码为 【' + uniqueId + '】 请三分钟内完成注册';
        // 发送邮件
        let mailOptions = {
            from: configModule.email_user,
            to: call.request.email,
            subject: '验证码',
            text: text_str,
        };

        let send_res = await emailModule.SendMail(mailOptions);
        console.log("send_res is ", send_res);

        callback(null, {
            email: call.request.email,
            error: constModule.Errors.Success
        });

    }
    catch (error) {

        console.log("catch error is ", error)
        callback(null, {
            email: call.request.email,
            error: constModule.Errors.Exception
        });
    }
}


function main() {
    var server = new grpc.Server()
    server.addService(message_proto.VerifyService.service, { GetVerifyCode: GetVerifyCode })
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), () => {
        console.log('----grpc server started----')
    })
}

main()