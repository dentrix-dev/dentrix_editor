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
};

#endif  // UTILS_H
