#ifndef JAWLOADERWORKER_H
#define JAWLOADERWORKER_H

#include <mcg/dental/arch_alignment.h>
#include <mcg/dental/segmentation.h>
#include <mcg/mesh_utils.h>
#include <pmp/io/io.h>
#include <pmp/surface_mesh.h>

#include <QObject>

#include "pmp/algorithms/utilities.h"
#include "pmp/mat_vec.h"

// JawLoadResult.h
struct JawLoadResult {
    pmp::SurfaceMesh gum;
    std::vector<std::pair<pmp::SurfaceMesh, int>> teeth;  // mesh and tooth ID
    pmp::Point center;
    std::vector<pmp::Face> remap;  // <- include this
};

class JawLoaderWorker : public QObject
{
    Q_OBJECT
public:
    explicit JawLoaderWorker(QString path);

signals:
    void finished(JawLoadResult result);

public slots:
    void run();

private:
    QString path;
};

#endif  // JAWLOADERWORKER_H
