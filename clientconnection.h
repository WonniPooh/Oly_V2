#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H
#pragma once

#include <QTcpSocket>
#include <QWidget>
#include <assert.h>
#include <QMap>
#include <QMutex>

class OlyServer;
class ClientConnection;
typedef QSharedPointer<QString> shared_data;

struct ForwardedMsgData
{
    quint16 sender_id;
    QSharedPointer<QString> data_to_forward;
};

struct ConnectionData
{
    ClientConnection* connection;
    QVector<ForwardedMsgData> data_to_forward;
    QMutex* mutex;
};

class ClientConnection : public QObject
{
    Q_OBJECT

signals:
    void displayMsg(quint16 client_id, shared_data data_to_forward);
    void clientIDRecieved(quint16 client_id);       //to init name set widget
    void destroyed(quint16 client_id);              //emited in destructor

public slots:
    void slotNewMsg();
    void slotDisconnected();
    void slotSocketStart();

public:
    ClientConnection(int descriptor, int event_type, QMap<quint16, ConnectionData*>* connected_clients, QMutex* map_mutex, QObject *parent = 0);
    ~ClientConnection();

private:
    QMutex*     map_access;
    QVector<int>  parseJson(QString* income_data);
    void        deleteRecipients(QString* income);
    bool        event(QEvent* income_event);
    void        sendData();
    int         m_event;
    int         msg_counter;
    int         socket_descriptor;
    quint16     client_id;
    QTcpSocket* client_socket;
    QSharedPointer <QString>            data_pointer;
    QMap<quint16, ConnectionData*>*      connections;
 };

#endif // CLIENTCONNECTION_H
