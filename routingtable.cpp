#include "routingtable.h"

RoutingTable::RoutingTable()
{

}

bool RoutingTable::addNewRoute(quint16 client_id, quint16 reciever_id)
{
    QVector<quint16>* routes = routing_table.value(client_id);
    if(routes)
    {
        routes->push_back(reciever_id);
        return true;
    }
    else
        return false;
}

void RoutingTable::addNewClient(quint16 client_id, QVector<quint16>* routes)
{
    if(connected_clients.indexOf(client_id) == -1)
        connected_clients.push_back(client_id);
    routing_table.insert(client_id, routes);
}

void RoutingTable::setNewRoutes(quint16 client_id, QVector<quint16>* routes)
{
    if(connected_clients.indexOf(client_id) == -1)
        connected_clients.push_back(client_id);
    routing_table.remove(client_id);
    delete routing_table.value(client_id);
    routing_table.insert(client_id, routes);
}

bool RoutingTable::deleteClientRoute(quint16 client_id, quint16 reciever_id)
{
    QVector<quint16>* routes = routing_table.value(client_id);

    if(routes)
    {
        int pos = routes->indexOf(reciever_id);

        if(pos == -1)
            return false;

        routes->remove(pos);
        return true;
    }
    else
        return false;
}

bool RoutingTable::deleteClient(quint16 client_id)
{
    connected_clients.removeOne(client_id);
    return routing_table.remove(client_id);
}

const QVector<quint16>* RoutingTable::getClientRoutes(quint16 client_id)
{
    return routing_table.value(client_id);
}

const QVector<quint16>* RoutingTable::getConnectedClients()
{
    return &connected_clients;
}
