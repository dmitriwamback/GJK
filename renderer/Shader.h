//
// Created by Dmitri on 2026-10-23.
//

#ifndef GJK_SHADER_H
#define GJK_SHADER_H

#include <glm/ext/matrix_float4x4.hpp>
#include <cstdint>

class Shader {
public:
    Shader();
    Shader(const char* vertexShaderSource, const char* fragmentShaderSource);
    void Use();
    void SetMatrix4(const char* variableName, const glm::mat4& matrix);
    void SetVector3(const char* variableName, const glm::vec3& vector);

private:
    uint32_t program;
    int LoadShaderSource(const char* source, int type);
};


#endif //GJK_SHADER_H
