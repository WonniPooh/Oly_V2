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
class MyTcpServer;
class QGridLayout;

class OlyServer : public QWidget, QTcpServer
{
    Q_OBJECT
public slots:
    void slot_thread_finished();
    void slot_client_disconnected();
    void slot_new_connection(quint16 client_id, QSharedPointer<QTextEdit> client_status_field);

public:
    OlyServer(int m_port);
    ~OlyServer();
    ClientConnection* getConnection(quint16 client_id);

protected:
    void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;

private:
    int grid_row_pos;
    int grid_column_pos;
    RoutingTable* m_table;
    ClientNames* m_names;
    QVector<QThread*> clients_online;
    QMap <quint16, ClientConnection*> connected_clients;

    QGridLayout* m_layout;
    QTextEdit* m_edit;
};

#endif // OLYSERVER_H
