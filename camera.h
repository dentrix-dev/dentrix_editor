#include "glm/glm.hpp"

enum MovementDirection { FORWARD, BACK, RIGHT, LEFT };

class Camera {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 100.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float pitch = 0.0f;
    float yaw = -90.0f;
    float speed = 15.0f;
    float sensitivity = 0.2f;

  public:
    glm::mat4 GetViewMatrix();

    void processMouse(float xOffset, float yOffset);
};
