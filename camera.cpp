#include <iostream>
#include "glm/ext/matrix_transform.hpp"
#include "camera.h"

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

void Camera::processMouse(float xOffset, float yOffset) {
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw = glm::mod(yaw + xOffset, 360.f);
    pitch += yOffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    position = glm::normalize(direction) * distance;
    front = glm::normalize(position * -1.0f);
}

void Camera::addDistance(float offset) {
    distance -= offset;
    if (distance < 5.0f)
        distance = 5.0f;
    if (distance > 150.0f)
        distance = 150.0f;

    position = glm::normalize(position) * distance;
}
