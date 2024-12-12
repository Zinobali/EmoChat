#pragma once

class ConfigMgr;
extern ConfigMgr g_configMgr;

enum class ErrorCodes {
    Success = 0,
    Error_Json = 1001,  //Json½âÎö´íÎó
    RPCFailed = 1002,  //RPCÇëÇó´íÎó
};