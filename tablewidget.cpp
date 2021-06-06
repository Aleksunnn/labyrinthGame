#include "tablewidget.h"
#include <QDebug>
#include <QDropEvent>
#include <QMimeData>
#include "man.h"
#include "widget.h"

TableWidget::TableWidget()
{
    setColumnCount(m_fieldSize);
    setRowCount(m_fieldSize);
    for(int i = 0; i < m_fieldSize; i++)
    {
        for(int j = 0; j < m_fieldSize; j++)
        {
            QTableWidgetItem *t = new QTableWidgetItem("");
            setItem(i, j, t);
        }
    }
    for(int i = 0; i < m_fieldSize; i++)
    {
        setColumnWidth(i, 50);
        setRowHeight(i, 50);
    }
    setFirstPoint();
    setFirstDirection();
    drawLabyrinth();
    drawWall();
    drawMist();
    setAcceptDrops(true);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setSelectionMode(QAbstractItemView::NoSelection);
    Man *widget = new Man;
    widget->setCount(1);
    widget->setInTable();
    widget->setTable(0, m_startPoint);
    setCellWidget(0, m_startPoint, widget);
    connect(this, SIGNAL(mist()), this, SLOT(openMist()));
}

void TableWidget::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug()<<"widget dragEnterEvent";
     if (event->mimeData()->hasFormat("aplication/subject")
             || event->mimeData()->hasFormat("aplication/subjectInTable")) {
         event->acceptProposedAction();
         qDebug()<<"accept";
      }
     else
     {
         qDebug()<<"ignore";
         event->ignore();
     }
     return;
}


void TableWidget::dropEvent(QDropEvent *event)
{
    qDebug()<<"widget dropEvent";

    QModelIndex indexCur = indexAt(event->pos());
    if (!indexCur.isValid())
        return;
    if (event->mimeData()->hasFormat("aplication/subjectInTable") )
    {
        QByteArray data = event->mimeData()->data("aplication/subjectInTable");
        QDataStream stream(&data, QIODevice::ReadOnly);
        int row;
        int col;
        stream >> row >> col;
        qDebug() << "from row = "<< row <<" col = "<< col;
        Man *widgetFrom = static_cast<Man *>(cellWidget(row, col));
        int countFrom = widgetFrom->getCount();
        removeCellWidget(row, col);
        event->accept();
        qDebug()<<"widget accept subjectInTable row = " << indexCur.row()
                 << "col =" << indexCur.column();
        if (cellWidget(indexCur.row(),indexCur.column()) != nullptr)
        {
            qDebug() << "widget has already added in table";
            Man *widget = static_cast<Man *>(cellWidget(indexCur.row(),indexCur.column()));
            int count = widget->getCount();
            widget->setCount(count + countFrom);
            return;
        }
        int R = (indexCur.row() - row)*(indexCur.row() - row)  + (indexCur.column() - col)*(indexCur.column() - col);
        if(R > 1)
        {
            Man *widget = new Man;
            int count = widgetFrom->getCount();
            widget->setCount(count);
            widget->setInTable();
            widget->setTable(row, col);
            setCellWidget(row, col, widget);
            qDebug() << "You can move only to 1";
            return;
        }
        if(!(item(indexCur.row(), indexCur.column())->backgroundColor() == waysColor))
        {
            Man *widget = new Man;
            int count = widgetFrom->getCount();
            widget->setCount(count);
            widget->setInTable();
            widget->setTable(row, col);
            setCellWidget(row, col, widget);
            qDebug() << "You can move only on red";
            return;
        }
        Man *widget = new Man;
        widget->setCount(countFrom);
        widget->setInTable();
        widget->setTable(indexCur.row(), indexCur.column());
        setCellWidget(indexCur.row(), indexCur.column(), widget);
        setOneStepColour(indexCur.row(), indexCur.column());
        m_currentX = indexCur.row();
        m_currentY = indexCur.column();
        if(!(m_currentX == m_finishX && m_currentY == m_finishY))
            emit mist();
        else
        {
            qDebug() << "You won!";
            setDragEnabled(false);
        }
        return;
    }

}

bool TableWidget::openMist()
{
    for(int i = m_currentX - 1; i < m_currentX + 2; i++)
        for(int j = m_currentY - 1; j < m_currentY + 2; j++)
        {
            if(item(i, j)->backgroundColor() != wallColor)
            {
                m_coordinates.x = i;
                m_coordinates.y = j;
                if(labyrinthCoordinates.contains(m_coordinates))
                    item(i, j)->setBackgroundColor(waysColor);
                else
                    item(i, j)->setBackgroundColor(pitColor);
            }
        }
    return true;
}

