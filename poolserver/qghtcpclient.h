#ifndef QGHTCPCLIENT_H
#define QGHTCPCLIENT_H
#include <QObject>
#include <QTcpSocket>

class QGHTcpClient : public QTcpSocket
{
    Q_OBJECT
public:
    QGHTcpClient(QObject *parent = 0, int payLoad = 4096);
    ~QGHTcpClient();

private:
    int m_payLoad;
    QList<QByteArray> m_buffer_data;
    QList<qint64> m_buffer_offset;

public slots:
    void handleSendData(qint64);
    void sendData(QByteArray data);
};
#endif // QGHTCPCLIENT_H
