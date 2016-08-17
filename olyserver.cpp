#include "olyserver.h"

#include "clientconnection.h"
#include "routingtable.h"
#include "clientnames.h"
#include "routingwidget.h"

#include <QMessageBox>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QThread>

OlyServer::~OlyServer()
{
    for(int i = 0; i < clients_online.length(); i++)
    {
        clients_online[i]->quit();
        clients_online[i]->wait();
    }

    delete [] txt_edit;
    delete [] m_hbox;
    delete m_vbox;
    delete m_edit;
    delete m_table;
    delete m_names;
}

OlyServer::OlyServer(int m_port)
{
    QWidget* main_wdgt = new QWidget;
    QScrollArea* scroll = new QScrollArea;
    scroll->setWidget(main_wdgt);
    scroll->setWidgetResizable(true);

    QVBoxLayout* m_vbox1 = new QVBoxLayout();
    m_vbox1->addWidget(new QLabel("<H1 align = \"center\">Server</H1>"));
    m_vbox1->addWidget(scroll);

    m_table = new RoutingTable;
    m_names = new ClientNames;
    m_vbox = new QVBoxLayout(main_wdgt);

    m_hbox = new QHBoxLayout[3];
    txt_edit = new QTextEdit[9];
    m_edit = new QTextEdit;
    m_edit->setReadOnly(true);
    m_tcp_server = new QTcpServer(this);

    if(!m_tcp_server -> listen(QHostAddress::Any,  m_port))
    {
        QMessageBox::critical(0, "Server Error", "Unable to start the server:" + m_tcp_server -> errorString());
        m_tcp_server -> close();
        return;
    }
    connect(m_tcp_server, &QTcpServer::newConnection, this, &OlyServer::slot_new_connection);


    for(int i = 0; i< 9; i++)
    {
        txt_edit[i].setReadOnly(true);
        txt_edit[i].append(QString::number(i));
    }

    for(int i = 0; i < 3; i++)
    {
        m_hbox[i].addWidget(&txt_edit[3*i]);

        m_hbox[i].addWidget(&txt_edit[3*i + 1]);

        m_hbox[i].addWidget(&txt_edit[3*i + 2]);
    }

    m_vbox->addWidget(m_edit);

    for(int i = 0; i < 3; i++)
        m_vbox->addLayout(&m_hbox[i]);

    setLayout(m_vbox1);
}

QTcpSocket *OlyServer::get_connection(quint16 client_ID)
{
    return connected_clients.value(client_ID);
}

void OlyServer::slot_new_connection()
{
    QTcpSocket* new_connection = m_tcp_server -> nextPendingConnection();
    connect(new_connection, &QTcpSocket::disconnected, this, &OlyServer::slot_client_disconnected);
    connect(new_connection, &QTcpSocket::readyRead, this, &OlyServer::slot_read_first_msg);
    connect(new_connection, &QTcpSocket::disconnected, new_connection, &QTcpSocket::deleteLater);
    m_edit->append("New Pending Connection...");
}

void OlyServer::slot_read_first_msg()
{
    quint16 client_id;
    QThread* new_client = new QThread;
    clients_online.push_back(new_client);
    QTcpSocket* m_client_socket = (QTcpSocket*)sender();

    QDataStream in(m_client_socket);
    in.setVersion(QDataStream::Qt_5_5);

    for(;;)
    {
        if(m_client_socket->bytesAvailable() < sizeof(quint16))
        {
           break;
        }

        in >> client_id;
        break;
    }

    m_names->addNewClient(client_id, nullptr);
    RoutingWidget new_client_routes(client_id, m_table, m_names);
    new_client_routes.processNewConnection();
    m_edit->append(QString("New Client With ID ") + QString::number(client_id) + QString(" Connected!"));

    //txt_edit[clients_online.length() - 1].clear();//TODO!! --v + delete
    ClientConnection* new_connection = new ClientConnection(this, client_id, m_client_socket, m_table, &txt_edit[clients_online.length()]);
    connected_clients.insert(client_id, m_client_socket);
    disconnect(m_client_socket, &QTcpSocket::readyRead, this, &OlyServer::slot_read_first_msg);
    connect(m_client_socket, &QTcpSocket::disconnected, this, &OlyServer::slot_client_disconnected);
    new_connection->moveToThread(new_client);
    connect(new_client, &QThread::finished, new_connection, &QObject::deleteLater);
    connect(new_client, &QThread::finished, this, &OlyServer::slot_thread_finished, Qt::QueuedConnection);
    new_client->start();
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
    quint16 key = connected_clients.key((QTcpSocket*)sender());
    connected_clients.remove(key);
    m_edit->append("Client with ID " + QString::number(key)  + " disconnected.\n");
}