void TableWidget::setFirstPoint()
{
    m_startPoint = 1 + rand() % (m_fieldSize - 2);
    m_currentY = m_startPoint;
    saveLabyrinthCoordinates(m_currentX, m_startPoint);
    qDebug() << "DRAWN " << m_currentX << " " << m_currentY;
}


void TableWidget::setFirstDirection()
{
    int startDirection = rand() % 2;
    m_currenctDirection = startDirection;
        int fromNonZeroTenDrawing = rand() % (m_fieldSize - 2) + 1;
        while(m_currentX < fromNonZeroTenDrawing)
        {
            item(m_currentX + 1, m_currentY)->setBackgroundColor(waysColor);
            saveLabyrinthCoordinates(m_currentX + 1, m_currentY);
            m_currentX++;
            m_stepCountGlobal++;
            qDebug() << "DRAWN " << m_currentX << " " << m_currentY;
        }
        m_currenctDirection = 0;
        setIntersection();
}

void TableWidget::drawLabyrinth()
{
    do
    {
        int drawToLeft = 0;
        int drawToRight = 0;
        int drawToUp = 0;
        int drawToDown = 0;
        bool isAnySteps = 0;
        if(m_checkingCount < 150)
        {
        switch (m_currenctDirection) //0 - horizontal, 1 - vertical
        {
        case 0:
            m_leftOrRight = rand() % 2; //left - 0; right - 1;
            if(m_currentY == 0 || m_currentY == 1)
                m_leftOrRight = 1;
            if(m_currentY == m_fieldSize - 1 || m_currentY == m_fieldSize - 2)
                m_leftOrRight = 0;
            switch (m_leftOrRight)
            {
            case 0:
                drawToLeft = rand() % m_currentY + 1;
                m_nextRandCoordination = m_currentY - drawToLeft;
                while (m_currentY > m_nextRandCoordination)
                {
                    if(!checkIntersection(0))
                        break;
                    item(m_currentX, m_currentY - 1)->setBackgroundColor(waysColor);
                    saveLabyrinthCoordinates(m_currentX, m_currentY - 1);
                    m_currentY--;
                    m_stepCountGlobal++;
                    isAnySteps = 1;
                    qDebug() << "DRAWN " << m_currentX << " " << m_currentY;
                }
                if (isAnySteps)
                    setIntersection();
                m_currenctDirection = 1;
                m_leftOrRight = 2;
                isAnySteps = 0;
                break;
            case 1:
                drawToRight = m_currentY + 1 + rand() % (m_fieldSize - 1 - m_currentY);
                m_nextRandCoordination = drawToRight;
                while (m_currentY < m_nextRandCoordination)
                {
                    if(!checkIntersection(1))
                        break;
                    item(m_currentX, m_currentY + 1)->setBackgroundColor(waysColor);
                    saveLabyrinthCoordinates(m_currentX, m_currentY + 1);
                    m_currentY++;
                    isAnySteps = 1;
                    m_stepCountGlobal++;
                    qDebug() << "DRAWN " << m_currentX << " " << m_currentY;
                }
                if (isAnySteps)
                    setIntersection();
                m_currenctDirection = 1;
                m_leftOrRight = 2;
                isAnySteps = 0;
                break;
            }
            break;
        case 1:
            m_upOrDown = rand() % 2; //up - 0; down - 1;
            if(m_currentX == 0)
                m_upOrDown = 1;
            switch (m_upOrDown)
            {
            case 0:
                drawToUp = rand() % m_currentX + 1;
                m_nextRandCoordination = m_currentX - drawToUp;
                while (m_currentX > m_nextRandCoordination)
                {
                    if(!checkIntersection(2))
                        break;
                    item(m_currentX - 1, m_currentY)->setBackgroundColor(waysColor);
                    saveLabyrinthCoordinates(m_currentX - 1, m_currentY);
                    m_currentX--;
                    isAnySteps = 1;
                    m_stepCountGlobal++;
                    qDebug() << "DRAWN " << m_currentX << " " << m_currentY;
                }
                if (isAnySteps)
                    setIntersection();
                m_currenctDirection = 0;
                m_upOrDown = 2;
                isAnySteps = 0;
                break;
            case 1:
                  drawToDown = m_currentX + 1 + rand() % (m_fieldSize - 1 - m_currentX);
                m_nextRandCoordination = drawToDown;
                while (m_currentX < m_nextRandCoordination)
                {
                    if(!checkIntersection(3))
                        break;
                    item(m_currentX + 1, m_currentY)->setBackgroundColor(waysColor);
                    saveLabyrinthCoordinates(m_currentX + 1, m_currentY);
                    m_currentX++;
                    isAnySteps = 1;
                    m_stepCountGlobal++;
                    qDebug() << "DRAWN " << m_currentX << " " << m_currentY;
                }
                if (isAnySteps)
                    setIntersection();
                m_currenctDirection = 0;
                m_upOrDown = 2;
                isAnySteps = 0;
                break;
            }
            break;
        }
        }
        else
            break;

    } while(!((m_currentX == m_fieldSize - 1) || (m_currentX == 0) || (m_currentY == 0) || (m_currentY == m_fieldSize - 1)));

    if (m_stepCountGlobal < 15 || intersectionCoordinates.size() < 8 || m_checkingCount == 150)
        clearBoard();
    else
    {
        m_finishX = m_currentX;
        m_finishY = m_currentY;
        drawImpasse();
    }
}

