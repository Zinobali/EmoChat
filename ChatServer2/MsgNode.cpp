#include "MsgNode.h"
#include <boost/endian.hpp>

MsgNode::MsgNode(uint16_t max_len) :total_len_(max_len), cur_len_(0), buffer_(max_len) {}

void MsgNode::Clear() {
    std::memset(buffer_.data(), 0, total_len_);
    cur_len_ = 0;
}

RecvNode::RecvNode(uint16_t msg_id, uint16_t msg_len) :MsgNode(msg_len), msg_id_(msg_id) {}

SendNode::SendNode(uint16_t msg_id, const std::string& msg)
    :MsgNode(msg.size() + HEAD_TOTAL_LEN), msg_id_(msg_id) {
    uint16_t net_id = boost::endian::native_to_big(msg_id);
    //重大bug，native_to_big函数接收的数据类型可以是任意，但本地协议是ushort 类型，所以这里要显式转换
    uint16_t net_len = boost::endian::native_to_big(static_cast<uint16_t>(msg.size()));

    char* buf = buffer_.data();
    std::memcpy(buf, &net_id, HEAD_ID_LEN);
    std::memcpy(buf + HEAD_ID_LEN, &net_len, HEAD_DATA_LEN);
    std::memcpy(buf + HEAD_TOTAL_LEN, msg.c_str(), msg.size()); // 明显效率更高
    //std::copy(msg.begin(), msg.end(), buffer_.begin() + HEAD_TOTAL_LEN); // 只为了复习一下copy函数
}
