#include "olyserver.h"

#include "clientconnection.h"
#include "routingtable.h"
#include "clientnames.h"
#include "routingwidget.h"

#include <QMessageBox>
#include <QTextEdit>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include <QThread>

OlyServer::~OlyServer()
{
    for(int i = 0; i < clients_online.length(); i++)
    {
        clients_online[i]->quit();
        clients_online[i]->wait();
    }
    delete m_edit;
    delete m_table;
    delete m_names;
}

OlyServer::OlyServer(int m_port)
{
    grid_column_pos = 0;
    grid_row_pos = 0;

    QWidget* main_wdgt = new QWidget;
    QScrollArea* scroll = new QScrollArea;
    scroll->setWidget(main_wdgt);
    scroll->setWidgetResizable(true);

    QVBoxLayout* main_vbox = new QVBoxLayout();
    main_vbox->addWidget(new QLabel("<H1 align = \"center\">Server</H1>"));
    main_vbox->addWidget(scroll);

    m_table = new RoutingTable(this);
    m_names = new ClientNames;
    m_layout = new QGridLayout(main_wdgt);

    m_edit = new QTextEdit;
    m_edit->setReadOnly(true);

    if(listen(QHostAddress::Any,  m_port))
    {
        QMessageBox::critical(0, "Server Error", "Unable to start the server:" + this -> errorString());
        QTcpServer::close();
        return;
    }

    //->addWidget(m_edit);

    setLayout(main_vbox);
}

void OlyServer::incomingConnection(qintptr socketDescriptor)
{
    QThread* new_connection = new QThread;
    ClientConnection* new_client_connection = new ClientConnection(this, socketDescriptor);
    new_client_connection->moveToThread(new_connection);
    connect(new_connection, &QThread::finished, this, &OlyServer::slot_thread_finished);
    connect(new_client_connection, &ClientConnection::destroyed, this, &OlyServer::slot_client_disconnected);
    connect(new_connection, &QThread::started, new_client_connection, &ClientConnection::slotSocketStart);
    clients_online.push_back(new_connection);
    new_connection->start();
}

ClientConnection* OlyServer::getConnection(quint16 client_id)
{
    return connected_clients.value(client_id);
}

void OlyServer::slot_new_connection(quint16 client_id, QSharedPointer <QTextEdit> client_status_field)
{
    connected_clients.insert(client_id, (ClientConnection*)sender());
    m_layout->addWidget(client_status_field.data(), grid_row_pos, grid_column_pos);
    // TODO! grid_row_pos++;
    grid_column_pos++;
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

void OlyServer::slot_client_disconnected()
{
    quint16 key = connected_clients.key((ClientConnection*)sender());
    connected_clients.remove(key);
    m_edit->append("Client with ID " + QString::number(key)  + " disconnected.\n");
}
