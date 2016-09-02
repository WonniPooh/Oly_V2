#include "clientnames.h"

ClientNames::ClientNames()
{

}

const QString& ClientNames::getClientName(quint16 client_id)
{
    return *(names.value(client_id));
}

bool ClientNames::changeName(quint16 client_id, QString* new_name)
{
    bool exist = names.value(client_id);

    if(exist)
    {
        QString* name = names.value(client_id);
        names.remove(client_id);
        delete name;

        if(!new_name)
        {
            name = new QString("ID " + QString::number(client_id));
            names.insert(client_id, name);
        }
        else
            names.insert(client_id, new_name);
        return true;
    }
    else
        return false;
}

void ClientNames::addNewClient(quint16 client_id, QString* name)
{
    if(!name)
    {
        QString* default_name = new QString("ID " + QString::number(client_id));
        names.insert(client_id, default_name);
    }
    else
        names.insert(client_id, name);
}

bool ClientNames::deleteClient(quint16 client_id)
{
    QString* name = names.value(client_id);
    if(name)
        delete name;
    return names.remove(client_id);
}

ClientNames::~ClientNames()
{
    /*QList<QString*> str_to_delete = names.values();

    int list_size = str_to_delete.length();

    for(int i = 0; i < list_size; i++)
    {
        if(str_to_delete.value(i))
            delete str_to_delete.value(i);
    }*/
}
