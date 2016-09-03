#ifndef ROUTINGTABLE_H
#define ROUTINGTABLE_H
#pragma once

#include <QWidget>
#include <QMap>
#include "clientnames.h"

class OlyServer;

class RoutingTable
{
public:
    RoutingTable(OlyServer* server_ptr);
    ~RoutingTable();

    void processNewConnection(quint16 client_id, ClientNames* names);

    const QVector<quint16>* getConnectedClients();
    const QVector<quint16>* getClientRoutes(quint16 client_id);

    void addNewClient(quint16 client_id, QVector<quint16>* routes);
    void setNewRoutes(quint16 client_id, QVector<quint16>* routes);

    void setConnects(quint16 client_id, QVector<quint16>* routes);
    void setConnects(quint16 client_id, quint16 reciever_id);

    void setDisconnects(quint16 client_id, QVector<quint16>* old_routes);
    void setDisconnects(quint16 client_id, quint16 reciever_id);

    bool addNewRoute(quint16 client_id, quint16 reciever_id);
    bool deleteClientRoute(quint16 client_id, quint16 reciever_id);
    bool deleteClient(quint16 client_id);

private:
    OlyServer* current_server;
    QVector <quint16> connected_clients;
    QMap <quint16, QVector<quint16>*> routing_table;
};

#endif // ROUTINGTABLE_H
