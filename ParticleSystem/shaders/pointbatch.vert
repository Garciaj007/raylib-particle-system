#version 330 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in float aSize;
layout (location = 2) in vec4 aColor;

varying vec4 vColor;

uniform mat4 uProjection;

void main() {
    vColor = aColor;
    gl_PointSize = aSize;
    gl_Position = uProjection * vec4(aPosition, 0.0, 1.0);
}