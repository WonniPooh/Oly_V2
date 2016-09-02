#include <QApplication>
#include "olyserverwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    OlyServerWidget m_server(2323);

    m_server.show();

    return a.exec();
}
