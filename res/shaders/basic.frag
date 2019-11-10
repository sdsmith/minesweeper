#version 330 core

in vec3 vertexColor;
in vec2 texCoord;
out vec4 color;

uniform sampler2D tex1;
uniform sampler2D tex2;

void main() {
    color = mix(texture(tex1, texCoord), texture(tex2, texCoord), 0.2);
}
