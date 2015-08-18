#pragma once

#include <QObject>

class RiaCameraCommand : public QObject
{
    Q_OBJECT

public:
    explicit RiaCameraCommand(QObject *parent = 0);

    void execute();

    friend QDataStream& operator<<(QDataStream& ds, const RiaCameraCommand& obj);
    friend QDataStream& operator>>(QDataStream& ds, RiaCameraCommand& obj);
};

