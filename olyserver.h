#ifndef OLYSERVER_H
#define OLYSERVER_H
#pragma once

#include <QObject>
#include <QtNetwork>
#include <QMap>
#include "clientconnection.h"

class OlyServerWidget;
class ClientNames;
class QTextEdit;

class OlyServer : public QTcpServer
{
    Q_OBJECT
public slots:
    void slotThreadFinished();
    void slotClientDisconnected(quint16 client_id);
    void slotNewConnection(quint16 client_id, ClientConnection* sender);

public:
    OlyServer(QTextEdit *server_status_field, int m_port, ClientNames* names, OlyServerWidget* class_parent, QObject * parent = 0);
    ~OlyServer();

protected:
    void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;

private:
    int event_type;
    OlyServerWidget* m_parent;
    ClientNames* m_names;
    QTextEdit* m_edit;
    QVector<QThread*> clients_online;
    QMutex map_access;
    QMap <quint16, ConnectionData*> connected_clients;
};

#endif // OLYSERVER_H
