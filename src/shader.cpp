#include "shader.h"

Shader::Shader(const char* vs_path, const char* fs_path)
{
    std::string vs = ParseShader(vs_path);
    std::string fs = ParseShader(fs_path);
    m_ID = CreateShader(vs, fs);
}

Shader::~Shader()
{
    glDeleteProgram(m_ID); GLCall;
    m_ID = 0;
}

std::string Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath); // opens file

    std::string line;
    std::stringstream ss;

    while (getline(stream, line))
    {
        ss << line << "\n";
    }

    return ss.str();
}

uint Shader::CompileShader(uint type, const std::string& source)
{
    uint id = glCreateShader(type); GLCall;
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, NULL); GLCall;
    glCompileShader(id); GLCall;

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); GLCall;
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); GLCall;
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message); GLCall;
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader." << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id); GLCall;

        return 0;
    }

    std::cout << "Successfully compiled " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader." << std::endl;

    return id;
}

uint Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    // create program for shaders to link to
    uint programID = glCreateProgram(); GLCall;
    uint vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    uint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // link shaders into one program
    glAttachShader(programID, vs); GLCall;
    glAttachShader(programID, fs); GLCall;
    glLinkProgram(programID); GLCall;
    glValidateProgram(programID); GLCall;

    int result;
    glGetProgramiv(programID, GL_LINK_STATUS, &result); GLCall;
    if (result == GL_FALSE)
    {
        int length;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length); GLCall;
        char* message = (char*)alloca(length * sizeof(char));
        glGetProgramInfoLog(programID, length, &length, message); GLCall;
        std::cout << "Failed to link shaders :: " << message << std::endl;
        glDeleteProgram(programID); GLCall;

        return 0;
    }

    std::cout << "Successfully linked shaders" << std::endl; 

    glDeleteShader(vs); GLCall;
    glDeleteShader(fs); GLCall;

    return programID;
}

void Shader::Bind() const
{
    glUseProgram(m_ID); GLCall;
}

void Shader::Unbind() const
{
    glUseProgram(0); GLCall;
}

void Shader::SetUniformInt(const std::string& name, int val)
{
    glUniform1i(GetUniformLocation(name), val); GLCall;
}

void Shader::SetUniformFloat(const std::string& name, float val)
{
    glUniform1f(GetUniformLocation(name), val); GLCall;
}

void Shader::SetUniformVec2(const std::string& name, float val0, float val1)
{
    glUniform2f(GetUniformLocation(name), val0, val1); GLCall;
}

void Shader::SetUniformVec3(const std::string& name, float val0, float val1, float val2)
{
    glUniform3f(GetUniformLocation(name), val0, val1, val2); GLCall;
}

void Shader::SetUniformVec4(const std::string& name, float val0, float val1, float val2, float val3)
{
    glUniform4f(GetUniformLocation(name), val0, val1, val2, val3); GLCall;
}

void Shader::SetUniformMat4(const std::string& name, const glm::mat4& matrix)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix)); GLCall;
}

uint Shader::GetUniformLocation(const std::string &name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    uint location = glGetUniformLocation(m_ID, name.c_str()); GLCall;
    if (location == -1)
        std::cout << "[Warning] Uniform '" << name << "' doesn't exist!" << std::endl;
    
    m_UniformLocationCache[name] = location;

    return location;
}
