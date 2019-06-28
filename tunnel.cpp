#include "tunnel.h"
#include <QHostAddress>

Tunnel::Tunnel(QObject *parent, QTcpSocket *source, const QHostAddress &destinationAddress, int destinationPort) : QObject(parent)
{
    this->source = source;
    source->setParent(this);
    destination = new QTcpSocket(this);
    totalTransferred = 0;

    // If one of the socket is disconnected, delete the tunnel
    // (the sockets are children so they will get deleted too).
    connect(source, SIGNAL(disconnected()), SLOT(deleteLater()));
    connect(destination, SIGNAL(disconnected()), SLOT(deleteLater()));

    // Connect the signals when something new is available for reading.
    connect(source, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(destination, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    // When bytes have been written, we want to check if there are any more bytes and send them as well.
    connect(source, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten()));
    connect(destination, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten()));

    // Connect the destination.
    destination->connectToHost(destinationAddress, destinationPort);
}

Tunnel::~Tunnel()
{
    qDebug(QString("Deleting tunnel from %1:%2 to %3:%4, data transferred: %5.").arg(source->peerAddress().toString()).arg(source->peerPort()).arg(destination->peerName()).arg(destination->peerPort()).arg(totalTransferred).toUtf8().constData());
}

void Tunnel::onReadyRead()
{
    QTcpSocket *source = (QTcpSocket*) sender();
    QTcpSocket *destination = getPeer(source);
    if (destination) {
        transferBlock(source, destination);
    }
}

void Tunnel::onBytesWritten()
{
    QTcpSocket *destination = (QTcpSocket*) sender();
    QTcpSocket *source = getPeer(destination);
    if (source) {
        transferBlock(source, destination);
    }
}

void Tunnel::transferBlock(QTcpSocket *source, QTcpSocket *destination)
{
    // Transfer up to 64KB. We don't want to overwhelm the buffers for the destination device.
    char buffer[64*1024];
    int n = source->read(buffer, 64*1024);
    if (n > 0) {
        destination->write(buffer, n);
    }
    totalTransferred += n;
}

QTcpSocket *Tunnel::getPeer(QTcpSocket *socket)
{
    return socket == source ? destination : source;
}
