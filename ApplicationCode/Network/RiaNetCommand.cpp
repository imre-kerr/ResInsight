#include "RiaNetCommand.h"


QDataStream& operator<<(QDataStream& ds, const RiaNetCommand::CommandType& obj)
{
    return ds << static_cast<int>(obj);
}

QDataStream& operator>>(QDataStream& ds, RiaNetCommand::CommandType& obj)
{
    int t;
    ds >> t;
    obj = static_cast<RiaNetCommand::CommandType>(t);
    return ds;
}
