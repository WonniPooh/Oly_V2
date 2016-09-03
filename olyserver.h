#ifndef OLYSERVER_H
#define OLYSERVER_H
#pragma once

#include <QObject>
#include <QtNetwork>
#include <QMap>

class OlyServerWidget;
class ClientNames;
class RoutingTable;
class ClientConnection;
class QTextEdit;

class OlyServer : public QTcpServer
{
    Q_OBJECT
public slots:
    void slotThreadFinished();
    void slotClientDisconnected(quint16 client_id);
    void slot_new_connection(quint16 client_id);

public:
    OlyServer(QTextEdit *server_status_field, int m_port, ClientNames* names, QObject * parent = 0);
    ~OlyServer();
    ClientConnection* getConnection(quint16 client_id);

protected:
    void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;

private:
    OlyServerWidget* m_parent;
    RoutingTable* m_table;
    ClientNames* m_names;
    QTextEdit* m_edit;
    QVector<QThread*> clients_online;
    QMap <quint16, ClientConnection*> connected_clients;
};

#endif // OLYSERVER_H
