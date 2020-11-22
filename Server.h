#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QTimer>

class Server: public QObject
{
    Q_OBJECT

public:
    Server();
    bool StartServer(quint16 port);
    void SendData(QString msg);

private:
    QWebSocketServer * pServer;
    QWebSocket * pClientConnection;
    void NewConnect();
};

#endif // SERVER_H
