#include <QSharedPointer>
#include "clientconnection.h"
#include "olyserver.h"

ClientConnection::ClientConnection(int descriptor, int event_type, QMap<quint16, ConnectionData*>* connected_clients,  QMutex* map_mutex, QObject *parent) : QObject(parent)
{
    map_access = map_mutex;
    connections = connected_clients;
    connected_clients = nullptr;
    client_socket = nullptr;
    socket_descriptor = descriptor;
    m_event = event_type;
    msg_counter = 0;
    client_id = 0;
    qRegisterMetaType<shared_data>("shared_data");
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

bool ClientConnection::event(QEvent* income_event)
{
    if (income_event->type() == m_event)
    {
        sendData();
        return true;
    }

    return QObject::event(income_event);
}

void ClientConnection::sendData()
{
   map_access->lock();
   ConnectionData* data = connections->value(client_id);
   data->mutex->lock();

   int size = data->data_to_forward.length();

   for(int i = 0; i < size; i++)
   {
       QByteArray arrBlock;
       QDataStream out(&arrBlock, QIODevice::WriteOnly);
       out.setVersion(QDataStream::Qt_5_5);

       out << quint16(0) << data->data_to_forward[0].sender_id << *data->data_to_forward[0].data_to_forward.data();
       data->data_to_forward.pop_front();

       out.device()->seek(0);
       out << quint16(arrBlock.size() - sizeof(quint16));
       client_socket->write(arrBlock);
   }

   data->mutex->unlock();
   map_access->unlock();
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

            client_id = data_recieved->toInt();

            emit clientIDRecieved(client_id);

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

        emit displayMsg(client_id, data_pointer);

        QVector<int> recipients = parseJson(data_recieved);

        deleteRecipients(data_recieved);

        int vector_size = recipients.size();

        for(int i = 0; i < vector_size; i++)
        {
            if(connections->contains(recipients[i]))
            {
                ConnectionData* recipient_data = connections->value(recipients[i]);
                recipient_data->mutex->lock();
                ForwardedMsgData data;
                data.sender_id = client_id;
                data.data_to_forward = data_pointer;
                recipient_data->data_to_forward.push_back(data);
                recipient_data->mutex->unlock();
                QCoreApplication::postEvent(recipient_data->connection, new QEvent((QEvent::Type)m_event));
            }
        }
    }
}

void ClientConnection::deleteRecipients(QString* income)
{
    int pos = income->indexOf("]") + 1;
    income->remove(1,pos);
}

QVector<int> ClientConnection::parseJson(QString* income_data)
{
    QVector<int> forward_to;

  // Parse document
    QJsonDocument doc(QJsonDocument::fromJson(income_data->toLatin1()));

    // Get JSON object
    QJsonObject json = doc.object();

    QJsonArray recipients = json["reciver_id"].toArray();

    int size = recipients.size();

    for(int i = 0; i < size; i++)
    {
        forward_to.push_back(recipients[i].toInt());
    }

    return forward_to;
}

void ClientConnection::slotDisconnected()
{
    QThread::currentThread()->exit(0);
}
