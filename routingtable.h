#ifndef ROUTINGTABLE_H
#define ROUTINGTABLE_H
#pragma once

#include <QWidget>
#include <QMap>

class RoutingTable
{
public:
    RoutingTable();
    const QVector<quint16>* getConnectedClients();
    const QVector<quint16>* getClientRoutes(quint16 client_id);
    bool addNewRoute(quint16 client_id, quint16 reciever_id);
    void addNewClient(quint16 client_id, QVector<quint16>* routes);
    void setNewRoutes(quint16 client_id, QVector<quint16>* routes);
    bool deleteClientRoute(quint16 client_id, quint16 reciever_id);
    bool deleteClient(quint16 client_id);

private:
    QVector <quint16> connected_clients;
    QMap <quint16, QVector<quint16>*> routing_table;
};

#endif // ROUTINGTABLE_H
