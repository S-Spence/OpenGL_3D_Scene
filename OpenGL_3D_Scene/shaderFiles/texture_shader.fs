#version 440 core
in vec2 vertexTextureCoordinate;

out vec4 fragmentColor;

uniform sampler2D uTexture;
uniform vec2 uvScale;

void main()
{
    fragmentColor = texture(uTexture, vertexTextureCoordinate * uvScale);
}