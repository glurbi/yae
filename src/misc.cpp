#include <iostream>
#include <GL/glew.h>
#include <SDL.h>

#include "misc.hpp"

SDL_Window* yae::create_simple_window()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_Window* win = SDL_CreateWindow("GLEW Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    SDL_GLContext ctx = SDL_GL_CreateContext(win);
    glewInit();
    glViewport(0, 0, 800, 600);
    return win;
}

void yae::check_for_opengl_errors()
{
    switch (glGetError()) {
    case GL_INVALID_ENUM: std::cout << "GLenum argument out of range" << std::endl; break;
    case GL_INVALID_VALUE: std::cout << "Numeric argument out of range" << std::endl; break;
    case GL_INVALID_OPERATION: std::cout << "Operation illegal in current state" << std::endl; break;
    case GL_STACK_OVERFLOW: std::cout << "Command would cause a stack overflow" << std::endl; break;
    case GL_STACK_UNDERFLOW: std::cout << "Command would cause a stack underflow" << std::endl; break;
    case GL_OUT_OF_MEMORY: std::cout << "Not enough memory left to execute command" << std::endl; break;
    }
}
