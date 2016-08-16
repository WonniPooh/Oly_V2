#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H
#pragma once

#include <QTcpSocket>
#include <QTextEdit>
#include <QWidget>
#include <assert.h>

class OlyServer;
class RoutingTable;

class ClientConnection : public QObject
{
    Q_OBJECT
public slots:
    void slot_new_msg();
    void slot_disconnected();

public:
    ClientConnection(int n);
    ClientConnection(OlyServer* server, quint16 ID, QTcpSocket* connection, RoutingTable *table, QTextEdit* output_field, QObject *parent = 0);

private:
    void sendData(QTcpSocket *connection, const QString &str);

    quint16 client_ID;
    QTextEdit* output;
    OlyServer* current_server;
    QTcpSocket* client_socket;
    RoutingTable* routing_table;
};

#endif // CLIENTCONNECTION_H

/*
 * I have rouing table, client's threads Qvector, and connected clients QMap (Id/socket adress).
 * Routing table is pretty stable thing, i have to modify it only when I connect new unknow client etc. Sooooo if the client is disconnected i won't touch it.
 * If client is disconnected, I should delete it's (QTcpSocket*, ProgId) from the QMap(connected clients), delete information about its thread.
 */
