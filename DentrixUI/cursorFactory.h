// CursorFactory.h
#pragma once

#include <QCursor>

enum class BrushMode { Add, Remove, Smooth };

class CursorFactory {
public:
    static QCursor createCursor(int size, BrushMode mode);
};
