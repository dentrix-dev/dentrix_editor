#include "camera.h"

#include <iostream>

#include "glm/detail/type_vec3.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(position, target, up);
}

void Camera::processMouse(float xOffset, float yOffset)
{
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw = glm::mod(yaw + xOffset, 360.f);
    pitch += yOffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    position = target + glm::normalize(direction) * distance;
    front = glm::normalize(position * -1.0f);
}

void Camera::processMove(float xPan, float yPan)
{
    xPan *= sensitivity;
    yPan *= sensitivity;

    glm::vec3 right = glm::normalize(glm::cross(front, up));
    glm::vec3 cameraUp = glm::normalize(glm::cross(right, front));
    target += right * -xPan + cameraUp * yPan;
    position += right * -xPan + cameraUp * yPan;
}

void Camera::addDistance(float offset)
{
    distance -= offset;
    if (distance < 5.0f) distance = 5.0f;
    if (distance > 150.0f) distance = 150.0f;

    position = target + glm::normalize(position - target) * distance;
}

void Camera::ScreenPosToWorldRay(int mouseX, int mouseY, int screenWidth, int screenHeight, glm::mat4 ViewMatrix,
                                 glm::mat4 ProjectionMatrix, glm::vec3& out_direction)
{
    // 1. Screen to NDC
    float ndcX = (2.0f * mouseX) / screenWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * mouseY) / screenHeight;  // Flip Y for OpenGL NDC

    // 2. NDC to Clip Space (using near plane projection in NDC)
    glm::vec4 ray_clip = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);

    // 3. Clip Space to View Space
    glm::mat4 inverseProjection = glm::inverse(ProjectionMatrix);
    glm::vec4 ray_eye_h = inverseProjection * ray_clip;

    // Check if perspective divide is needed (W might not be 1)
    if (ray_eye_h.w != 0.0f) {
        ray_eye_h /= ray_eye_h.w;
    } else {
        // Handle potential error or orthographic projection case
        // For standard perspective, w should not be 0 here.
        // Maybe set ray_eye_h.w = 1.0f if needed, although the math should work out.
        std::cout << "ERROR::CAMERA::SCREENPOSTOWORLDRAY::PERSPECTIVE_DIVIDE" << std::endl;
    }
    // The resulting ray_eye_h.xyz is now a point on the near plane in View Space.
    // The direction vector in View Space goes from the origin (camera) to this point.
    glm::vec3 ray_view_dir = glm::normalize(glm::vec3(ray_eye_h.x, ray_eye_h.y, ray_eye_h.z));

    // 4. View Space to World Space
    glm::mat4 inverseView = glm::inverse(ViewMatrix);
    // Transform the direction from View Space to World Space
    // Use w=0 for transforming directions
    glm::vec4 ray_world_dir_h = inverseView * glm::vec4(ray_view_dir, 0.0f);
    out_direction = glm::normalize(glm::vec3(ray_world_dir_h));
}
