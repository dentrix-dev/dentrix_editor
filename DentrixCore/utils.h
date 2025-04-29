#ifndef UTILS_H
#define UTILS_H
#include "pmp/types.h"
#include "glm/glm.hpp"

class Utils
{
public:
    Utils();
    static pmp::Point glmToPmpPoint(glm::vec3 glmVector3);
    static glm::vec3 pmpPointToGlm(pmp::Point pmpPoint);

};

#endif // UTILS_H
