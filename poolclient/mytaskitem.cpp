#include "mytaskitem.h"

MyTaskItem::MyTaskItem(QObject *parent)
    :QGHThreadTaskItem(parent)
{

}

MyTaskItem::~MyTaskItem()
{

}

void MyTaskItem::run(QObject *tast_data, const QByteArray &byte)
{
    QByteArray res;
    foreach (char c,byte)
    {
        res.push_back((c>='a'&& c<='z')?c+('A'-'a'):c);
        if (c==015)
            res.push_back(012);

    }
    _sleep (50);
    emit sendHandleData(tast_data,res);
    return ;
}
