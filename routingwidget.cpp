#include "routingwidget.h"
#include "routingtable.h"
#include "clientnames.h"

RoutingWidget::~RoutingWidget()
{
    delete m_dialog;
}

RoutingWidget::RoutingWidget(quint16 client_to_serve, RoutingTable *table_to_use, ClientNames *names_to_use, QObject *parent) : QObject(parent)
{
    assert(table_to_use);
    assert(names_to_use);
    client_id = client_to_serve;
    table = table_to_use;
    names = names_to_use;

    m_dialog = new QDialog;
    main_layout = new QVBoxLayout;
    m_scroll = new QScrollArea;
    m_widget = new QWidget;
    m_grid_layout = new QGridLayout(m_widget);
    m_button = new QPushButton;
    lable_from = new QLabel;
    lable_to = new QLabel;
    m_dialog_header = new QLabel;

    m_scroll->setWidget(m_widget);
    m_scroll->setWidgetResizable(true);
    main_layout->addWidget(m_dialog_header);
    main_layout->addWidget(m_scroll);

    m_button->setText("OK");
    m_button->setDefault(true);
    QObject::connect(m_button, &QPushButton::clicked, m_dialog, &QDialog::accept);

    lable_from->setText("<H3 align = \"center\">From</H3>");
    lable_to->setText("<H3 align = \"center\">To</H3>");
    m_grid_layout->addWidget(lable_from, 0, 1);
    m_grid_layout->addWidget(lable_to, 0, 2);
}

void RoutingWidget::changeExistingConnection()
{
    m_dialog_header->setText("<H1 align = \"center\">Change Routes For Existing Client: " + names->getClientName(client_id) + "</H1>");

    const QVector<quint16>* connected_clients = table->getConnectedClients();
    const QVector<quint16>* client_routes = table->getClientRoutes(client_id);

    int max_routes_num = connected_clients->length() - 1;
    int id_array_pos = 0;
    bool* was_checked = new bool[2 * max_routes_num];
    QCheckBox* m_boxes = new QCheckBox[2 * max_routes_num];
    QLabel* m_lables = new QLabel[max_routes_num];

    for(int i = 0; i < max_routes_num; i++)
    {
        if(client_id != connected_clients[0][i])
        {
            m_lables[i].setText(names->getClientName(connected_clients[0][i]));
            if(client_routes->indexOf(connected_clients[0][i]) != -1)
            {
                m_boxes[ 2*i ].setChecked(true);
                was_checked[2*i] = true;
            }
            else
                was_checked[2*i] = false;

            if((table->getClientRoutes(connected_clients[0][i]))->indexOf(client_id) != -1)
            {
                m_boxes[2*i+1].setChecked(true);
                was_checked[2*i+1] = true;
            }
            else
                was_checked[2*i+1] = false;

            m_grid_layout->addWidget(&m_lables[i ], i + 1, 0);
            m_grid_layout->addWidget(&m_boxes[ 2*i ], i + 1, 1);
            m_grid_layout->addWidget(&m_boxes[2*i+1], i + 1, 2);
        }
        else
            id_array_pos = i;
    }

    m_grid_layout->addWidget(m_button, max_routes_num + 1, 1);

    m_dialog->setLayout(main_layout);
    m_dialog->setModal(true);
    m_dialog->exec();

    QVector<quint16>* new_client_routes = new QVector<quint16>;

    for(int i = 0; i < connected_clients[0].length(); i++)
    {
        if(m_boxes[2*i].isChecked())
        {
            new_client_routes->push_back(connected_clients[0][i]);
        }

        if(m_boxes[2*i + 1].isChecked() && !was_checked[2*i + 1])
        {
            table->addNewRoute(connected_clients[0][i], client_id);
        }
        else
        {
            if(!m_boxes[2*i + 1].isChecked() && was_checked[2*i + 1])
            {
                table->deleteClientRoute(connected_clients[0][i], client_id);
            }
        }
    }

    table->setNewRoutes(client_id, new_client_routes);

    delete [] m_boxes;
    delete [] m_lables;
}

void RoutingWidget::processNewConnection()
{

    const QVector<quint16>* connected_clients = table->getConnectedClients();

    QVector<quint16>* new_client_routes = new QVector<quint16>;

    if(connected_clients[0].length())
    {
        m_dialog_header->setText("<H1 align = \"center\">Set Routes For New Client: " + names->getClientName(client_id) + "</H1>");
        QCheckBox* m_boxes = new QCheckBox[2 * connected_clients[0].length()];
        QLabel* m_lables = new QLabel[connected_clients[0].length()];

        for(int i = 0; i < connected_clients[0].length(); i++)
        {
            m_lables[i].setText(names->getClientName(connected_clients[0][i]));

            m_grid_layout->addWidget(&m_lables[i], i + 1, 0);
            m_grid_layout->addWidget(&m_boxes[ 2*i ], i + 1, 1);
            m_grid_layout->addWidget(&m_boxes[2*i+1], i + 1, 2);
        }

        if(connected_clients[0].length())
            m_grid_layout->addWidget(m_button, connected_clients[0].length() + 1, 1);
        else
            m_grid_layout->addWidget(m_button, connected_clients[0].length() + 1, 1, 2, 2);

        m_dialog->setLayout(main_layout);
        m_dialog->setModal(true);
        m_dialog->exec();

        for(int i = 0; i < connected_clients[0].length(); i++)
        {
            if(m_boxes[2*i].isChecked())
            {
                new_client_routes->push_back(connected_clients[0][i]);
            }

            if(m_boxes[2*i + 1].isChecked())
            {
                table->addNewRoute(connected_clients[0][i], client_id);
            }
        }

        delete [] m_boxes;
        delete [] m_lables;
    }

    table->addNewClient(client_id, new_client_routes);
}
