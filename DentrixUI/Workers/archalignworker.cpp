#include "archalignworker.h"

ArchAlignWorker::ArchAlignWorker(ArchAlignData archAlignData, QObject *parent) : QObject{parent}
{
    this->archAlignData = archAlignData;
}

void ArchAlignWorker::run()
{
    mcg::Segmentation_Result seg{};
    seg.upper = archAlignData.upperArch;
    seg.lower = archAlignData.lowerArch;

    pmp::mat4 t = pmp::rotation_matrix_x(10.0f) * pmp::rotation_matrix_y(67.0f) * pmp::rotation_matrix_z(103.0f);
    t = pmp::translation_matrix(pmp::vec3{5.0f, 2.3f, -4.2}) * t;
    mcg::mesh_transform(archAlignData.upperJawUnsegmented, t);
    mcg::Alignment_Result res =
        mcg::arch_align_upper_and_lower(archAlignData.upperJawUnsegmented, archAlignData.lowerJawUnsegmented, seg);

    // Apply arch alignment
    mcg::mesh_transform(archAlignData.lowerJawUnsegmented, res.lower_transform);
    mcg::mesh_transform(archAlignData.upperJawUnsegmented, res.upper_transform);

    // Rotate towards camera
    t = pmp::rotation_matrix_x(-90.0f);
    mcg::mesh_transform(archAlignData.upperJawUnsegmented, t);
    t = pmp::rotation_matrix_x(-90.0f);
    mcg::mesh_transform(archAlignData.lowerJawUnsegmented, t);

    archAlignData.upperGumMesh =
        mcg::mesh_extract(archAlignData.upperJawUnsegmented, archAlignData.upperArch.gum_faces);
    for (mcg::Tooth t : archAlignData.upperArch.teeth) {  // get from meber upperArch
        if (t.is_present) {
            // to worker 3ady
            pmp::SurfaceMesh tooth = mcg::mesh_extract(archAlignData.upperJawUnsegmented, t.faces);  // op on mem
            archAlignData.toothWithNameUpper.push_back(std::pair(tooth, t.name));
        }
    }
    // same but for lower
    archAlignData.lowerGumMesh =
        mcg::mesh_extract(archAlignData.lowerJawUnsegmented, archAlignData.lowerArch.gum_faces);
    for (mcg::Tooth t : archAlignData.lowerArch.teeth) {
        if (t.is_present) {
            pmp::SurfaceMesh tooth = mcg::mesh_extract(archAlignData.lowerJawUnsegmented, t.faces);
            archAlignData.toothWithNameLower.push_back(std::pair(tooth, t.name));
        }
    }

    emit finished(archAlignData);
}
