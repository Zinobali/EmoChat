#include "logicmgr.h"
#include "global.h"

LogicWorker::LogicWorker(QObject *parent)
{
    InitHandlers();
}

void LogicWorker::InitHandlers()
{
    // 上传文件回包
    _handlers[RequestId::ID_UPLOAD_FILE_RSP] = [this](QJsonObject obj)
    {
        auto err = obj["error"].toInt();
        if (err != ErrorCodes::SUCCESS)
        {
            qDebug() << "upload msg rsp err is " << err;
            return;
        }

        auto name = obj["name"].toString();
        auto total_size = obj["total_size"].toInt();
        auto trans_size = obj["trans_size"].toInt();

        qDebug() << "recv : " << name << " file trans_size is " << trans_size;
        emit sig_trans_size(trans_size);
    };
}

void LogicWorker::slot_logic_process(quint16 msgid, QJsonObject obj)
{
    if (!_handlers.contains(msgid))
    {
        qDebug() << "msgid: " << msgid << " not found";
        return;
    }
    // 执行对应的处理函数
    _handlers[msgid](obj);
}

LogicMgr::~LogicMgr()
{
    _work_thread->quit();
    _work_thread->wait();
    _work_thread->deleteLater();
    _worker->deleteLater();
}

LogicMgr::LogicMgr(QObject *parent)
    : QObject{parent}
{
    _worker = new LogicWorker();
    _work_thread = new QThread(this);

    // worker移动到子线程处理消息
    _worker->moveToThread(_work_thread);

    // 连接信号和槽函数
    connect(this, &LogicMgr::sig_logic_process, _worker, &LogicWorker::slot_logic_process);
    connect(_worker, &LogicWorker::sig_trans_size, this, &LogicMgr::sig_trans_size);

    // 启动线程
    _work_thread->start();
}
