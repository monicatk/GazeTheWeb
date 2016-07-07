//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Quad to render with OpenGL.

#ifndef SHADER_H_
#define SHADER_H_

#include "externals/OGL/gl_core_3_3.h"
#include "submodules/glm/glm/glm.hpp"
#include <string>

class Shader
{
public:

    // Constructor
    Shader(std::string vertSource, std::string fragSource);

    // Destructor
    virtual ~Shader();

    // Bind shader program
    void Bind() const;

    // Update values in shader. Bind before updating!
    void UpdateValue(std::string name, const int& rValue) const;
    void UpdateValue(std::string name, const float& rValue) const;
    void UpdateValue(std::string name, const glm::vec2& rValue) const;
    void UpdateValue(std::string name, const glm::vec3& rValue) const;
    void UpdateValue(std::string name, const glm::vec4& rValue) const;
    void UpdateValue(std::string name, const glm::mat4& rValue) const;

    // Get program handle
    GLuint getProgram() const { return _program; }

private:

    // Handle
    GLuint _program = 0;
};


#endif // SHADER_H_
