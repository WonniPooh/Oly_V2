#ifndef ROUTINGWIDGET_H
#define ROUTINGWIDGET_H
#pragma once

#include <QDialog>
#include <QObject>
#include <QVector>
#include <QGridLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QWidget>
#include <assert.h>
#include <QVBoxLayout>

class ClientNames;
class RoutingTable;

class RoutingWidget : public QObject
{
    Q_OBJECT
public:
    ~RoutingWidget();
    RoutingWidget(quint16 client_id, RoutingTable* table_to_use, ClientNames* names_to_use, QObject *parent = 0);
    void processNewConnection();
    void changeExistingConnection();
private:
    quint16 client_id;

    QVBoxLayout* main_layout;
    QDialog* m_dialog;
    QLabel* m_dialog_header;
    QScrollArea* m_scroll;
    QWidget* m_widget;
    QLabel* lable_to;
    QLabel* lable_from;
    QPushButton* m_button;

    QGridLayout* m_grid_layout;
    RoutingTable* table;
    ClientNames* names;
};

#endif // ROUTINGWIDGET_H
