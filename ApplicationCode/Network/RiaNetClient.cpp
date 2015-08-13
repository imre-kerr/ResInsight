#include "RiaNetClient.h"

static inline QByteArray IntToArray(qint32 source);

RiaNetClient::RiaNetClient(QObject *parent)
: QObject(parent),
  m_sockets()
{
}

RiaNetClient::~RiaNetClient()
{
    for (std::vector<QTcpSocket*>::iterator it = m_sockets.begin(); it != m_sockets.end(); ++it)
    {
        delete *it;
    }
}

bool RiaNetClient::slotConnect(std::vector<QString> hosts)
{
    bool success = true;
    for (std::vector<QString>::iterator it = hosts.begin(); it != hosts.end(); ++it)
    {
        QUrl url(*it);
        QTcpSocket *socket = new QTcpSocket(this);
        socket->connectToHost(url.host(), url.port());
        success &= socket->waitForConnected();
        m_sockets.push_back(socket);
    }
    return success;
}

bool RiaNetClient::slotWriteData(QByteArray data)
{
    bool success = true;
    for (std::vector<QTcpSocket*>::iterator it = m_sockets.begin(); it != m_sockets.end(); ++it)
    {
        QTcpSocket* socket = *it;
        if (socket->state() == QAbstractSocket::ConnectedState)
        {
            socket->write(IntToArray(data.size())); //write size of data
            socket->write(data); //write the data itself
            success &= socket->waitForBytesWritten();
        }
        else
        {
            success = false;
        }
    }
    return success;
}

QByteArray IntToArray(qint32 source) //Use qint32 to ensure that the number have 4 bytes
{
    //Avoid use of cast, this is the Qt way to serialize objects
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}
