#ifndef QGHTHREADENGINE_H
#define QGHTHREADENGINE_H
#include <QObject>
#include <QMap>
#include <QVector>
#include <QMutex>


class QGHThreadEngine : public QObject
{
    Q_OBJECT

public:
    QGHThreadEngine();
    ~QGHThreadEngine();
};

#endif // QGHTHREADENGINE_H
