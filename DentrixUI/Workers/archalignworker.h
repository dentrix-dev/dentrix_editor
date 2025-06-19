#ifndef ARCHALIGNWORKER_H
#define ARCHALIGNWORKER_H

#include <QObject>

class ArchAlignWorker : public QObject
{
    Q_OBJECT
public:
    explicit ArchAlignWorker(QObject *parent = nullptr);

signals:
};

#endif  // ARCHALIGNWORKER_H
