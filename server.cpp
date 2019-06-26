#include "server.h"

#include <QTcpSocket>

Server::Server(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    if(!server->listen(QHostAddress::Any, 1111))
    {
        qDebug() << "Server could not start";
    }
}

void Server::onNewConnection()
{
    QTcpSocket *source = server->nextPendingConnection();
    QTcpSocket *destination = new QTcpSocket(this);

    // Use a "peer" property so the source/destination can find each other.
    source->setProperty("peer", QVariant::fromValue((void*) destination));
    destination->setProperty("peer", QVariant::fromValue((void*) source));

    // Connect the destination.
    destination->connectToHost(QHostAddress::LocalHost, 2222);

    // If one of the socket is disconnected, delete both.
    connect(source, SIGNAL(disconnected()), source, SLOT(deleteLater()));
    connect(source, SIGNAL(disconnected()), destination, SLOT(deleteLater()));
    connect(destination, SIGNAL(disconnected()), source, SLOT(deleteLater()));
    connect(destination, SIGNAL(disconnected()), destination, SLOT(deleteLater()));

    connect(source, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(destination, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    connect(source, SIGNAL(bytesWritten(qint64)), this, SLOT(onReadyRead()));
    connect(destination, SIGNAL(bytesWritten(qint64)), this, SLOT(onReadyRead()));
}

void Server::onReadyRead()
{
    QTcpSocket *source = (QTcpSocket*) sender();
    QTcpSocket *destination = (QTcpSocket*) source->property("peer").value<void*>();

    char buffer[64*1024];
    int n = source->read(buffer, 64*1024);
    if (n > 0) {
        destination->write(buffer, n);
    }
}
