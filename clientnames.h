#ifndef CLIENTNAMES_H
#define CLIENTNAMES_H
#pragma once

#include <QMap>
#include <QString>

class ClientNames
{
public:
    ClientNames();
    ~ClientNames();
    const QString& getClientName(quint16 client_id);
    bool changeName(quint16 client_id, QString* new_name);
    void processNewConnection(quint16 client_id);
    bool deleteClient(quint16 client_id);

private:
    QMap <quint16, QString*> names;
};

#endif // CLIENTNAMES_H
