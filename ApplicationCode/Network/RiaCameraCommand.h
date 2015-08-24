#pragma once

#include <QObject>
#include "cvfObject.h"
#include "cvfMatrix4.h"
#include "cvfCamera.h"

class RiaCameraCommand : public QObject
{
    Q_OBJECT

public:
    explicit RiaCameraCommand(QObject *parent = 0);
    RiaCameraCommand(QObject *parent, cvf::Camera::MatrixType type, cvf::Mat4d matrix);

    void execute();

    friend QDataStream& operator<<(QDataStream& ds, const RiaCameraCommand& obj);
    friend QDataStream& operator>>(QDataStream& ds, RiaCameraCommand& obj);

private:

    cvf::Camera::MatrixType      m_type;
    cvf::Mat4d                   m_matrix;
};

