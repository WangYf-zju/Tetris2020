#include "Server.h"

Server::Server()
{
    pServer = new QWebSocketServer(QString("LocalHost"), 
        QWebSocketServer::NonSecureMode, this);
    connect(pServer, &QWebSocketServer::newConnection, this, [&]() { NewConnect(); });
}

bool Server::StartServer(quint16 port = 3000)
{
    return pServer->listen(QHostAddress::LocalHost, port);
}

void Server::NewConnect()
{
    if (pClientConnection)
    {
        pClientConnection->close();
    }
    pClientConnection = pServer->nextPendingConnection();
}

void Server::SendData(QString msg)
{
    if (pClientConnection)
        pClientConnection->sendTextMessage(msg);
}
