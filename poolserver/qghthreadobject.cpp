#include "qghthreadobject.h"
#include "assert.h"


QGHThreadObject::QGHThreadObject(QObject *parent, QGHThreadTaskItem *taskItem)
    : QObject(parent), item(taskItem)
{

}

QGHThreadObject::~QGHThreadObject()
{

}

void QGHThreadObject::setTaskItem(QGHThreadTaskItem *pItem)
{
    assert(pItem != NULL);
    item = pItem;
}

void QGHThreadObject::process(QObject *data_source, const QByteArray &data, QObject *target)
{
    if (target == this) {
        item->run(data_source, data);
        emit sig_process_finished(data_source);
    }
}
