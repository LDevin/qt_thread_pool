#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    isConnt = false;

    ui->msgView->setModel(&model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == m_timerId) {
        int nTotalClients = ui->clientNumEdit->text().isEmpty() ? 10 : ui->clientNumEdit->text().toInt();

        QList<QGHTcpClient*> list = m_clientList.keys();
        foreach (QGHTcpClient *client, list) {
            if (rand() % 10 < 3) {
                client->sendData(QByteArray(qrand()%1024+1024,qrand()%(128-32)+32));
            }
        }

        if (rand() % 10 < 1) {

            if (m_clientList.size() > nTotalClients) {
                int nDel = m_clientList.size() > nTotalClients;

                QList<QGHTcpClient*> list = m_clientList.keys();
                for (int i = 0; i < nDel; ++i) {
                    list[i]->disconnectFromHost();
                }
            }

            QGHTcpClient *ghclient = new QGHTcpClient(this, ui->payLoadEdit->text().toInt());
            ghclient->connectToHost(ui->ipEdit->text(), ui->portEdit->text().toShort());

            m_clientList[ghclient] = QDateTime::currentDateTime();

            connect(ghclient, &QGHTcpClient::readyRead, this, &MainWindow::new_data_received);
            connect(ghclient, &QGHTcpClient::connected, this, &MainWindow::new_client_connected);
            connect(ghclient, &QGHTcpClient::disconnected, this, &MainWindow::new_client_disconnected);
            //connect(ghclient, &QGHTcpClient::error, this, &MainWindow::showSocketError);
            connect(ghclient, &QGHTcpClient::bytesWritten, this, &MainWindow::byteWritten);
            connect(ghclient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(showSocketError(QAbstractSocket::SocketError)));
        }
    }
}

void MainWindow::on_connectBtn_clicked()
{
    isConnt = !isConnt;
    if (isConnt) {
        m_timerId = startTimer(500);
    } else {
        killTimer(m_timerId);
    }
}

void MainWindow::displayMsg(const QString &msg)
{
    model.insertRow(0, new QStandardItem(msg));
    while (model.rowCount() >= 200) {
        model.removeRow(model.rowCount() - 1);
    }
}

void MainWindow::new_client_connected()
{
    QGHTcpClient *socket = qobject_cast<QGHTcpClient*>(sender());

     if (socket) {
         displayMsg(tr("client %1 connected!! ").arg((quintptr)socket));
         socket->sendData(QByteArray(qrand()%1024+1024,qrand()%(128-32)+32));
     }
}

void MainWindow::new_client_disconnected()
{
    QGHTcpClient *ghclient = qobject_cast<QGHTcpClient*>(sender());

     if (ghclient) {
         displayMsg(tr("client %1 disconnected!! ").arg((quintptr)ghclient));

         disconnect(ghclient, &QGHTcpClient::readyRead, this, &MainWindow::new_data_received);
         disconnect(ghclient, &QGHTcpClient::connected, this, &MainWindow::new_client_connected);
         disconnect(ghclient, &QGHTcpClient::disconnected, this, &MainWindow::new_client_disconnected);
         //connect(ghclient, &QGHTcpClient::error, this, &MainWindow::showSocketError);
         disconnect(ghclient, &QGHTcpClient::bytesWritten, this, &MainWindow::byteWritten);
         disconnect(ghclient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(showSocketError(QAbstractSocket::SocketError)));

         m_clientList.remove(ghclient);
         ghclient->deleteLater();
     }
}

void MainWindow::new_data_received()
{
    QGHTcpClient *socket = qobject_cast<QGHTcpClient*>(sender());

    if (socket) {
        QByteArray data = socket->readAll();
        displayMsg(tr("client %1 recevied msg %2 bytes").arg((quintptr)socket).arg(data.size()));
    }
}

void MainWindow::showSocketError(QAbstractSocket::SocketError err)
{
    qDebug()<<"socket error: "<<err;
}

void MainWindow::byteWritten(qint64 bytes)
{
    QGHTcpClient * pSock = qobject_cast<QGHTcpClient*>(sender());
    if (pSock)
    {
        displayMsg(QString("client %1 Transferrd %2 bytes.").arg((quintptr)pSock).arg(bytes));
    }
}

void MainWindow::on_pushButton_clicked()
{
    isConnt=  false;
    killTimer(m_timerId);

    QList<QGHTcpClient*> lsit = m_clientList.keys();
    foreach (QGHTcpClient *client, lsit) {
        client->disconnectFromHost();
        client->deleteLater();
    }

    model.removeRows(0, model.rowCount());

}
