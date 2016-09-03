#include "clientnames.h"
#include "clientnameswidget.h"

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

void ClientNames::processNewConnection(quint16 client_id)
{
    if(names.value(client_id))
        return;

    ClientNamesWidget new_name;
    names.insert(client_id, new_name.getClientName(client_id));
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
    QMap <quint16, QString*>::iterator i = names.begin();

    for(; i != names.end(); i++)
        delete i.value();
}
