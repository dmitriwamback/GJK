//
//  shader.h
//  GJK
//
//  Created by Dmitri Wamback on 2025-01-02.
//

#ifndef shader_h
#define shader_h

namespace gjk::core {

class Shader {
public:
    static Shader Create(const char* shaderFolderPath);
    void Use();
    void SetMatrix4(const char* variableName, glm::mat4& mat);
    void SetVector3(const char* variableName, glm::vec3 vec);
private:
    static void CompileShader(int shader, const char* source);
    static void PrintShaderLog(int shader);
    static int LoadShaderSource(const char* shaderPath, int shaderType);
    uint32_t program;
};

Shader Shader::Create(const char* shaderFolderPath) {
    Shader shader = Shader();
    
    std::string vsSrc = (std::string(shaderFolderPath) + "/vMain.glsl");
    std::string fsSrc = (std::string(shaderFolderPath) + "/fMain.glsl");
        
    const char* vertexShaderPath = vsSrc.c_str();
    const char* fragmentShaderPath = fsSrc.c_str();
            
    int vert = Shader::LoadShaderSource(vertexShaderPath, GL_VERTEX_SHADER);
    int frag = Shader::LoadShaderSource(fragmentShaderPath, GL_FRAGMENT_SHADER);
    
    shader.program = glCreateProgram();
    glAttachShader(shader.program, vert);
    glAttachShader(shader.program, frag);
    glLinkProgram(shader.program);
    glDeleteShader(vert);
    glDeleteShader(frag);
    
    return shader;
}

int Shader::LoadShaderSource(const char* shaderPath, int shaderType) {
    
    std::ifstream shader;
    shader.open(shaderPath);
    
    std::stringstream stream;
    stream << shader.rdbuf();
    shader.close();
    
    std::string shaderSourceStr = stream.str();
    const char* shaderSourceConstChar = shaderSourceStr.c_str();
    
    int shaderProgram = glCreateShader(shaderType);
    Shader::CompileShader(shaderProgram, shaderSourceConstChar);
    Shader::PrintShaderLog(shaderProgram);
    
    return shaderProgram;
}

void Shader::CompileShader(int shader, const char* source) {
    
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
}

void Shader::PrintShaderLog(int shader) {
    
    int success;
    char infoLog[1024];
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << infoLog << '\n';
    }
    else {
        std::cout << "successfully compiled shader\n";
    }
}

void Shader::Use() {
    glUseProgram(program);
}

void Shader::SetMatrix4(const char* variableName, glm::mat4& mat) {
    int location = glGetUniformLocation(program, variableName);
    glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetVector3(const char* variableName, glm::vec3 vec) {
    int location = glGetUniformLocation(program, variableName);
    glUniform3fv(location, 1, &vec[0]);
}

}

#endif /* shader_h */
