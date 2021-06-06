#ifndef WIDGETNEW_H
#define WIDGETNEW_H

#include <QWidget>
#include "man.h"
#include "tablewidget.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();


protected:
    TableWidget *m_table;
    Man *m_man;
    Ui::Widget *ui;
};

#endif // WIDGETNEW_H
