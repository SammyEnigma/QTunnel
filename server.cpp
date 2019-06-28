#include "server.h"
#include "tunnel.h"

#include <QPointer>
#include <QTcpSocket>

Server::Server(QObject *parent, const QHostAddress &sourceAddress, int sourcePort, const QHostAddress &destinationAddress, int destinationPort) : QObject(parent)
{
    this->destinationAddress = destinationAddress;
    this->destinationPort = destinationPort;

    // Start listening for connections.
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    if(!server->listen(sourceAddress, sourcePort))
    {
        qDebug() << "Server could not start";
    }
}

void Server::onNewConnection()
{
    new Tunnel(this, server->nextPendingConnection(), destinationAddress, destinationPort);
}
