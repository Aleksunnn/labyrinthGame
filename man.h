#ifndef MANNEW_H
#define MANNEW_H

#include <QWidget>

namespace Ui {
class Man;
}

class Man : public QWidget
{
    Q_OBJECT

public:
    explicit Man(QWidget *parent = nullptr);
    ~Man();
    bool setCount(int count);
    void setTable(int row,int col);
    int getCount();
    bool setInTable();
    int getRow();
    int getCol();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
signals:
    void openMist();

private:
    QPixmap m_pixmap;
    QPoint m_dragStart;
    int m_count;
    bool m_inTable;
    int m_row;
    int m_col;
    Ui::Man *ui;
};

#endif // MANNEW_H
