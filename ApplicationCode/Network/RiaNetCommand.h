#pragma once

#include <QObject>
#include <QDataStream>
#include "RiaAnimationCommand.h"
#include "RiaCameraCommand.h"

namespace RiaNetCommand
{
    enum class CommandType
    {
        Camera,
        Animation
    };

}

QDataStream& operator<<(QDataStream& ds, const RiaNetCommand::CommandType& obj);
QDataStream& operator>>(QDataStream& ds, RiaNetCommand::CommandType& obj);
