#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H
#pragma once

#include <QTcpSocket>
#include <QWidget>
#include <assert.h>

class OlyServer;
typedef QSharedPointer<QString> shared_data;


class ClientConnection : public QObject
{
    Q_OBJECT

signals:
    void sendData(quint16 client_id, shared_data data_to_forward);
    void newConnection(quint16 client_id);
    void destroyed(quint16 client_id);

public slots:
    void slotNewMsg();
    void slotDisconnected();
    void slotSocketStart();
    void slotSendData(quint16 client_id, shared_data data_to_forward);

public:
    ClientConnection(int descriptor, QObject *parent = 0);
    ~ClientConnection();
private:

    int         msg_counter;
    int         socket_descriptor;
    quint16     client_id;
    QTcpSocket* client_socket;
    QSharedPointer <QString> data_pointer;
 };

#endif // CLIENTCONNECTION_H
