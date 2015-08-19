#pragma once

#include <QObject>
#include "cvfObject.h"
#include "cvfMatrix4.h"

class RiaCameraCommand : public QObject
{
    Q_OBJECT

public:
    explicit RiaCameraCommand(QObject *parent = 0);

    void execute();

    friend QDataStream& operator<<(QDataStream& ds, const RiaCameraCommand& obj);
    friend QDataStream& operator>>(QDataStream& ds, RiaCameraCommand& obj);

private:
    enum MatrixType {
        VIEW,
        PROJECTION
    };

    MatrixType      m_type;
    cvf::Mat4d      m_matrix;
};

