#ifndef ARCHALIGNWORKER_H
#define ARCHALIGNWORKER_H
#include <mcg/dental/arch_alignment.h>
#include <mcg/dental/segmentation.h>
#include <mcg/mesh_utils.h>
#include <pmp/io/io.h>
#include <pmp/surface_mesh.h>

#include <QObject>

#include "pmp/algorithms/utilities.h"
#include "pmp/mat_vec.h"

struct ArchAlignData {
    mcg::Arch upperArch;
    mcg::Arch lowerArch;
    pmp::SurfaceMesh upperJawUnsegmented;
    pmp::SurfaceMesh lowerJawUnsegmented;
    std::vector<std::pair<pmp::SurfaceMesh, mcg::Tooth_Name>> toothWithNameUpper;
    std::vector<std::pair<pmp::SurfaceMesh, mcg::Tooth_Name>> toothWithNameLower;
    pmp::SurfaceMesh upperGumMesh;
    pmp::SurfaceMesh lowerGumMesh;
};

class ArchAlignWorker : public QObject
{
    Q_OBJECT
public:
    explicit ArchAlignWorker(ArchAlignData archAlignData, QObject *parent = nullptr);

signals:
    void finished(ArchAlignData archAlignData);

public slots:
    void run();

private:
    ArchAlignData archAlignData;
};

#endif  // ARCHALIGNWORKER_H
