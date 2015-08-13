#pragma once

#include <QtCore>
#include <QtNetwork>

class RiaNetClient : public QObject
{
    Q_OBJECT
public:
    RiaNetClient(QObject *parent = 0);
    ~RiaNetClient();

public slots:
    bool slotConnect(std::vector<QString> hosts);
    bool slotWriteData(QByteArray data);

private:
    std::vector<QTcpSocket*>    m_sockets;

    void disconnect();
};
