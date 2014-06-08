#include <iostream>
#include <GL/glew.h>

#include "misc.hpp"

void check_for_opengl_errors() {
    switch (glGetError()) {
    case GL_INVALID_ENUM: std::cout << "GLenum argument out of range" << std::endl; break;
    case GL_INVALID_VALUE: std::cout << "Numeric argument out of range" << std::endl; break;
    case GL_INVALID_OPERATION: std::cout << "Operation illegal in current state" << std::endl; break;
    case GL_STACK_OVERFLOW: std::cout << "Command would cause a stack overflow" << std::endl; break;
    case GL_STACK_UNDERFLOW: std::cout << "Command would cause a stack underflow" << std::endl; break;
    case GL_OUT_OF_MEMORY: std::cout << "Not enough memory left to execute command" << std::endl; break;
    }
}
