#include "man.h"
#include "ui_man.h"
#include "widget.h"
#include <QMessageBox>
#include <QPainter>
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDebug>

Man::Man(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Man)
{
    ui->setupUi(this);
    if (!m_pixmap.load("/home/aleksun/parea/A5_2019/man.jpg")) {
        QMessageBox::warning(this, tr("Open Image"),
                             tr("The image file could not be loaded."),
                             QMessageBox::Close);
        return;
    }
    qDebug() << "created m_man";
    setFixedHeight(50);
    setFixedWidth(50);
    setAcceptDrops(true);
    m_count = 0;
    m_inTable = false;

}

Man::~Man()
{
    delete ui;
}

bool Man::setCount(int count)
{
    m_count = count;
    repaint();
    return true;
}

void Man::setTable(int row, int col)
{
    m_row = row;
    m_col = col;
}

int Man::getCount()
{
    return m_count;
}

bool Man::setInTable()
{
    m_inTable = true;
    return  true;
}

int Man::getRow()
{
    return m_row;
}

int Man::getCol()
{
    return m_col;
}

void Man::mouseMoveEvent(QMouseEvent *event)
{
    qDebug()<<"mouseMoveEvent";
    if((event->buttons() & Qt::LeftButton)
    && QApplication::startDragDistance() <= (event->pos() - m_dragStart).manhattanLength()) {
          qDebug()<<"startDragDistance";
          QDrag *drag = new QDrag(this);
          QByteArray itemData;
          QDataStream stream(&itemData, QIODevice::WriteOnly);
          QMimeData *mimeData = new QMimeData;
          if(m_inTable)
          {
               stream << m_row << m_col;
               mimeData->setData("aplication/subjectInTable", itemData);

          }
          else
                mimeData->setData("aplication/subject", itemData);

          drag->setMimeData(mimeData);
          drag->setPixmap(m_pixmap.scaled(size(), Qt::KeepAspectRatio) );
          drag->exec(Qt::MoveAction);
    }

}
void Man::mousePressEvent(QMouseEvent *event)
{
    m_dragStart = event->pos();
    QWidget::mousePressEvent(event);
}

void Man::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(this->rect(), m_pixmap);
    QString text = QVariant(m_count).toString();
    if(m_count != 0)
        painter.drawText(rect(), Qt::AlignRight | Qt::AlignBottom, text);
}
