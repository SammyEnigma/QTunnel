#include "server.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    new Server(&a, QHostAddress::Any, 1111, QHostAddress::LocalHost, 2222);
    return a.exec();
}
