#include "camera.h"

glm::mat4 projection;

void Camera::Resize(int width, int height)
{
    projection = glm::ortho(
        0.0f,            // left
        (float)width,    // right
        (float)height,   // bottom
        0.0f,            // top
        -1.0f, 1.0f      // near and far planes
    );
}

glm::mat4& Camera::GetProjection()
{
    return projection;
}
