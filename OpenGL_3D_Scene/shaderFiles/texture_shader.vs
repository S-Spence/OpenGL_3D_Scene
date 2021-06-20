#version 440 core
layout(location = 0) in vec3 position;
layout(location = 2) in vec2 textureCoordinate;

out vec2 vertexTextureCoordinate;


//Global variables for the transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
    vertexTextureCoordinate = textureCoordinate;
}

