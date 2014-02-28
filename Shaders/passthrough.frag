#version 420 core

noperspective in vec2 UV;

out vec4 FragColor;

uniform sampler2D renderedTexture;

void main()
{
	FragColor = texture(renderedTexture, UV);
}
