#ifndef MYTASKITEM_H
#define MYTASKITEM_H
#include "qghthreadtaskitem.h"

class MyTaskItem : public QGHThreadTaskItem
{
//Q_OBJECT
public:
    MyTaskItem(QObject *parent);
    ~MyTaskItem();

    void run(QObject *tast_data, const QByteArray &byte);



};
#endif // MYTASKITEM_H
