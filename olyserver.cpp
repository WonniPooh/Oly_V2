#include "olyserver.h"

#include "clientconnection.h"
#include "routingtable.h"
#include "clientnames.h"
#include "olyserverwidget.h"

#include <QTextEdit>
#include <QThread>
#include <QMessageBox>

OlyServer::OlyServer(QTextEdit* server_status_field, int m_port, ClientNames *names, OlyServerWidget* class_parent, QObject *parent):QTcpServer(parent)
{
    m_parent = class_parent;
    m_table = new RoutingTable(this);
    m_edit = server_status_field;
    m_names = names;

    connect(m_parent, &OlyServerWidget::nameSet, this, &OlyServer::slot_new_connection);

    if(!QTcpServer::listen(QHostAddress::Any,  m_port))
    {
        QMessageBox::critical(0, "Server Error", "Unable to start the server:" + this -> errorString());
        QTcpServer::close();
        return;
    }
}

OlyServer::~OlyServer()
{
    for(int i = 0; i < clients_online.length(); i++)
    {
        clients_online[i]->quit();
        clients_online[i]->wait();
    }

    delete m_table;
}

void OlyServer::slot_new_connection(quint16 client_id, ClientConnection* sender)
{
    m_edit->append("Client \""+ m_names->getClientName(client_id) + "\" with ID" + QString::number(client_id)  + " connected.\n");
    connected_clients.insert(client_id, sender);
    m_table->processNewConnection(client_id, m_names);
}

void OlyServer::incomingConnection(qintptr socketDescriptor)
{
    QThread* new_connection = new QThread;
    ClientConnection* new_client_connection = new ClientConnection(socketDescriptor);
    new_client_connection->moveToThread(new_connection);

    connect(new_connection, &QThread::finished, this, &OlyServer::slotThreadFinished);
    connect(new_connection, &QThread::finished, new_client_connection, &ClientConnection::deleteLater);
    connect(new_connection, &QThread::started, new_client_connection, &ClientConnection::slotSocketStart);

    connect(new_client_connection, &ClientConnection::sendData, m_parent, &OlyServerWidget::newMsg);
    connect(new_client_connection, &ClientConnection::destroyed, this, &OlyServer::slotClientDisconnected);
    connect(new_client_connection, &ClientConnection::destroyed, m_parent, &OlyServerWidget::slotClientDisconnected);
    connect(new_client_connection, &ClientConnection::newConnection, m_parent, &OlyServerWidget::slotClientConnected);

    clients_online.push_back(new_connection);
    new_connection->start();
}

ClientConnection* OlyServer::getConnection(quint16 client_id)
{
    return connected_clients.value(client_id);
}

void OlyServer::slotThreadFinished()
{
    int arr_length = clients_online.length();

    for(int i = 0; i < arr_length; i++)
    {
        if(clients_online[i]->isFinished())
        {
            delete clients_online[i];
            clients_online.remove(i);
            break;
        }
    }
}

void OlyServer::slotClientDisconnected(quint16 client_id)
{
    connected_clients.remove(client_id);
    m_edit->append("Client with ID " + QString::number(client_id)  + " disconnected.\n");
}
