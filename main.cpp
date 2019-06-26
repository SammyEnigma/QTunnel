#include "server.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (argc == 4) {
        new Server(&a, QHostAddress::Any, QString(argv[1]).toInt(), QHostAddress(argv[2]), QString(argv[3]).toInt());
    } else if (argc == 5) {
        new Server(&a, QHostAddress(argv[1]), QString(argv[2]).toInt(), QHostAddress(argv[3]), QString(argv[4]).toInt());
    } else {
        qDebug("Needs 3 or 4 parameters.");
        qDebug("Usage: qtunnel [<source address>] <source port> <destination address> <destination port>");
        qDebug("Example usage: qtunnel 0.0.0.0 81 127.0.0.1 82");
        qDebug("Example usage: qtunnel 81 127.0.0.1 82");
        return 1;
    }

    return a.exec();
}