bool TableWidget::checkIntersection(int code)
{
    if(m_currentY == m_fieldSize - 2 && m_leftOrRight == 1)
        return true;
    if(m_currentY == 1 && m_leftOrRight == 0)
        return true;
    if(m_currentX == 1 && m_upOrDown == 0)
        return true;
    if(m_currentX == m_fieldSize - 2 && m_upOrDown == 1)
        return true;
    return checkAvailablePath(code);
}



void TableWidget::clearBoard()
{
    for(int i = 0; i < m_fieldSize; i++)
        for(int j = 0; j < m_fieldSize; j++)
            item(i, j)->setBackgroundColor(pitColor);
    m_upOrDown = 2;
    m_leftOrRight = 2;
    m_currenctDirection = 2;
    m_stepCountGlobal = 0;
    m_currentX = 0;
    intersectionCoordinates.clear();
    m_checkingCount = 0;
    labyrinthCoordinates.clear();
    qDebug() << "BOARD WAS CLEARED";
    setFirstPoint();
    setFirstDirection();
    drawLabyrinth();
} 

void TableWidget::drawImpasse()
{
    for(int i = 0; i < intersectionCoordinates.size(); i++)
    {
        for(int j = 0; j < 4; j++)
        {
            m_currenctDirection = j;
            int stopImpasse = 0;
            int firstDir = 0;
            int isAnySteps = 0;
            int drawToLeft;
            int drawToRight;
            int drawToUp;
            int drawToDown;
            m_currentX = intersectionCoordinates[i].x;
            m_currentY = intersectionCoordinates[i].y;
            if(checkingForImpasse(j)) //0 - left, 1 - right, 2 - up, 3 - down
            {
                while(!(stopImpasse == 3))
                {
                    switch (m_currenctDirection)
                    {
                    case 0:
                    {
                        skip_0:
                        drawToLeft = (rand() % (m_currentY));
                        while(m_currentY > drawToLeft)
                        {
                            if(!checkingForImpasse(m_currenctDirection))
                            {
                                if(isAnySteps == 0 && stopImpasse == 0)
                                {
                                    stopImpasse++;
                                    m_currenctDirection = 1;
                                    goto skip_1;
                                }
                                if(stopImpasse == 1)
                                {
                                    if(firstDir == 2)
                                    {
                                        stopImpasse++;
                                        m_currenctDirection = 2;
                                        goto skip_2;
                                    }
                                    if(firstDir == 3)
                                    {
                                        stopImpasse++;
                                        m_currenctDirection = 3;
                                        goto skip_3;
                                    }
                                }
                                if(stopImpasse == 2)
                                    stopImpasse++;
                                break;
                            }
                            item(m_currentX, m_currentY - 1)->setBackgroundColor(waysColor);
                            saveLabyrinthCoordinates(m_currentX, m_currentY - 1);
                            m_currentY--;
                            qDebug() << "DRAWN_IN " << m_currentX << " " << m_currentY;
                            isAnySteps = 1;
                            stopImpasse = 0;
                            firstDir = m_currenctDirection;
                        }
                        m_currenctDirection = 2 + rand() % 2;
                        isAnySteps = 0;
                        break;
                    }
                    case 1:
                    {
                        skip_1:
                        drawToRight = 0;
                        drawToRight = (m_currentY) + 1 + (rand() % (m_fieldSize - 1 - m_currentY));
                        while (m_currentY < drawToRight) {
                            if(!checkingForImpasse(m_currenctDirection))
                            {
                                if(isAnySteps == 0 && stopImpasse == 0)
                                {
                                    stopImpasse++;
                                    m_currenctDirection = 0;
                                    goto skip_0;
                                }
                                if(stopImpasse == 1)
                                {
                                    if(firstDir == 2)
                                    {
                                        stopImpasse++;
                                        m_currenctDirection = 2;
                                        goto skip_2;
                                    }
                                    if(firstDir == 3)
                                    {
                                        stopImpasse++;
                                        m_currenctDirection = 3;
                                        goto skip_3;
                                    }
                                }
                                if(stopImpasse == 2)
                                    stopImpasse++;
                                break;
                            }
                            item(m_currentX, m_currentY + 1)->setBackgroundColor(waysColor);
                            saveLabyrinthCoordinates(m_currentX, m_currentY + 1);
                            m_currentY++;
                            qDebug() << "DRAWN_IN " << m_currentX << " " << m_currentY;
                            firstDir = m_currenctDirection;
                            isAnySteps = 1;
                            stopImpasse = 0;
                        }
                        m_currenctDirection = 2 + rand() % 2;
                        isAnySteps = 0;
                        break;
                    }
                    case 2:
                    {
                        skip_2:
                        drawToUp = (rand() % (m_currentX));
                        while (m_currentX > drawToUp) {
                            if(!checkingForImpasse(m_currenctDirection))
                            {
                                if(isAnySteps == 0 && stopImpasse == 0)
                                {
                                    stopImpasse++;
                                    m_currenctDirection = 3;
                                    goto skip_3;
                                }
                                if(stopImpasse == 1)
                                {
                                    if(firstDir == 0)
                                    {
                                        stopImpasse++;
                                        m_currenctDirection = 0;
                                        goto skip_0;
                                    }
                                    if(firstDir == 1)
                                    {
                                        stopImpasse++;
                                        m_currenctDirection = 1;
                                        goto skip_1;
                                    }
                                }
                                if(stopImpasse == 2)
                                    stopImpasse++;
                                break;
                            }

                            item(m_currentX - 1, m_currentY)->setBackgroundColor(waysColor);
                            saveLabyrinthCoordinates(m_currentX - 1, m_currentY);
                            m_currentX--;
                            qDebug() << "DRAWN_IN " << m_currentX << " " << m_currentY;
                            isAnySteps = 1;
                            stopImpasse = 0;
                            firstDir = m_currenctDirection;

                        }
                            m_currenctDirection = rand() % 2;
                            isAnySteps = 0;
                            break;

                    }
                    case 3:
                    {
                        skip_3:
                        drawToDown = (m_currentX) + 1 + (rand() % (m_fieldSize - 1 - m_currentX));
                        while (m_currentX < drawToDown)
                        {
                            if(!checkingForImpasse(m_currenctDirection))
                            {
                                if(isAnySteps == 0 && stopImpasse == 0)
                                {
                                    stopImpasse++;
                                    m_currenctDirection = 2;
                                    goto skip_2;
                                }
                                if(stopImpasse == 1)
                                {
                                    if(firstDir == 0)
                                    {
                                        stopImpasse++;
                                        m_currenctDirection = 0;
                                        goto skip_0;
                                    }
                                    if(firstDir == 1)
                                    {
                                        stopImpasse++;
                                        m_currenctDirection = 1;
                                        goto skip_1;
                                    }
                                }
                                if(stopImpasse == 2)
                                    stopImpasse++;
                                break;
                            }
                            item(m_currentX + 1, m_currentY)->setBackgroundColor(waysColor);
                            saveLabyrinthCoordinates(m_currentX + 1, m_currentY);
                            m_currentX++;
                            qDebug() << "DRAWN_IN " << m_currentX << " " << m_currentY;
                            isAnySteps = 1;
                            stopImpasse = 0;
                            firstDir = m_currenctDirection;
                        }
                        m_currenctDirection = rand() % 2;
                        isAnySteps = 0;
                        break;
                    }
                    }
                }
            }
        }
    }
}

