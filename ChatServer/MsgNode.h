#pragma once
#include <cstdint>
#include <vector>
#include <string>


constexpr int MAX_LENGTH = 1024 * 2;

constexpr uint16_t HEAD_TOTAL_LEN = 4;
constexpr uint16_t HEAD_ID_LEN = 2;
constexpr uint16_t HEAD_DATA_LEN = 2;

class LogicSystem;
class CSession;
class MsgNode
{
    friend class CSession;
    friend class LogicSystem;
public:
    MsgNode(uint16_t max_len);
    void Clear();
protected:
    uint16_t cur_len_;
    uint16_t total_len_;
    std::vector<char> buffer_; //使用vector的原因是，不需要手动管理内存，方便初始化boost::asio::buffer，不会越界，自动扩容
};

class RecvNode : public MsgNode
{
    friend class CSession;
    friend class LogicSystem;
public:
    RecvNode(uint16_t msg_id, uint16_t msg_len);
private:
    uint16_t msg_id_;
};

class SendNode : public MsgNode
{
    friend class CSession;
    friend class LogicSystem;
public:
    SendNode(uint16_t msg_id, const std::string& msg);
private:
    uint16_t msg_id_;
};

