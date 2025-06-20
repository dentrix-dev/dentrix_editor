#include "jawloaderworker.h"

JawLoaderWorker::JawLoaderWorker(QString path) : path{std::move(path)} {}

void JawLoaderWorker::run()
{
    JawLoadResult result;

    std::string pathStr = path.toStdString();
    std::string labelPath = pathStr.substr(0, pathStr.size() - 4) + ".txt";
    std::vector<pmp::Face> remap;

    pmp::SurfaceMesh unsegmented = pmp::read(pathStr, &remap);
    result.upperJawUnsegmented = unsegmented;
    auto arch = mcg::arch_segment(unsegmented, labelPath.c_str(), remap);
    result.arch = arch;

    pmp::SurfaceMesh gum = mcg::mesh_extract(unsegmented, arch.gum_faces);
    gum.add_vertex_property<bool>("v:selected");
    result.gum = gum;

    pmp::BoundingBox aabb = pmp::bounds(gum);
    pmp::Point center = 0.5f * (aabb.min() + aabb.max());
    int count = 1;

    for (const auto& tooth : arch.teeth) {
        if (!tooth.is_present) continue;
        pmp::SurfaceMesh mesh = mcg::mesh_extract(unsegmented, tooth.faces);
        result.teeth.emplace_back(mesh, tooth.name);

        aabb = pmp::bounds(mesh);
        center += 0.5f * (aabb.min() + aabb.max());
        count++;
    }

    result.center = center / float(count);
    emit finished(result);
}
