#include "RiaAnimationCommand.h"

RiaAnimationCommand::RiaAnimationCommand(QObject *parent)
    : QObject(parent)
{

}

void RiaAnimationCommand::execute()
{

}

QDataStream& operator<<(QDataStream& ds, const RiaAnimationCommand& obj)
{
    return ds;
}

QDataStream& operator>>(QDataStream& ds, RiaAnimationCommand& obj)
{
    return ds;
}
