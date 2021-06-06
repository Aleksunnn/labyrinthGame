#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QDropEvent>
#include <QMimeData>
#include <QDrag>
#include <QDragEnterEvent>
#include <QHeaderView>
#include <QRegion>
#include <QPainter>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    srand(time(NULL));
    m_table = new TableWidget;
    ui->setupUi(this);
    ui->horizontalLayout->insertWidget(0, m_table);
}

Widget::~Widget()
{
    delete ui;
}
