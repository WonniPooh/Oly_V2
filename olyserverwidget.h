#ifndef OLYSERVERWIDGET_H
#define OLYSERVERWIDGET_H
#pragma once

#include <QObject>
#include <QWidget>
#include <QtNetwork>
#include <QMap>

class ClientNames;
class ClientConnection;
class QTextEdit;
class MyTcpServer;
class QGridLayout;
class OlyServer;
class QLabel;
class ClientConnection;

typedef QSharedPointer<QString> shared_data;

struct StatusFieldData
{
    QTextEdit* status_field;
    QLabel*    field_title;
    QPoint field_coords;
};

class OlyServerWidget : public QWidget
{
    Q_OBJECT

signals:
    void  nameSet(quint16 client_id, ClientConnection* sender);

public slots:
    void  newMsg(quint16 client_id, shared_data forwarded_data);
    void  slotClientConnected(quint16 client_id);
    void  slotClientDisconnected(quint16 client_id);

public:

    OlyServerWidget(int m_port);
    ~OlyServerWidget();

private:
    int grid_row_pos;
    int grid_column_pos;

    ClientNames* m_names;
    OlyServer* m_server;
    QGridLayout* m_layout;
    QMap<quint16, StatusFieldData*> m_client_status_fields;
    QTextEdit* m_edit;
};

#endif // OLYSERVERWIDGET_H
