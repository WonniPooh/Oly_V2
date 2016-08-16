#include <QApplication>
#include "olyserver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    OlyServer m_server(2323);

    m_server.show();

    return a.exec();
}
