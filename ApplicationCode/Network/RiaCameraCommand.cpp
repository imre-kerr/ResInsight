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
    return ds << static_cast<int>(obj.m_type) << obj.m_matrix;
}

QDataStream& operator>>(QDataStream& ds, RiaCameraCommand& obj)
{
    int t;
    ds >> t;
    obj.m_type = static_cast<RiaCameraCommand::MatrixType>(t);
    return ds >> obj.m_matrix;
}
