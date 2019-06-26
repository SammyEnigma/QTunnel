#include "server.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    new Server(&a);
    return a.exec();
}
