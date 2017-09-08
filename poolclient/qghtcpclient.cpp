#include "qghtcpclient.h"
#include <assert.h>


QGHTcpClient::QGHTcpClient(QObject *parent, int payLoad)
    :QTcpSocket(parent), m_payLoad(payLoad)
{
    connect(this, &QGHTcpClient::bytesWritten, this, &QGHTcpClient::handleSendData);
}

QGHTcpClient::~QGHTcpClient()
{

}

void QGHTcpClient::handleSendData(qint64)
{
    while (!m_buffer_data.empty()) {
        QByteArray &tmp_data = *m_buffer_data.begin();
        qint64 &currentOffset = *m_buffer_offset.begin();

        qint64 totalBytes = tmp_data.size();

        assert(totalBytes >= currentOffset);

        qint64 byteWritten = write(tmp_data.constData() + currentOffset, qMin((int)(totalBytes - currentOffset), m_payLoad));

        currentOffset += byteWritten;

        if (currentOffset >= totalBytes) {
            m_buffer_data.pop_front();
            m_buffer_offset.pop_front();
        } else {
            break;
        }
    }
}

void QGHTcpClient::sendData(QByteArray data)
{
    if (data.size() < 1) return;

    if (m_buffer_data.isEmpty()) {

        qint64 byteWriten = write(data.constData(), qMin(data.size(), m_payLoad));

        if (byteWriten < data.size()) {
            m_buffer_data.push_back(data);
            m_buffer_offset.push_back(byteWriten);
        }
    } else {
        m_buffer_data.push_back(data);
        m_buffer_offset.push_back(0);
    }
}
