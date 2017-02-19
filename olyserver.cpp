#include "olyserver.h"

#include "clientconnection.h"
#include "clientnames.h"
#include "olyserverwidget.h"

#include <QTextEdit>
#include <QThread>
#include <QMessageBox>
#include <QDebug>

OlyServer::OlyServer(QTextEdit* server_status_field,
                     int m_port, ClientNames *names,
                     OlyServerWidget* class_parent,
                     QObject *parent):QTcpServer(parent)
{
    m_parent = class_parent;
    m_edit = server_status_field;                       //print last actions, such as "client with id@@ connected"
    m_names = names;

    event_type = QEvent::registerEventType();

    connect(m_parent, &OlyServerWidget::nameSet, this, &OlyServer::slotNewConnection);

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
}

//prints msg abount new client connected and
void OlyServer::slotNewConnection(quint16 client_id, ClientConnection* sender)
{
    m_edit->append("Client \""+ m_names->getClientName(client_id) + "\" with ID" + QString::number(client_id)  + " connected.\n");
    ConnectionData* new_connection = new ConnectionData;
    new_connection->connection = sender;
    new_connection->mutex = new QMutex;
    connected_clients.insert(client_id, new_connection);
}

void OlyServer::incomingConnection(qintptr socketDescriptor)
{
    QThread* new_connection = new QThread;
    ClientConnection* new_client_connection = new ClientConnection(socketDescriptor, event_type, &connected_clients, &map_access);
    new_client_connection->moveToThread(new_connection);

    connect(new_connection, &QThread::finished, this, &OlyServer::slotThreadFinished);
    connect(new_connection, &QThread::finished, new_client_connection, &ClientConnection::deleteLater);
    connect(new_connection, &QThread::started, new_client_connection, &ClientConnection::slotSocketStart);

    connect(new_client_connection, &ClientConnection::displayMsg, m_parent, &OlyServerWidget::newMsg);
    connect(new_client_connection, &ClientConnection::destroyed, this, &OlyServer::slotClientDisconnected);
    connect(new_client_connection, &ClientConnection::destroyed, m_parent, &OlyServerWidget::slotClientDisconnected);
    connect(new_client_connection, &ClientConnection::clientIDRecieved, m_parent, &OlyServerWidget::slotClientConnected);

    clients_online.push_back(new_connection);
    new_connection->start();
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
    map_access.lock();
    ConnectionData* data_to_delete = connected_clients.value(client_id);
    delete data_to_delete->mutex;
    delete data_to_delete;
    connected_clients.remove(client_id);
    map_access.unlock();
    m_edit->append("Client with ID " + QString::number(client_id)  + " disconnected.\n");
}
