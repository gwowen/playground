#include <cstdio>
#include <cstring>
#include <iostream>

#include <SDL2/SDL.h>

#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include "ogl_util.h"
#include "ogl_maths_3d.h"

GLuint VBO;
GLuint gWorldLocation;

SDL_Window *window;

const char* pVSFileName = "../../shaders/vertex.glsl";
const char* pFSFileName = "../../shaders/fragment.glsl";

static void Render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    static float Scale = 0.0f;

    Scale += 0.001f;

    Matrix4f World;

    World.m[0][0] = cosf(Scale); World.m[0][1] = -sinf(Scale); World.m[0][2] = 0.0f; World.m[0][3] = 0.0f;
    World.m[1][0] = sinf(Scale); World.m[1][1] = cosf(Scale);  World.m[1][2] = 0.0f; World.m[1][3] = 0.0f;
    World.m[2][0] = 0.0f;        World.m[2][1] = 0.0f;         World.m[2][2] = 1.0f; World.m[2][3] = 0.0f;
    World.m[3][0] = 0.0f;        World.m[3][1] = 0.0f;         World.m[3][2] = 0.0f; World.m[3][3] = 1.0f;

    glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &World.m[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);

}

static void CreateVertexBuffer()
{
    Vector3f Vertices[3];
    Vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f);
    Vertices[1] = Vector3f(1.0f, -1.0f, 0.0f);
    Vertices[2] = Vector3f(0.0f, 1.0f, 0.0f);

 	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText,
                        GLenum ShaderType)
{
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(1);
    }

    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0]= strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths);
    glCompileShader(ShaderObj);
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    glAttachShader(ShaderProgram, ShaderObj);

}

static void CompileShaders()
{
    GLuint ShaderProgram = glCreateProgram();

    if (ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }
    string vs, fs;

    if (!ReadFile(pVSFileName, vs)) {
        exit(1);
    };

    if (!ReadFile(pFSFileName, fs)) {
        exit(1);
    };

    AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
    AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
	}

    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(ShaderProgram);

    gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
    assert(gWorldLocation != 0xFFFFFFFF);
}


int main(int argc, char* argv[])
{
    SDL_Event event;

    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow(
        "Triangle test",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_OPENGL
    );

    if(window == NULL)
    {
        std::cout << "Could not create window: " << SDL_GetError() << std::endl;
        return 1;
    }


    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    if(glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW\n";
        return -1;
    }

    bool running = true;

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    CreateVertexBuffer();

    CompileShaders();

    

    while(running)
    {
        // input loop
        while(SDL_PollEvent(&event) != 0)
        {
            if(event.type == SDL_QUIT)
            {
                running = false;
            }
            else if(event.type == SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        running = false;
                    default: 
                        break;
                }
            }
        }

        Render();
        SDL_GL_SwapWindow(window);
    }

    // clean up
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}
