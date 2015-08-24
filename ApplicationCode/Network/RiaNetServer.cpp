#include "RiaNetServer.h"
#include "RiaNetCommand.h"

static inline qint32 ArrayToInt(QByteArray source);

RiaNetServer::RiaNetServer(QObject *parent, unsigned short serverPort)
: QObject(parent)
{
    m_tcpServer = new QTcpServer(this);
    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
    connect(this, SIGNAL(dataReceived(QByteArray)), this, SLOT(slotDataRecieved(QByteArray)));
    qDebug() << "Listening:" << m_tcpServer->listen(QHostAddress::Any, serverPort);
}

void RiaNetServer::setServerPort(unsigned short serverPort)
{
    m_tcpServer->close();
    qDebug() << "Listening:" << m_tcpServer->listen(QHostAddress::Any, serverPort);
}

void RiaNetServer::slotNewConnection()
{
    while (m_tcpServer->hasPendingConnections())
    {
        QTcpSocket *socket = m_tcpServer->nextPendingConnection();
        connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
        QByteArray *buffer = new QByteArray();
        qint32 *s = new qint32(0);
        m_buffers.insert(socket, buffer);
        m_sizes.insert(socket, s);
    }
}

void RiaNetServer::slotDisconnected()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    QByteArray *buffer = m_buffers.value(socket);
    qint32 *s = m_sizes.value(socket);
    socket->deleteLater();
    delete buffer;
    delete s;
}

void RiaNetServer::slotReadyRead()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    QByteArray *buffer = m_buffers.value(socket);
    qint32 *s = m_sizes.value(socket);
    qint32 size = *s;
    while (socket->bytesAvailable() > 0)
    {
        buffer->append(socket->readAll());
        while ((size == 0 && buffer->size() >= 4) || (size > 0 && buffer->size() >= size)) //While can process data, process it
        {
            if (size == 0 && buffer->size() >= 4) //if size of data has received completely, then store it on our global variable
            {
                size = ArrayToInt(buffer->mid(0, 4));
                *s = size;
                buffer->remove(0, 4);
            }
            if (size > 0 && buffer->size() >= size) // If data has received completely, then emit our SIGNAL with the data
            {
                QByteArray data = buffer->mid(0, size);
                buffer->remove(0, size);
                size = 0;
                *s = size;
                emit dataReceived(data);
            }
        }
    }
}

void RiaNetServer::slotDataRecieved(QByteArray data)
{
    char buf[1024];
    QDataStream stream(&data, QIODevice::ReadOnly);
    int len = stream.readRawData(buf, 1024);
    qDebug() << "Received raw data:";
    for(int i = 0; i < len; ++i)
    {
        qDebug() << (quint8)(buf[i]);
    }
    RiaNetCommand::CommandType type;
    stream >> type;
    switch (type)
    {
    case RiaNetCommand::CommandType::Animation:
    {
        RiaAnimationCommand comm;
        stream >> comm;
        comm.execute();
        break;
    }
    case RiaNetCommand::CommandType::Camera:
    {
        RiaCameraCommand comm;
        stream >> comm;
        comm.execute();
        break;
    }
    }
}

qint32 ArrayToInt(QByteArray source)
{
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}
