#include "olyserver.h"
#include "routingtable.h"
#include "clientconnection.h"
#include <assert.h>
#include <QObject>

RoutingTable::RoutingTable(OlyServer *server_ptr)
{
    assert(server_ptr);
    current_server = server_ptr;
}

void RoutingTable::setConnects(quint16 client_id, QVector<quint16>* routes)
{
    ClientConnection* sender = current_server->getConnection(client_id);

    if(sender)
    {
        ClientConnection* reciever = nullptr;

        for(int i = 0; i < routes->length(); i++)
        {
            reciever = current_server->getConnection(routes[0][i]);

            if(reciever)
                QObject::connect(sender, &ClientConnection::sendData, reciever, &ClientConnection::slotSendData);

        }
    }

}

void RoutingTable::setConnects(quint16 client_id, quint16 reciever_id)
{
    ClientConnection* sender = current_server->getConnection(client_id);
    ClientConnection* reciever = current_server->getConnection(reciever_id);

    if(sender && reciever)
        QObject::connect(sender, &ClientConnection::sendData, reciever, &ClientConnection::slotSendData);
}

void RoutingTable::setDisconnects(quint16 client_id, QVector<quint16>* old_routes)
{
    ClientConnection* sender = current_server->getConnection(client_id);

    if(sender)
    {
        ClientConnection* reciever = nullptr;

        for(int i = 0; i < old_routes->length(); i++)
        {
            reciever = current_server->getConnection(old_routes[0][i]);

            if(reciever)
                QObject::disconnect(sender, &ClientConnection::sendData, reciever, &ClientConnection::slotSendData);

        }
    }
}

void RoutingTable::setDisconnects(quint16 client_id, quint16 reciever_id)
{
    ClientConnection* sender = current_server->getConnection(client_id);
    ClientConnection* reciever = current_server->getConnection(reciever_id);

    if(sender && reciever)
        QObject::disconnect(sender, &ClientConnection::sendData, reciever, &ClientConnection::slotSendData);

}

bool RoutingTable::addNewRoute(quint16 client_id, quint16 reciever_id)
{
    QVector<quint16>* routes = routing_table.value(client_id);

    if(routes)
    {
        setConnects(client_id, reciever_id);
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

    setConnects(client_id, routes);

    routing_table.insert(client_id, routes);
}

void RoutingTable::setNewRoutes(quint16 client_id, QVector<quint16>* routes)
{
    if(connected_clients.indexOf(client_id) == -1)
        connected_clients.push_back(client_id);

    setDisconnects(client_id, routing_table.value(client_id));
    setConnects(client_id, routes);

    delete routing_table.value(client_id);
    routing_table.remove(client_id);
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

        setDisconnects(client_id, reciever_id);

        return true;
    }
    else
        return false;
}

bool RoutingTable::deleteClient(quint16 client_id)
{
    connected_clients.removeOne(client_id);
    delete routing_table.value(client_id);
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
