#version 460 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 texCoords;

out vec2 fragTextCoords;
uniform mat4 projection;
void main() {
    gl_Position = projection * vec4(aPos, 1.0f, 1.0);
	fragTextCoords = texCoords;
}
