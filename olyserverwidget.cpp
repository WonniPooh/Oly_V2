#include "olyserverwidget.h"
#include "olyserver.h"

#include <QMessageBox>
#include <QTextEdit>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>

typedef QSharedPointer<QString> shared_data;

OlyServerWidget::~OlyServerWidget()
{
    delete m_edit;
    delete m_server;
    delete m_layout;
}

OlyServerWidget::OlyServerWidget(int m_port)
{
    grid_column_pos = 0;
    grid_row_pos = 1;

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

    m_layout->addWidget(m_edit, 0, 0, 0, 2);

    setLayout(main_vbox);

    qRegisterMetaType<shared_data>("shared_data");
    m_server = new OlyServer(m_edit, m_port, this);
}