void TableWidget::setIntersection()
{
    if(m_currentX != 0 && m_currentX != m_fieldSize - 1 && m_currentY != 0 && m_currentY != m_fieldSize - 1)
    {
        m_coordinates.x = m_currentX;
        m_coordinates.y = m_currentY;
        intersectionCoordinates.push_back(m_coordinates);
    }

}

bool TableWidget::checkingForImpasse(int code)
{
    if(m_currentY == m_fieldSize - 2 && m_currenctDirection == 1)
        return false;
    if(m_currentY == 1 && m_currenctDirection == 0)
        return false;
    if(m_currentX == 1 && m_currenctDirection == 2)
        return false;
    if(m_currentX == m_fieldSize - 2 && m_currenctDirection == 3)
        return false;
    return checkAvailablePath(code);
}


bool TableWidget::checkAvailablePath(int code)
{
    m_checkingCount++;
    switch (code) {
    case 0:
        if(item(m_currentX, m_currentY - 1)->backgroundColor() == waysColor || item(m_currentX, m_currentY - 2)->backgroundColor() == waysColor || item(m_currentX + 1, m_currentY - 1)->backgroundColor() == waysColor || item(m_currentX - 1, m_currentY - 1)->backgroundColor() == waysColor)
            return false;
        else
            return true;
    case 1:
        if(item(m_currentX, m_currentY + 1)->backgroundColor() == waysColor || item(m_currentX, m_currentY + 2)->backgroundColor() == waysColor || item(m_currentX + 1, m_currentY + 1)->backgroundColor() == waysColor || item(m_currentX - 1, m_currentY + 1)->backgroundColor() == waysColor)
            return false;
        else
            return true;
    case 2:
        if(item(m_currentX - 1, m_currentY)->backgroundColor() == waysColor || item(m_currentX - 2, m_currentY)->backgroundColor() == waysColor || item(m_currentX - 1, m_currentY + 1)->backgroundColor() == waysColor || item(m_currentX - 1, m_currentY - 1)->backgroundColor() == waysColor)
            return false;
        else
            return true;
    case 3:
        if(item(m_currentX + 1, m_currentY)->backgroundColor() == waysColor || item(m_currentX + 2, m_currentY)->backgroundColor() == waysColor || item(m_currentX + 1, m_currentY + 1)->backgroundColor() == waysColor || item(m_currentX + 1, m_currentY - 1)->backgroundColor() == waysColor)
            return false;
        else
            return true;
    default:
        return true;
    }
}

