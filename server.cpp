#include "server.h"

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
    QTcpSocket *source = server->nextPendingConnection();
    QTcpSocket *destination = new QTcpSocket(this);

    // Use a "peer" property so the source/destination can find each other.
    source->setProperty("peer", QVariant::fromValue(QPointer<QTcpSocket>(destination)));
    destination->setProperty("peer", QVariant::fromValue(QPointer<QTcpSocket>(source)));

    // Connect the destination.
    destination->connectToHost(destinationAddress, destinationPort);

    // If one of the socket is disconnected, delete both.
    connect(source, SIGNAL(disconnected()), source, SLOT(deleteLater()));
    connect(source, SIGNAL(disconnected()), destination, SLOT(deleteLater()));
    connect(destination, SIGNAL(disconnected()), source, SLOT(deleteLater()));
    connect(destination, SIGNAL(disconnected()), destination, SLOT(deleteLater()));

    // Connect the signals when something new is available for reading.
    connect(source, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(destination, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    // When bytes have been written, we want to check if there are any more bytes and send them as well.
    connect(source, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten()));
    connect(destination, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten()));
}

void Server::onReadyRead()
{
    QTcpSocket *source = (QTcpSocket*) sender();
    QTcpSocket *destination = (QTcpSocket*) source->property("peer").value<QPointer<QTcpSocket> >();
    if (destination) {
        transferBlock(source, destination);
    }
}

void Server::onBytesWritten()
{
    QTcpSocket *destination = (QTcpSocket*) sender();
    QTcpSocket *source = (QTcpSocket*) destination->property("peer").value<QPointer<QTcpSocket> >();
    if (source) {
        transferBlock(source, destination);
    }
}

void Server::transferBlock(QTcpSocket *source, QTcpSocket *destination)
{
    // Transfer up to 64KB. We don't want to overwhelm the buffers for the destination device.
    char buffer[64*1024];
    int n = source->read(buffer, 64*1024);
    if (n > 0) {
        destination->write(buffer, n);
    }
}
