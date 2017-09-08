#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_tcpSer = new QGHTcpServer(this);

    m_clientModel = new QStandardItemModel(0, 4, this);
    m_msgModel = new QStandardItemModel(0, 2, this);

    m_clientModel->setHeaderData(0, Qt::Horizontal, "ID");
    m_clientModel->setHeaderData(1, Qt::Horizontal, "IP");
    m_clientModel->setHeaderData(2, Qt::Horizontal, "RECEIVED");
    m_clientModel->setHeaderData(3, Qt::Horizontal, "SENDED");

    m_msgModel->setHeaderData(0, Qt::Horizontal, "ID");
    m_msgModel->setHeaderData(1, Qt::Horizontal, "PAYLOAD");

    ui->clientTableView->setModel(m_clientModel);
    ui->poolTableView->setModel(m_msgModel);
    isListen = false;
}

MainWindow::~MainWindow()
{
    delete ui;

    if (m_engine) {
        disconnect(m_tcpSer, &QGHTcpServer::sig_new_data_received, m_engine, &QGHThreadEngine::append_new);
        disconnect(m_myTaskItem, &MyTaskItem::sendHandleData, m_tcpSer, &QGHTcpServer::sendDataToClient);
        disconnect(m_tcpSer, &QGHTcpServer::sig_new_connect, this, &MainWindow::clientConnectSlot);
        disconnect(m_tcpSer, &QGHTcpServer::sig_new_disconnect, this, &MainWindow::clientDisConnectSlot);
        disconnect(m_tcpSer, &QGHTcpServer::sig_new_data_received, this, &MainWindow::clientDataReceivedSlot);
        disconnect(m_tcpSer, &QGHTcpServer::sig_new_data_write, this, &MainWindow::clientDataSendSlot);

        m_tcpSer->KickAllClients();
        m_tcpSer->close();
        killTimer(m_timerId);

        m_myTaskItem->deleteLater();
        m_engine->deleteLater();

        m_myTaskItem = NULL;
        m_engine = NULL;

        m_clientModel->removeRows(0, m_clientModel->rowCount());
        m_msgModel->removeRows(0, m_msgModel->rowCount());
    }
    m_tcpSer->deleteLater();
}

void MainWindow::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == m_timerId) {
    //qDebug()<<"timereventg";
        if (m_engine) {
            QList<qint32> list = m_engine->CurrentLoad();
            int index = 0;
            foreach (qint32 li, list) {
                QString ind = tr("%1").arg(index+1);
                QString id = tr("%1").arg(li);
                m_msgModel->setItem(index, 0, new QStandardItem(ind));
                m_msgModel->setItem(index, 1, new QStandardItem(id));
                index++;
            }
        }

        if (m_tcpSer) {

            int index = 0;
            QList<QObject*> clientS = m_tcpSer->clients();
            foreach (QObject *client, clientS) {
                QString ind = tr("%1").arg((qint64)client);
                QString sip;
                QGHTcpClient *socket = qobject_cast<QGHTcpClient*>(client);

                if (socket) {
                    sip = socket->peerAddress().toString();
                }

                m_clientModel->setItem(index, 0, new QStandardItem(ind));
                m_clientModel->setItem(index, 1, new QStandardItem(sip));


                if (m_ClientRecieved.find(client) != m_ClientRecieved.end()) {
                    QString rece = tr("%1").arg(m_ClientRecieved[client]);
                    m_clientModel->setItem(index, 2, new QStandardItem(rece));
                }

                if (m_ClientSended.find(client) != m_ClientSended.end()) {
                    QString send = tr("%1").arg(m_ClientSended[client]);
                    m_clientModel->setItem(index, 3, new QStandardItem(send));
                }

                index++;
            }

            while (m_clientModel->rowCount() > index) {
                m_clientModel->removeRow(m_clientModel->rowCount() - 1);
            }
        }
    }
}

void MainWindow::on_listenBtn_clicked()
{
    isListen = !isListen;

    int port = ui->portEdit->text().toInt(0);
    if (port < 1 || port > 32767) {
        return;
    }

    int nThreadCount = ui->threadEdit->text().toInt(0);
    if (nThreadCount < 1 || nThreadCount > 64) {
        return;
    }

    bool is_fifo = ui->checkBox->isChecked();

    if (isListen && !m_tcpSer->isListening()) {

        if (m_tcpSer->listen(QHostAddress::Any, port)) {
            m_tcpSer->setPayLoad(ui->payLoadEdit->text().toInt(0));

            m_myTaskItem =  new MyTaskItem(this);
            m_engine = new QGHThreadEngine(this, m_myTaskItem, nThreadCount, is_fifo);

            connect(m_tcpSer, &QGHTcpServer::sig_new_data_received, m_engine, &QGHThreadEngine::append_new);
            connect(m_myTaskItem, &MyTaskItem::sendHandleData, m_tcpSer, &QGHTcpServer::sendDataToClient, Qt::DirectConnection);
            connect(m_tcpSer, &QGHTcpServer::sig_new_connect, this, &MainWindow::clientConnectSlot);
            connect(m_tcpSer, &QGHTcpServer::sig_new_disconnect, this, &MainWindow::clientDisConnectSlot);
            connect(m_tcpSer, &QGHTcpServer::sig_new_data_received, this, &MainWindow::clientDataReceivedSlot);
            connect(m_tcpSer, &QGHTcpServer::sig_new_data_write, this, &MainWindow::clientDataSendSlot);

            qDebug()<<"the server is listening now !!!";
            m_timerId = startTimer(500);

        }

    } else if (!isListen && m_tcpSer->isListening()) {
        disconnect(m_tcpSer, &QGHTcpServer::sig_new_data_received, m_engine, &QGHThreadEngine::append_new);
        disconnect(m_myTaskItem, &MyTaskItem::sendHandleData, m_tcpSer, &QGHTcpServer::sendDataToClient);
        disconnect(m_tcpSer, &QGHTcpServer::sig_new_connect, this, &MainWindow::clientConnectSlot);
        disconnect(m_tcpSer, &QGHTcpServer::sig_new_disconnect, this, &MainWindow::clientDisConnectSlot);
        disconnect(m_tcpSer, &QGHTcpServer::sig_new_data_received, this, &MainWindow::clientDataReceivedSlot);
        disconnect(m_tcpSer, &QGHTcpServer::sig_new_data_write, this, &MainWindow::clientDataSendSlot);

        m_tcpSer->KickAllClients();
        m_tcpSer->close();
        killTimer(m_timerId);

        m_myTaskItem->deleteLater();
        m_engine->deleteLater();

        m_myTaskItem = NULL;
        m_engine = NULL;

        m_clientModel->removeRows(0, m_clientModel->rowCount());
        m_msgModel->removeRows(0, m_msgModel->rowCount());

        qDebug()<<"the server is closing now !!!";
    }

}

void MainWindow::clientConnectSlot(QObject *client)
{
    m_ClientRecieved[client] =0;
    m_ClientSended[client] =0;
}

void MainWindow::clientDisConnectSlot(QObject *client)
{
    m_ClientRecieved.remove(client);
    m_ClientSended.remove(client);
}

void MainWindow::clientDataReceivedSlot(QObject *client, const QByteArray &data)
{   //qDebug()<<"received data: "<<data;
    m_ClientRecieved[client] += data.size();
}

void MainWindow::clientDataSendSlot(QObject *client, qint64 bytes)
{
    m_ClientSended[client] += bytes;
}

void MainWindow::on_killBtn_clicked()
{
    if (m_tcpSer) {
        m_tcpSer->KickAllClients();
    }
}
