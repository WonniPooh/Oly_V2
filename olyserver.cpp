#include "olyserver.h"

#include "clientconnection.h"
#include "routingtable.h"
#include "clientnames.h"
#include "routingwidget.h"
#include "olyserverwidget.h"

#include <QTextEdit>
#include <QThread>
#include <QMessageBox>

OlyServer::OlyServer(QTextEdit* server_status_field, int m_port, QObject *parent):QTcpServer(parent)
{
    m_parent = (OlyServerWidget*)parent;
    m_table = new RoutingTable(this);
    m_names = new ClientNames;
    m_edit = server_status_field;

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
    delete m_names;
}

void OlyServer::slot_new_connection(quint16 client_id)
{
    m_edit->append("Client with ID " + QString::number(client_id)  + " connected.\n");
    connected_clients.insert(client_id, (ClientConnection*)sender());
    m_names->addNewClient(client_id, nullptr);
    RoutingWidget set_new_rotes(client_id, m_table, m_names);
    set_new_rotes.processNewConnection();
}


void OlyServer::incomingConnection(qintptr socketDescriptor)
{
    QThread* new_connection = new QThread;
    ClientConnection* new_client_connection = new ClientConnection(this, socketDescriptor);
    new_client_connection->moveToThread(new_connection);
    connect(new_connection, &QThread::finished, this, &OlyServer::slot_thread_finished);
    connect(new_connection, &QThread::finished, new_client_connection, &ClientConnection::deleteLater);
    connect(new_client_connection, &ClientConnection::destroyed, this, &OlyServer::slot_client_disconnected);
    connect(new_connection, &QThread::started, new_client_connection, &ClientConnection::slotSocketStart);
    clients_online.push_back(new_connection);
    new_connection->start();
}

ClientConnection* OlyServer::getConnection(quint16 client_id)
{
    return connected_clients.value(client_id);
}

void OlyServer::slot_thread_finished()
{
    int arr_length = clients_online.length();

    for(int i = 0; i < arr_length; i++)
    {
        if(clients_online[i]->isFinished())
        {
            clients_online.remove(i);
            break;
        }
    }
}

void OlyServer::slot_client_disconnected(quint16 client_id)
{
    connected_clients.remove(client_id);
    m_edit->append("Client with ID " + QString::number(client_id)  + " disconnected.\n");
}
