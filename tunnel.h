#ifndef TUNNEL_H
#define TUNNEL_H

#include <QObject>
#include <QTcpSocket>

class Tunnel : public QObject
{
    Q_OBJECT
public:
    explicit Tunnel(QObject *parent, QTcpSocket *source, const QHostAddress &destinationAddress, int destinationPort);
    virtual ~Tunnel();

signals:

private slots:
    void onReadyRead();
    void onBytesWritten();

private:
    QTcpSocket *source;
    QTcpSocket *destination;

    void transferBlock(QTcpSocket *source, QTcpSocket *destination);
    QTcpSocket *getPeer(QTcpSocket *socket);
    quint64 totalTransferred;
};

#endif // TUNNEL_H
