#include "RiaNetClient.h"
#include "RiaCameraCommand.h"
#include "RiaApplication.h"
#include "RimView.h"
#include "RiuViewer.h"
#include "cvfCamera.h"
#include "cvfObject.h"
#include "cvfMatrix4.h"

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
    cvf::Camera *cam = RiaApplication::instance()->activeReservoirView()->viewer()->mainCamera();
    connect(cam, SIGNAL(matrixChanged(cvf::Camera::MatrixType, const cvf::Mat4d&)), this, SLOT(slotMatrixChanged(cvf::Camera::MatrixType, const cvf::Mat4d&)));
    bool success = true;
    for (std::vector<QString>::iterator it = hosts.begin(); it != hosts.end(); ++it)
    {
        QUrl url("http://" + *it); // QUrl constructor is garbage
        QTcpSocket *socket = new QTcpSocket(this);
        socket->connectToHost(url.host(), url.port());
        success &= socket->waitForConnected();
        if (success)
        {
            qDebug() << "Connected to: " << *it;
            m_sockets.push_back(socket);
        }
        else
        {
            qDebug() << "Couldn't connect to: " << url.host() << ":" << url.port();
            qDebug() << "Reason: " << socket->errorString();
            delete socket;
        }
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
            qDebug() << "Writing data to " << socket->peerName();
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

void RiaNetClient::slotMatrixChanged(cvf::Camera::MatrixType type, const cvf::Mat4d &mat)
{
    qDebug() << "Got matrix change! Sending via network...";
    RiaCameraCommand comm(0, type, mat);
    QByteArray data;
    QDataStream stream(&data, QIODevice::ReadWrite);
    stream << comm;

    if (!slotWriteData(data))
    {
        qDebug() << "Sending data failed!";
    }
    else
    {
        qDebug() << "Send successful!";
    }
}

QByteArray IntToArray(qint32 source) //Use qint32 to ensure that the number have 4 bytes
{
    //Avoid use of cast, this is the Qt way to serialize objects
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}
