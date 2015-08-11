/////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2011-     Statoil ASA
//  Copyright (C) 2013-     Ceetron Solutions AS
//  Copyright (C) 2011-2012 Ceetron AS
//
//  ResInsight is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  ResInsight is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.
//
//  See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html>
//  for more details.
//
/////////////////////////////////////////////////////////////////////////////////

#include "RiaNetServer.h"

#include <QtNetwork>

#include "RiuMainWindow.h"

//--------------------------------------------------------------------------------------------------
///
//--------------------------------------------------------------------------------------------------
RiaNetServer::RiaNetServer(QObject* parent, unsigned short serverPort)
: QObject(parent),
  m_tcpServer(NULL),
  m_currentClient(NULL),
  m_serverPort(serverPort)
{
    m_errorMessageDialog = new QErrorMessage(RiuMainWindow::instance());

    m_tcpServer = new QTcpServer(this);

    if (!m_tcpServer->listen(QHostAddress::LocalHost, m_serverPort)) {
        m_errorMessageDialog->showMessage("Couldn't start network server.\n"
                                          "Please close all other ResInsight processes and try again.\n"
                                          "\n"
                                          + tr("The error given was: %1.").arg(m_tcpServer->errorString()));
        return;
    }
}

//--------------------------------------------------------------------------------------------------
///
//--------------------------------------------------------------------------------------------------
RiaNetServer::~RiaNetServer()
{
    // Make sure all commands are done
}

//--------------------------------------------------------------------------------------------------
///
//--------------------------------------------------------------------------------------------------
unsigned short RiaNetServer::serverPort()
{
    if (m_tcpServer) return m_tcpServer->serverPort();
    else return 0;
}
