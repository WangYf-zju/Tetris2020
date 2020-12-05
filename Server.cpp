#include "Server.h"

Server::Server()
{
    pServer = new QWebSocketServer(QString("LocalHost"), 
        QWebSocketServer::NonSecureMode, this);
    connect(pServer, &QWebSocketServer::newConnection, this, [&]() { NewConnect(); });
}

bool Server::StartServer(quint16 port = 3000)
{
    return pServer->listen(QHostAddress("192.168.137.1"), port);
}

void Server::NewConnect()
{
    if (vClientConnection.length() > MAX_CONNECT)
    {
        pServer->nextPendingConnection()->close();
    }
    vClientConnection.push_back(pServer->nextPendingConnection());
}

void Server::SendData(QString msg)
{
    for (int i = 0; i < vClientConnection.length(); i++)
    {
        vClientConnection[i] && vClientConnection[i]->sendTextMessage(msg);
    }
}
