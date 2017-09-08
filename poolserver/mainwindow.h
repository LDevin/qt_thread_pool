#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimerEvent>
#include <QStandardItemModel>
#include "qghtcpclient.h"
#include "qghtcpserver.h"
#include "qghthreadengine.h"
#include "qghthreadobject.h"
#include "mytaskitem.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void timerEvent(QTimerEvent *e);

private slots:
    void on_listenBtn_clicked();

private:
    Ui::MainWindow *ui;

    int m_timerId;

    QStandardItemModel *m_clientModel;
    QStandardItemModel *m_msgModel;

    QGHTcpServer *m_tcpSer;
    QGHThreadEngine *m_engine;
    MyTaskItem *m_myTaskItem;

    bool isListen;

    QMap<QObject*,int> m_ClientRecieved;
    QMap<QObject*,int> m_ClientSended;

public slots:
    void clientConnectSlot(QObject *client);
    void clientDisConnectSlot(QObject *client);
    void clientDataReceivedSlot(QObject *client, const QByteArray &data);
    void clientDataSendSlot(QObject *client, qint64 bytes);

    void on_killBtn_clicked();
};

#endif // MAINWINDOW_H
