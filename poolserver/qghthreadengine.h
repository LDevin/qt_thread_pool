#ifndef QGHTHREADENGINE_H
#define QGHTHREADENGINE_H
#include <QObject>
#include <QMap>
#include <QVector>
#include <QMutex>
#include "qghthreadobject.h"
#include "qghthreadtaskitem.h"
#include <QThread>


class QGHThreadEngine : public QObject
{
    Q_OBJECT

public:
    QGHThreadEngine(QObject *parent, QGHThreadTaskItem * pTaskItem,int nThreads = 2,bool bFIFOKeep = true);
    ~QGHThreadEngine();

private:
    QVector<QThread *> m_ThreadPool;
    QVector<QGHThreadObject *> m_ThreadObjs;
    QGHThreadTaskItem *_item;
    int m_thread_count;
    bool m_bFIFOKeep;

    //各个m_ThreadPool\m_ThreadObjs的任务数
    QMap<QObject *,qint32> m_map_Tasks;
    //m_bFIFOKeep == true 时，下面两个成员将保证非空闲的单个 data_source 将始终在单一线程处理
    //各个data_source 目前的处理线程
    QMap<QObject *,QObject *> m_map_busy_source_task;
    //各个data_source 目前的排队数目
    QMap<QObject *,int> m_map_busy_source_counter;

public:
    void SetThreadTaskItem(QGHThreadTaskItem * pTaskItem) {_item = pTaskItem;}
    QList<qint32> CurrentLoad()
    {
        return m_map_Tasks.values();
    }

public slots:
    void append_new(QObject * data_source, const QByteArray & data);
    //捕获QGHThreadObject::sig_process_finished, 以便管理data_source的 FIFO 顺序
    void on_sig_process_finished(QObject * data_source);
signals:
    //************************************
    // Method:    do_task
    // FullName:  QGHThreadEngine::do_task
    // Access:    public
    // Returns:   void
    // Qualifier:
    // Parameter: QObject *		任务来源 （相同任务源的任务，在队列非空时会被安排到同一个线程处理，以确保对相同源的FIFO）
    // Parameter: QByteArray	任务体
    // Parameter: QObject *		处理任务的线程对象（QGHThreadObject）
    //************************************
    void do_task(QObject *, const QByteArray &,QObject *);
};

#endif // QGHTHREADENGINE_H
