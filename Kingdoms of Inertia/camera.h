#pragma once
#include <Glm/glm.hpp>
#include <Glm/gtc/matrix_transform.hpp>

namespace Camera {
	void Resize(int width, int height);
	glm::mat4& GetProjection();
}

