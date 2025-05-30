#ifndef UTILS_H
#define UTILS_H
#include "glm/glm.hpp"
#include "pmp/types.h"

class Utils
{
public:
    Utils();
    static pmp::Point glmToPmpPoint(glm::vec3 glmVector3);
    static glm::vec3 pmpPointToGlm(pmp::Point pmpPoint);

    static bool doesRayIntersectAABB(
        glm::vec3 aabb_min,           // Min coord of bounding box
        glm::vec3 aabb_max,           // Max coord of bounding box
        glm::vec3 ray_origin,         // Ray origin, in world space
        glm::vec3 ray_direction,      // Ray direction (NOT target position!), in world space. Must be normalize()'d.
        glm::mat4 ModelMatrix,        // Transformation applied to the mesh
        float& intersection_distance  // Output : distance between ray_origin and the intersection with the OBB
    );
};

#endif  // UTILS_H
