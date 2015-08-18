#include "RiaCameraCommand.h"

RiaCameraCommand::RiaCameraCommand(QObject *parent)
    : QObject(parent)
{

}

void RiaCameraCommand::execute()
{

}

QDataStream& operator<<(QDataStream& ds, const RiaCameraCommand& obj)
{
    return ds;
}

QDataStream& operator>>(QDataStream& ds, RiaCameraCommand& obj)
{
    return ds;
}
