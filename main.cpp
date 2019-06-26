#include "server.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (argc != 5) {
        qDebug("Needs 4 parameters.");
        qDebug("Usage: qtunnel <source address> <source port> <destination address> <destination port>");
        qDebug("Example usage: qtunnel 0.0.0.0 81 127.0.0.1 82");
        return 1;
    }

    new Server(&a, QHostAddress(argv[1]), QString(argv[2]).toInt(), QHostAddress(argv[3]), QString(argv[4]).toInt());
    return a.exec();
}
