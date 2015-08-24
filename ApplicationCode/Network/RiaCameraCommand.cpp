#include "RiaCameraCommand.h"
#include "RiaApplication.h"
#include "RimView.h"
#include "RiuViewer.h"

RiaCameraCommand::RiaCameraCommand(QObject *parent)
    : QObject(parent)
{

}

RiaCameraCommand::RiaCameraCommand(QObject *parent, cvf::Camera::MatrixType type, cvf::Mat4d matrix)
    : QObject(parent),
      m_type(type),
      m_matrix(matrix)
{

}

void RiaCameraCommand::execute()
{
    RimView *activeReservoirView = RiaApplication::instance()->activeReservoirView();
    if (activeReservoirView && activeReservoirView->viewer())
    {
        if (m_type == cvf::Camera::PROJECTION)
        {
            activeReservoirView->viewer()->mainCamera()->setProjectionMatrix(m_matrix);
        }
        else
        {
            activeReservoirView->viewer()->mainCamera()->setViewMatrix(m_matrix);
        }
    }
}

QDataStream& operator<<(QDataStream& ds, const RiaCameraCommand& obj)
{
    return ds << static_cast<int>(obj.m_type) << obj.m_matrix;
}

QDataStream& operator>>(QDataStream& ds, RiaCameraCommand& obj)
{
    int t;
    ds >> t;
    obj.m_type = static_cast<cvf::Camera::MatrixType>(t);
    return ds >> obj.m_matrix;
}
