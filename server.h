#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent, const QHostAddress &sourceAddress, int sourcePort, const QHostAddress &destinationAddress, int destinationPort);

signals:

private slots:
    void onNewConnection();
    void onReadyRead();
    void onBytesWritten();

private:
    QTcpServer *server;
    QHostAddress destinationAddress;
    int destinationPort;

    void transferBlock(QTcpSocket *source, QTcpSocket *destination);
};

#endif // SERVER_H
