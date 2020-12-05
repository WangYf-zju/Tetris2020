#ifndef SERVER_H
#define SERVER_H

#include <QVector>
#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QTimer>

#define MAX_CONNECT 20

class Server: public QObject
{
    Q_OBJECT

public:
    Server();
    bool StartServer(quint16 port);
    void SendData(QString msg);

private:
    QWebSocketServer * pServer;
    QVector<QWebSocket*> vClientConnection;
    void NewConnect();
};

#endif // SERVER_H
