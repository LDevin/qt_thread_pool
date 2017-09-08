#include "qghtcpserver.h"
#include <QDebug>


QGHTcpServer::QGHTcpServer(QObject *parent, int nPayLoad)
    :QTcpServer(parent), m_payLoad(nPayLoad)
{
    connect(this, &QGHTcpServer::newConnection, this, &QGHTcpServer::new_client_connected);
}

QGHTcpServer::~QGHTcpServer()
{

}

void QGHTcpServer::new_client_connected()
{   //qDebug()<<"1";
    QTcpSocket *client = nextPendingConnection();
    //QGHTcpClient *client = qobject_cast<QGHTcpClient*>(socket);
    if (client) {
    //qDebug()<<"2";
        connect(client, &QTcpSocket::readyRead, this, &QGHTcpServer::new_data_received);
        connect(client, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
        connect(client, &QTcpSocket::disconnected, this, &QGHTcpServer::client_closed);
        connect(client, &QTcpSocket::bytesWritten, this, &QGHTcpServer::send_some_data);

        m_clients[client] = 0;
        emit sig_new_connect(client);
        //client = nextPendingConnection();
    }
}

void QGHTcpServer::new_data_received()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        QByteArray arr = client->readAll();
        //qDebug()<<"new_data_received: "<< arr;
        emit sig_new_data_received(client, arr);
    }
}

void QGHTcpServer::displayError(QAbstractSocket::SocketError socketError)
{
   // Q_UNUSED(socketError)
   qDebug()<<"msg: "<<socketError;
}

void QGHTcpServer::client_closed()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        emit sig_new_disconnect(client);
        m_buf_offset.remove(client);
        m_buf_sending.remove(client);
        m_clients.remove(client);
        client->deleteLater();
    }
}

void QGHTcpServer::send_some_data(qint64 data)
{qDebug()<<"send some data";
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        emit sig_new_data_write(client, data);

        QList<QByteArray> &list_send_data = m_buf_sending[client];
        QList<qint64> &list_send_offset = m_buf_offset[client];

        while (!list_send_data.empty()) {

            QByteArray &arraySending = *list_send_data.begin();
            qint64 &currentOffset = *list_send_offset.begin();

            qint64 nTotalBytes = arraySending.size();

            qint64 writtenBytes = client->write(arraySending.constBegin() + currentOffset,
                                                qMin((int)(nTotalBytes - currentOffset), m_payLoad));

            currentOffset += writtenBytes;

            if (currentOffset >= nTotalBytes) {
                list_send_data.pop_front();
                list_send_offset.pop_front();
            } else {
                break;
            }
        }
    }
}

void QGHTcpServer::sendDataToClient(QObject *client,  QByteArray &data)
{
    qDebug()<<"sendDataToClient";
    if (m_clients.find(client) == m_clients.end())
        return;

    QTcpSocket *_client = qobject_cast<QTcpSocket*>(client);
    if (_client && data.size()) {
        QList<QByteArray> &list_send_data = m_buf_sending[_client];
        QList<qint64> &list_send_offset = m_buf_offset[_client];

        if (list_send_data.empty()) {
            qint64 byteWritten = _client->write(data.constData(), qMin(data.size(), m_payLoad));
            if (byteWritten < data.size()) {
                list_send_data.push_back(data);
                list_send_offset.push_back(byteWritten);
            }
        } else {
            list_send_data.push_back(data);
            list_send_offset.push_back(0);
        }
    }
}

void QGHTcpServer::KickAllClients()
{
    QList<QObject*> list = m_clients.keys();
    foreach (QObject *obj, list) {
        QTcpSocket *client = qobject_cast<QTcpSocket*>(obj);
        client->disconnectFromHost();
    }
}
