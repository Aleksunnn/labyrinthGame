#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H
#include <QTableWidget>


class TableWidget :public QTableWidget
{
    Q_OBJECT
public:
    TableWidget();
    void setFirstPoint();
    void goNext();
    void setFirstDirection();
    void firstDirectionDrawing();
    void drawLabyrinth();
    bool checkIntersection(int code);
    void clearBoard();
    void drawImpasse();
    void setIntersection();
    bool checkingForImpasse(int code);
    bool checkAvailablePath(int code);
    void drawWall();
    void drawMist();
    void saveLabyrinthCoordinates(int x, int y);
    bool setOneStepColour(int x, int y);
protected:
   void dragEnterEvent( QDragEnterEvent* event ) override;
   void dropEvent( QDropEvent* event ) override;
public slots:
   bool openMist();
signals:
   bool mist();
private:
   int m_fieldSize = 10;
//   int m_columnCount = 10;
   int m_upOrDown = 2; // up - 0; down - 1; 2 - neutral
   int m_leftOrRight = 2; //left - 0; right - 1; 2 - neutral
   int m_startPoint = 0;
   int m_nextRandCoordination = 0;
   int m_currenctDirection = 2; //0 - horizontal; 1 - vertical; 2 - neutral
   int m_currentX = 0;
   int m_currentY = 0;
   int m_finishX = 0;
   int m_finishY = 0;
   int m_stepCountGlobal = 0;
   int m_checkingCount = 0;
   struct Coordinates {
       int x;
       int y;
       bool operator==(const Coordinates& m) const
       {
           return ((m.x == x)&&(m.y == y));
       }
   };
   Coordinates m_coordinates;
   QVector<Coordinates> intersectionCoordinates;
   QVector<Coordinates> labyrinthCoordinates;
   QColor waysColor = Qt::white;
   QColor wallColor = Qt::gray;
   QColor pitColor = Qt::red;
   QColor mistColor = Qt::black;

};

#endif // TABLEWIDGET_H
