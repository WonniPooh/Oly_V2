#ifndef CLIENTNAMESWIDGET_H
#define CLIENTNAMESWIDGET_H

#include <QWidget>
class QDialog;
class QVBoxLayout;
class QLineEdit;
class QLabel;
class QPushButton;

class ClientNamesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ClientNamesWidget(QWidget *parent = 0);
    QString* getClientName(quint16 client_id);
    ~ClientNamesWidget();
private:

    QDialog*     m_dialog;
    QVBoxLayout* main_layout;
    QLabel*      title;
    QLineEdit*   input_field;
    QPushButton* m_button;

};

#endif // CLIENTNAMESWIDGET_H
