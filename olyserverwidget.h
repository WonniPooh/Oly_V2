#ifndef OLYSERVERWIDGET_H
#define OLYSERVERWIDGET_H
#pragma once

#include <QObject>
#include <QWidget>
#include <QtNetwork>
#include <QMap>

class ClientNames;
class RoutingTable;
class ClientConnection;
class QTextEdit;
class MyTcpServer;
class QGridLayout;
class OlyServer;

class OlyServerWidget : public QWidget
{
public:
    OlyServerWidget(int m_port);
    ~OlyServerWidget();

private:
    int grid_row_pos;
    int grid_column_pos;

    OlyServer* m_server;
    QGridLayout* m_layout;
    QTextEdit* m_edit;
};

#endif // OLYSERVERWIDGET_H
