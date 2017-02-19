#include "olyserverwidget.h"
#include "olyserver.h"
#include "clientnames.h"
#include "clientconnection.h"

#include <QMessageBox>
#include <QTextEdit>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>

OlyServerWidget::~OlyServerWidget()
{
    delete m_names;
    delete m_edit;
    delete m_server;
    delete m_layout;
}

//creates widget, register QString as Metatype to pass it through signals, creates new Names and Oly_server classes
OlyServerWidget::OlyServerWidget(int m_port)
{
    grid_column_pos = 0;
    grid_row_pos = 3;

    QWidget* main_wdgt = new QWidget;
    QScrollArea* scroll = new QScrollArea;
    scroll->setWidget(main_wdgt);
    scroll->setWidgetResizable(true);

    QVBoxLayout* main_vbox = new QVBoxLayout();
    main_vbox->addWidget(new QLabel("<H1 align = \"center\">Server</H1>"));
    main_vbox->addWidget(scroll);

    m_layout = new QGridLayout(main_wdgt);

    m_edit = new QTextEdit;
    m_edit->setReadOnly(true);

    m_layout->addWidget(m_edit, 0, 0, 2, 3);

    setLayout(main_vbox);

    qRegisterMetaType<shared_data>("shared_data");

    m_names = new ClientNames;

    m_server = new OlyServer(m_edit, m_port, m_names, this);
}

//creates new status_field to list income msges, and inits name set function
void  OlyServerWidget::slotClientConnected(quint16 client_id)
{
    StatusFieldData* field = new StatusFieldData;
    field->status_field = new QTextEdit;
    field->status_field->setReadOnly(true);

    m_names->processNewConnection(client_id);
    field->field_title = new QLabel(m_names->getClientName(client_id));
    field->field_title->setAlignment(Qt::AlignCenter);
    field->field_coords = QPoint(grid_column_pos, grid_row_pos);

    m_client_status_fields.insert(client_id, field);

    m_layout->addWidget(field->field_title, grid_row_pos, grid_column_pos);
    m_layout->addWidget(field->status_field, grid_row_pos+1, grid_column_pos++);

    grid_row_pos += 2 * (grid_column_pos / 3);
    grid_column_pos = grid_column_pos % 3;
    emit nameSet(client_id, qobject_cast<ClientConnection*>(sender()));
}

//prints msg recieved on the screen
void  OlyServerWidget::newMsg(quint16 client_id, shared_data forwarded_data)
{
    StatusFieldData* client_field = m_client_status_fields.value(client_id);
    client_field->status_field->append(*forwarded_data.data());
}

//remove textfield of disconnected client and move other filling empty space
void  OlyServerWidget::slotClientDisconnected(quint16 client_id)
{
    QMap<quint16, StatusFieldData*>::iterator i = m_client_status_fields.begin();

    StatusFieldData* last_status_field = nullptr;

    int max_x = 0;
    int max_y = 3;

    for(; i != m_client_status_fields.end(); i++)
    {
        if(i.value()->field_coords.y() >= max_y)
        {
            if(i.value()->field_coords.y() > max_y)
            {
                max_y = i.value()->field_coords.y();
                last_status_field = i.value();
                max_x = 0;
            }

            if(i.value()->field_coords.x() > max_x)
            {
                max_x = i.value()->field_coords.x();
                last_status_field = i.value();
            }
        }
    }


    m_layout->removeWidget(m_client_status_fields.value(client_id)->field_title);
    m_layout->removeWidget(m_client_status_fields.value(client_id)->status_field);

    delete m_client_status_fields.value(client_id)->field_title;
    delete m_client_status_fields.value(client_id)->status_field;

    if(grid_column_pos)
        grid_column_pos -=1;
    else
    {
        grid_column_pos = 2;
        grid_row_pos -= 2;
    }

     if(grid_column_pos == m_client_status_fields.value(client_id)->field_coords.x() && grid_row_pos == m_client_status_fields.value(client_id)->field_coords.y())
     {
         delete m_client_status_fields.value(client_id);
         m_client_status_fields.remove(client_id);
         return;
     }

    last_status_field->field_coords = QPoint(m_client_status_fields.value(client_id)->field_coords.x(), m_client_status_fields.value(client_id)->field_coords.y());

    m_layout->removeWidget(last_status_field->field_title);
    m_layout->removeWidget(last_status_field->status_field);

    m_layout->addWidget(last_status_field->field_title, m_client_status_fields.value(client_id)->field_coords.y(), m_client_status_fields.value(client_id)->field_coords.x());
    m_layout->addWidget(last_status_field->status_field, m_client_status_fields.value(client_id)->field_coords.y() + 1, m_client_status_fields.value(client_id)->field_coords.x());

    m_client_status_fields.remove(client_id);
}
