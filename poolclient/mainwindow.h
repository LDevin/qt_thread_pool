#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qghtcpclient.h"
#include <QMap>
#include <QStandardItem>
#include <QTimerEvent>
#include <QStandardItemModel>
#include <QDateTime>
#include <QAbstractSocket>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    virtual void timerEvent(QTimerEvent *event);

private slots:
    void on_connectBtn_clicked();
    void showSocketError(QAbstractSocket::SocketError err);

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    int m_timerId;
    bool isConnt;

    QMap<QGHTcpClient*, QDateTime> m_clientList;
    QStandardItemModel model;

    void displayMsg(const QString &msg);

    void new_data_received();
    void new_client_connected();
    void new_client_disconnected();
    void byteWritten(qint64 bytes);
};

#endif // MAINWINDOW_H
