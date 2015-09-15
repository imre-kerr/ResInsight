#include "RiaAnimationCommand.h"
#include "RiaApplication.h"
#include "RimView.h"
#include "RiuViewer.h"
#include "cafFrameAnimationControl.h"

RiaAnimationCommand::RiaAnimationCommand(QObject *parent)
    : QObject(parent)
{

}

RiaAnimationCommand::RiaAnimationCommand(QObject *parent, int frameIndex)
    : QObject(parent),
      m_frameIndex(frameIndex)
{
}

void RiaAnimationCommand::execute()
{
    qDebug() << "Setting frame to " << m_frameIndex;
    RimView *activeReservoirView = RiaApplication::instance()->activeReservoirView();
    if (activeReservoirView && activeReservoirView->viewer())
    {
        activeReservoirView->viewer()->animationControl()->setCurrentFrame(m_frameIndex);
        activeReservoirView->viewer()->update();
    }
}

QDataStream& operator<<(QDataStream& ds, const RiaAnimationCommand& obj)
{
    return ds << obj.m_frameIndex;
}

QDataStream& operator>>(QDataStream& ds, RiaAnimationCommand& obj)
{
    return ds >> obj.m_frameIndex;
}
