#pragma once

#include <QObject>

class RiaAnimationCommand : public QObject
{
    Q_OBJECT

public:
    explicit RiaAnimationCommand(QObject *parent = 0);

    void execute();

    friend QDataStream& operator<<(QDataStream& ds, const RiaAnimationCommand& obj);
    friend QDataStream& operator>>(QDataStream& ds, RiaAnimationCommand& obj);
};

