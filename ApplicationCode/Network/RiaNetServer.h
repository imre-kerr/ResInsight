#pragma once

#include <QObject>
#include <QtCore>
#include <QtNetwork>

class RiaNetServer : public QObject
{
    Q_OBJECT

public:
    RiaNetServer(QObject* parent = 0, unsigned short serverPort = 3310);

    void                setServerPort(unsigned short serverPort);

signals:
    void                dataReceived(QByteArray);

private slots:
    void                slotNewConnection();
    void                slotDisconnected();
    void                slotReadyRead();
    void                slotDataRecieved(QByteArray);

private:
    QTcpServer*         m_tcpServer;

    QHash<QTcpSocket*, QByteArray*> m_buffers; //We need a buffer to store data until block has completely received
    QHash<QTcpSocket*, qint32*> m_sizes; //We need to store the size to verify if a block has received completely

};
