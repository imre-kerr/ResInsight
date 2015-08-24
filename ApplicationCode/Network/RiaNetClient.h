#pragma once

#include <QtCore>
#include <QtNetwork>
#include "cvfObject.h"
#include "cvfCamera.h"
#include "cvfMatrix4.h"

class RiaNetClient : public QObject
{
    Q_OBJECT
public:
    RiaNetClient(QObject *parent = 0);
    ~RiaNetClient();

public slots:
    bool slotConnect(std::vector<QString> hosts);
    bool slotWriteData(QByteArray data);

    void slotMatrixChanged(cvf::Camera::MatrixType, const cvf::Mat4d&);

private:
    std::vector<QTcpSocket*>    m_sockets;

    void disconnect();
};
