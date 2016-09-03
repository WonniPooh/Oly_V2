#include "clientnameswidget.h"
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>

ClientNamesWidget::ClientNamesWidget(QWidget *parent) : QWidget(parent)
{
    m_dialog = new QDialog;
    main_layout = new QVBoxLayout;
    input_field = new QLineEdit;
    title = new QLabel("<H1 align = \"center\">Name the new connection:</H1>");
    m_button = new QPushButton("OK");
}

ClientNamesWidget::~ClientNamesWidget()
{
    delete title;
    delete input_field;
    delete main_layout;
    delete m_dialog;
}

QString* ClientNamesWidget::getClientName(quint16 client_id)
{
    QString* client_name = new QString("Client " + QString::number(client_id));
    QObject::connect(m_button, &QPushButton::clicked, m_dialog, &QDialog::accept);
    input_field->setText(*client_name);
    main_layout->addWidget(title);
    main_layout->addWidget(input_field);
    main_layout->addWidget(m_button);
    m_dialog->setLayout(main_layout);
    m_dialog->setModal(true);
    m_dialog->exec();
    *client_name = input_field->text();
    return client_name;
}
