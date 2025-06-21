// CursorFactory.cpp
#include "cursorFactory.h"

#include <QPainter>
#include <QPen>
#include <QPixmap>

QCursor CursorFactory::createCursor(int size, BrushMode mode)
{
    int cursorSize = std::min(64, std::max(8, size * 2));
    QPixmap pixmap(cursorSize, cursorSize);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor color;
    switch (mode) {
        case BrushMode::Add:
            color = Qt::green;
            break;
        case BrushMode::Remove:
            color = Qt::red;
            break;
        case BrushMode::Smooth:
            color = Qt::blue;
            break;
    }

    painter.setPen(QPen(color, 2));
    int padding = cursorSize / 8;
    painter.drawEllipse(padding, padding, cursorSize - padding * 2, cursorSize - padding * 2);
    int center = cursorSize / 2;

    if (mode == BrushMode::Add) {
        painter.drawLine(center, center - 6, center, center + 6);
        painter.drawLine(center - 6, center, center + 6, center);
    } else if (mode == BrushMode::Remove) {
        painter.drawLine(center - 6, center, center + 6, center);
    }
    return QCursor(pixmap, center, center);
}
