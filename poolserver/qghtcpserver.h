#ifndef QGHTCPSERVER_H
#define QGHTCPSERVER_H
#include <QTcpServer>
#include <QMap>
#include "qghtcpclient.h"


class QGHTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    QGHTcpServer(QObject *parent, int nPayLoad = 4096);
    ~QGHTcpServer();

    void KickAllClients();

    QList<QObject*> clients() const {return m_clients.keys();}

    void setPayLoad(int payLoad) {m_payLoad = payLoad;}

private:
    int m_payLoad;

    QMap<QObject*, QList<QByteArray> > m_buf_sending;
    QMap<QObject*, QList<qint64> >m_buf_offset;
    QMap<QObject*, int> m_clients;

public slots:
    void new_client_connected();
    void new_data_received();

    void client_closed();
    void send_some_data(qint64 data);

    void displayError(QAbstractSocket::SocketError socketError);

    void sendDataToClient(QObject *client, QByteArray &data);

signals:
    void sig_new_connect(QObject *client);
    void sig_new_disconnect(QObject *client);

    void sig_new_data_received(QObject *client, const QByteArray&);
    void sig_new_data_write(QObject *cliect, qint64);

    void sig_sock_connect_error(QObject *client, QAbstractSocket::SocketError);

};
#endif // QGHTCPSERVER_H
