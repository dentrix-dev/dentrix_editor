#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"

enum MovementDirection { FORWARD, BACK, RIGHT, LEFT };

class Camera
{
    float distance = 100.0f;
    float pitch = 0.0f;
    float yaw = 90.0f;
    float sensitivity = 0.2f;
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

public:
    glm::vec3 position = glm::vec3(0.0f, 0.0f, distance);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 target = glm::vec3(0.0f);
    glm::mat4 GetViewMatrix();

    void resetPosition();
    void processMouse(float xOffset, float yOffset);
    void processMove(float xPan, float yPan);
    void addDistance(float distance);

    static void ScreenPosToWorldRay(
        int mouseX, int mouseY,             // Mouse position, in pixels, from bottom-left corner of the window
        int screenWidth, int screenHeight,  // Window size, in pixels
        glm::mat4 ViewMatrix,               // Camera position and orientation
        glm::mat4 ProjectionMatrix,         // Camera parameters (ratio, field of view, near and far planes)
        glm::vec3& out_direction            // Output : Direction, in world space, of the ray that goes
                                            // "through" the mouse.
    );
};
