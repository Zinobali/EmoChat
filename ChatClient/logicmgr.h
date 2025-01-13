#ifndef LOGICMGR_H
#define LOGICMGR_H

#include <QObject>
#include "singleton.h"
#include <QThread>
#include <QMap>
#include <functional>
#include <QJsonObject>

class LogicWorker : public QObject
{
    Q_OBJECT
public:
    LogicWorker(QObject *parent = nullptr);
    void InitHandlers();

private:
    QMap<quint32, std::function<void(QJsonObject obj)>> _handlers;

signals:
    void sig_trans_size(int trans_size);

public slots:
    void slot_logic_process(quint16 msgid, QJsonObject obj);
};

class LogicMgr : public QObject, public Singleton<LogicMgr>
{
    Q_OBJECT
    friend class Singleton<LogicMgr>;

public:
    ~LogicMgr();

private:
    explicit LogicMgr(QObject *parent = nullptr);
    LogicWorker *_worker;
    QThread *_work_thread;

signals:
    void sig_logic_process(quint16 msgid, QJsonObject obj);
    void sig_trans_size(int trans_size);
};

#endif // LOGICMGR_H