void TableWidget::drawWall()
{
    for(int i = 0; i < m_fieldSize; i++)
    {
        if(!(item(0, i)->backgroundColor() == waysColor) && !(item(0, i)->backgroundColor() == mistColor))
            item(0, i)->setBackgroundColor(wallColor);
        if(!(item(i, 0)->backgroundColor() == waysColor) && !(item(i, 0)->backgroundColor() == mistColor))
            item(i, 0)->setBackgroundColor(wallColor);
        if(!(item(i, m_fieldSize - 1)->backgroundColor() == waysColor) && !(item(i, m_fieldSize - 1)->backgroundColor() == mistColor))
            item(i, m_fieldSize - 1)->setBackgroundColor(wallColor);
        if(!(item(m_fieldSize - 1, i)->backgroundColor() == waysColor) && !(item(m_fieldSize - 1, i)->backgroundColor() == mistColor))
            item(m_fieldSize - 1, i)->setBackgroundColor(wallColor);
    }
}

void TableWidget::drawMist()
{
    for(int i = 1; i < m_fieldSize - 1; i++)
    {
        for(int j = 1; j < m_fieldSize - 1; j++)
        {
            if(!(i == 1 && j == m_startPoint))
                item(i, j)->setBackgroundColor(mistColor);
        }
    }
}

void TableWidget::saveLabyrinthCoordinates(int x, int y)
{
    m_coordinates.x = x;
    m_coordinates.y = y;
    labyrinthCoordinates.push_back(m_coordinates);
}

bool TableWidget::setOneStepColour(int x, int y)
{
    int nextWayX = x - m_currentX;
    int nextWayY = y - m_currentY;
    if(nextWayX > 0 && nextWayY == 0) //down
        for(int i = 0; i < 3; i++)
            if(item(m_currentX - 1, m_currentY - 1 + i)->backgroundColor() != wallColor)
                item(m_currentX - 1, m_currentY - 1 + i)->setBackgroundColor(mistColor);
    if(nextWayX < 0 && nextWayY == 0) //up
        for(int i = 0; i < 3; i++)
            if(item(m_currentX + 1, m_currentY - 1 + i)->backgroundColor() != wallColor)
                item(m_currentX + 1, m_currentY - 1 + i)->setBackgroundColor(mistColor);
    if(nextWayX == 0 && nextWayY > 0) //right
        for(int i = 0; i < 3; i++)
            if(item(m_currentX - 1 + i, m_currentY - 1)->backgroundColor() != wallColor)
                item(m_currentX - 1 + i, m_currentY - 1)->setBackgroundColor(mistColor);
    if(nextWayX == 0 && nextWayY < 0) //left
        for(int i = 0; i < 3; i++)
            if(item(m_currentX - 1 + i, m_currentY + 1)->backgroundColor() != wallColor)
                item(m_currentX - 1 + i, m_currentY + 1)->setBackgroundColor(mistColor);
    return true;
}
