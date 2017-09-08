#ifndef QGHTHREADTASKITEM_H
#define QGHTHREADTASKITEM_H
#include <QObject>
#include <QByteArray>

class QGHThreadTaskItem : public QObject
{
    Q_OBJECT

public:
    QGHThreadTaskItem(QObject *parent) {}
    ~QGHThreadTaskItem() {}

    virtual void run(QObject *tast_data, const QByteArray &byte) = 0;
signals:
    void sendHandleData(QObject *obj, QByteArray &byte);
};

#endif // QGHTHREADTASKITEM_H
