#include "clientconnection.h"
#include "routingtable.h"
#include "olyserver.h"
#include <QTcpSocket>
#include <assert.h>
#include <QTextEdit>
#include <QWidget>

ClientConnection::ClientConnection(OlyServer *server, quint16 ID, QTcpSocket *connection, RoutingTable* table, QTextEdit* output_field, QObject *parent) : QObject(parent)
{
    assert(server);
    assert(connection);
    client_ID = ID;
    routing_table = table;
    output = output_field;
    current_server = server;
    client_socket = connection;
    connect(client_socket, &QTcpSocket::readyRead, this, &ClientConnection::slot_new_msg);
    connect(client_socket, &QTcpSocket::disconnected, this, &ClientConnection::slot_disconnected);
}

void ClientConnection::slot_new_msg()
{
    quint16 next_block_size = 0;
    const QVector <quint16>* msg_routes = routing_table->getClientRoutes(client_ID);
    int arr_length = msg_routes->length();
    QString str;
    QString strMsg;

    QDataStream in(client_socket);
    in.setVersion(QDataStream::Qt_5_5);

    for(;;) // msg counter from exact client
    {
        if(!next_block_size)
        {
            if(client_socket->bytesAvailable() < sizeof(quint16))
            {
                break;
            }

            in >> next_block_size;
        }

        if(client_socket->bytesAvailable() < next_block_size)
        {
            break;
        }

        in >> str;

        strMsg = "\nMsg: "+ str + "\n MsgSize: " + QString::number(next_block_size);

        next_block_size = 0;

        output->append(strMsg);
        QTcpSocket* route_clients_addr = nullptr;
        for(int i = 0; i < arr_length; i++)
        {
            route_clients_addr = current_server->get_connection(msg_routes[0][i]);
            if(route_clients_addr && (route_clients_addr->state() ==  QAbstractSocket::ConnectedState))
                sendData(route_clients_addr, "Server Response:: Recieved \"" + str + "\""); //QByteArray?
        }
    }
}

void ClientConnection::slot_disconnected()
{
    output->append("Connection closed.");
    QThread::currentThread()->exit(0);
}

void ClientConnection::sendData(QTcpSocket* connection, const QString& str)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_5);

    out << quint16(0) << client_ID << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    connection->write(arrBlock);
}

