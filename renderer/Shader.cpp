//
// Created by Dmitri on 2026-10-23.
//

#include "Shader.h"

#include <iostream>
#include <ostream>
#include <GL/glew.h>

Shader::Shader() {
    program = 0;
}

Shader::Shader(const char* vertexShaderSource, const char* fragmentShaderSource) {

    int vertex = LoadShaderSource(vertexShaderSource, GL_VERTEX_SHADER);
    int fragment = LoadShaderSource(fragmentShaderSource, GL_FRAGMENT_SHADER);

    program = glCreateProgram();

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
}

int Shader::LoadShaderSource(const char *source, int type) {
    int shaderProgram = glCreateShader(type);
    glShaderSource(shaderProgram, 1, &source, nullptr);
    glCompileShader(shaderProgram);

    int success;
    char infoLog[1024];

    glGetShaderiv(shaderProgram, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderProgram, 1024, nullptr, infoLog);
        std::cout << infoLog << std::endl;
    }

    return shaderProgram;
}

void Shader::Use() {
    glUseProgram(program);
}

void Shader::SetMatrix4(const char *variableName, const glm::mat4 &matrix) {
    int location = glGetUniformLocation(program, variableName);
    glUniformMatrix4fv(location, 1, 0, &matrix[0][0]);
}

void Shader::SetVector3(const char *variableName, const glm::vec3 &vector) {
    int location = glGetUniformLocation(program, variableName);
    glUniform3fv(location, 1, &vector[0]);
}