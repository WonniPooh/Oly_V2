#include <QSharedPointer>
#include "clientconnection.h"
#include "olyserver.h"

ClientConnection::ClientConnection(OlyServer* server_ptr, int descriptor, QObject *parent) : QObject(parent)
{
    assert(server_ptr);
    qRegisterMetaType<shared_data>("shared_data");
    connect(this, &ClientConnection::newConnection, server_ptr, &OlyServer::slot_new_connection);
    socket_descriptor = descriptor;
    msg_counter = 0;
    client_id = 0;
}

ClientConnection::~ClientConnection()
{
    delete client_socket;
    emit destroyed(client_id);
}

void ClientConnection::slotSocketStart()
{
    client_socket = new QTcpSocket;

    if (!client_socket->setSocketDescriptor(socket_descriptor))
    {
        return;
    }

    connect(client_socket, &QTcpSocket::readyRead, this, &ClientConnection::slotNewMsg);
    connect(client_socket, &QTcpSocket::disconnected, this, &ClientConnection::slotDisconnected);
}

void ClientConnection::slotNewMsg()
{
    quint16 next_block_size = 0;

    QString* data_recieved = new QString;

    QDataStream in(client_socket);

    in.setVersion(QDataStream::Qt_5_5);

    data_pointer.reset(data_recieved);

    for(;;) // msg counter from exact client
    {
        if(msg_counter == 0)
        {
            if(client_socket->bytesAvailable() < sizeof(quint16))
            {
                break;
            }

            in >> client_id;

            emit newConnection(client_id);

            msg_counter++;

            break;
        }

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

        in >> *data_recieved;

        emit sendData(client_id, data_pointer);
    }
}

void ClientConnection::slotSendData(quint16 client_id, shared_data data_to_forward)
{
   QByteArray arrBlock;
   QDataStream out(&arrBlock, QIODevice::WriteOnly);
   out.setVersion(QDataStream::Qt_5_5);

   out << quint16(0) << client_id << *data_to_forward.data();

   out.device()->seek(0);
   out << quint16(arrBlock.size() - sizeof(quint16));
   client_socket->write(arrBlock);
}

void ClientConnection::slotDisconnected()
{
    QThread::currentThread()->exit(0);
}
