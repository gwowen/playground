#version 330 core
#extension GL_ARB_separate_shader_objects: enable

layout(location = 0) in vec3 vertexPosition_modelspace;

void main()
{
    gl_Position.xyz = vertexPosition_modelspace;
    gl_Position.w = 1.0;
}