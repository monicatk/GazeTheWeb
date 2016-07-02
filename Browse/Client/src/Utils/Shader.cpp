//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Shader.h"
#include "submodules/GLM/glm/gtc/type_ptr.hpp"

Shader::Shader(std::string vertSource, std::string fragSource)
{
    // Vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    char const * pVertSource = vertSource.c_str();
    glShaderSource(vertexShader, 1, &pVertSource, NULL);
    glCompileShader(vertexShader);

    // Fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char const * pFragSource = fragSource.c_str();
    glShaderSource(fragmentShader, 1, &pFragSource, NULL);
    glCompileShader(fragmentShader);

    // Create program
    _program = glCreateProgram();
    glAttachShader(_program, vertexShader);
    glAttachShader(_program, fragmentShader);
    glLinkProgram(_program);

    // Delete shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
    glDeleteProgram(_program);
}

void Shader::Bind() const
{
    glUseProgram(_program);
}

void Shader::UpdateValue(std::string name, const int& rValue) const
{
    glUniform1i(glGetUniformLocation(_program, name.c_str()), rValue);
}

void Shader::UpdateValue(std::string name, const float& rValue) const
{
    glUniform1f(glGetUniformLocation(_program, name.c_str()), rValue);
}

void Shader::UpdateValue(std::string name, const glm::vec2& rValue) const
{
    glUniform2fv(glGetUniformLocation(_program, name.c_str()), 1, glm::value_ptr(rValue));
}

void Shader::UpdateValue(std::string name, const glm::vec3& rValue) const
{
    glUniform3fv(glGetUniformLocation(_program, name.c_str()), 1, glm::value_ptr(rValue));
}

void Shader::UpdateValue(std::string name, const glm::vec4& rValue) const
{
    glUniform4fv(glGetUniformLocation(_program, name.c_str()), 1, glm::value_ptr(rValue));
}

void Shader::UpdateValue(std::string name, const glm::mat4& rValue) const
{
    glUniformMatrix4fv(glGetUniformLocation(_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(rValue));
}
