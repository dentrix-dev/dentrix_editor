#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>

class Vertex
{
public:
    glm::vec3 Position;
    glm::vec3 Normal;

    Vertex(glm::vec3 Position, glm::vec3 Normal);
};

#endif // VERTEX_H
