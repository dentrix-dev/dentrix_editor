#include "utils.h"

Utils::Utils() {}

pmp::Point Utils::glmToPmpPoint(glm::vec3 glmVector3)
{
    return pmp::Point(glmVector3.x, glmVector3.y, glmVector3.z);
}

glm::vec3 Utils::pmpPointToGlm(pmp::Point pmpPoint)
{
    return glm::vec3(pmpPoint[0], pmpPoint[1], pmpPoint[2]);
}
