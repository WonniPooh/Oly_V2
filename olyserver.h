#ifndef OLYSERVER_H
#define OLYSERVER_H
#pragma once

#include <QObject>
#include <QWidget>
#include <QtNetwork>
#include <QMap>

class ClientNames;
class RoutingTable;
class ClientConnection;
class QTextEdit;
class QVBoxLayout;
class QHBoxLayout;

class OlyServer : public QWidget
{
    Q_OBJECT
public slots:
    void slot_new_connection();
    void slot_read_first_msg();
    void slot_thread_finished();
    void slot_client_disconnected();

public:
    OlyServer(int m_port);
    ~OlyServer();
    QTcpSocket* get_connection(quint16 client_ID);

private:

    RoutingTable* m_table;
    ClientNames* m_names;
    QVector<QThread*> clients_online;
    QTcpServer* m_tcp_server;
    QMap <quint16, QTcpSocket*>connected_clients;

    QVBoxLayout* m_vbox;
    QHBoxLayout* m_hbox;
    QTextEdit* txt_edit;
    QTextEdit* m_edit;
};

#endif // OLYSERVER_H
