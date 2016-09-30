#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

SDL_Window* window;

std::string readFile(const char* filePath)
{
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if( !fileStream.is_open() ) 
    {
        std::cerr << "Could not read file " << filePath << ". File does not exist\n";
        return "";
    }

    std::string line = "";
    while( !fileStream.eof() ) 
    {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    std::cout << filePath << " read!\n";
    return content;
}

GLuint loadShader(const char* vertex_path, 
                  const char* fragment_path)
{
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    //read shaders
    std::string vertShaderStr = readFile(vertex_path);
    std::string fragShaderStr = readFile(fragment_path);
    const char *vertShaderSrc = vertShaderStr.c_str();
    const char *fragShaderSrc = fragShaderStr.c_str();

    GLint result = GL_FALSE;
    int logLength;

    //compile vertex shader
    std::cout << "Compiling vertex shader\n";
    glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
    glCompileShader(vertShader);

    //check vertex shader
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> vertShaderError(logLength);
    glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
    std::cout << &vertShaderError[0] << "\n";
    
    //compile fragment shader
    std::cout << "Compiling fragment shader\n";
    glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
    glCompileShader(fragShader);

    //check fragment shader
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, & result);
    std::vector<char> fragShaderError(logLength);
    glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
    std::cout << &fragShaderError[0] << "\n";

    std::cout << "Linking program\n";
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> programError((logLength > 1) ? logLength : 1);
    glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
    std::cout << &programError[0] << "\n";

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;

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

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // create the VAO, load programs (which I haven't made yet)
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint programID = loadShader("../../shaders/vertshader.glsl", "../../shaders/fragshader.glsl");

    bool running = true;

    // 3 vertices of the triangle
    static const GLfloat g_vertex_buffer_data[] =
    {
        -0.75f, -0.75f, 0.0f,
        0.75f, -0.75f, 0.0f,
        0.0f, 0.75f, 0.75f,
    };

    // make and bind the vertexbuffer
    GLuint VertexBuffer;
    glGenBuffers(1, &VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data),
                 g_vertex_buffer_data, GL_STATIC_DRAW);
    
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

        // render
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(programID);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                              0, (void*)0);
        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);
        SDL_GL_SwapWindow(window);
    }

    // clean up
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}