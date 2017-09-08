#ifndef QGHTHREADOBJECT_H
#define QGHTHREADOBJECT_H
#include <QObject>
#include "qghthreadtaskitem.h"


class QGHThreadObject : public QObject
{
    Q_OBJECT
public:
    QGHThreadObject(QObject *parent, QGHThreadTaskItem *taskItem);
    ~QGHThreadObject();

    void setTaskItem(QGHThreadTaskItem *pItem);

private:
    QGHThreadTaskItem *item;

public slots:
    void process(QObject *data_source, const QByteArray &data, QObject *target);

signals:
    //信号，表示一次处理已经完成。QGHThreadEngine捕获该信号，管理data_source的 FIFO 顺序
    void sig_process_finished(QObject * data_source);
};
#endif // QGHTHREADOBJECT_H
