#include "COpenGLShader.h"

CResourceManager<COpenGLShader> gShaderResourceMgr;

void COpenGLShader::LoadShaders(std::string vsFilepath, std::string fsFilepath, std::string gsFilepath) {

    long vertexShaderSize = 0;
    long fragmentShaderSize = 0;
    long geometryShaderSize = 0;

    uint8_t* vertexShaderData = ReadEntireFile(vsFilepath, &vertexShaderSize, false);
    uint8_t* fragmentShaderData = ReadEntireFile(fsFilepath, &fragmentShaderSize, false);
    uint8_t* geometryShaderData = ReadEntireFile(gsFilepath, &geometryShaderSize, false);

    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint geometryShaderId = glCreateShader(GL_GEOMETRY_SHADER);

    GLuint programId = glCreateProgram();

    if (!vertexShaderData) {
        ABORT_EXIT("Shader must at least have a vertex shader! (%s)\n", vsFilepath.c_str());
    }

    if (vertexShaderData) {
        glShaderSource(vertexShaderId, 1, reinterpret_cast<const GLchar* const*>(&vertexShaderData), NULL);
        glCompileShader(vertexShaderId);

        GLint infoLogLen;
        glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            char* shaderError = new char[infoLogLen];
            glGetShaderInfoLog(vertexShaderId, infoLogLen, NULL, shaderError);
            ABORT_EXIT("Error while compiling vertex shader: %s\n\n%s\n", vsFilepath.c_str(), shaderError);
        }

        glAttachShader(programId, vertexShaderId);
    }

    if (fragmentShaderData) {
        glShaderSource(fragmentShaderId, 1, reinterpret_cast<const GLchar* const*>(&fragmentShaderData), NULL);
        glCompileShader(fragmentShaderId);

        GLint infoLogLen;
        glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            char* shaderError = new char[infoLogLen];
            glGetShaderInfoLog(fragmentShaderId, infoLogLen, NULL, shaderError);
            ABORT_EXIT("Error while compiling fragment shader: %s\n\n%s\n", fsFilepath.c_str(), shaderError);
        }

        glAttachShader(programId, fragmentShaderId);
    }

    if (geometryShaderData) {
        glShaderSource(geometryShaderId, 1, reinterpret_cast<const GLchar* const*>(&geometryShaderData), NULL);
        glCompileShader(geometryShaderId);

        GLint infoLogLen;
        glGetShaderiv(geometryShaderId, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            char* shaderError = new char[infoLogLen];
            glGetShaderInfoLog(geometryShaderId, infoLogLen, NULL, shaderError);
            ABORT_EXIT("Error while compiling geometry shader: %s\n\n%s\n", gsFilepath.c_str(), shaderError);
        }

        glAttachShader(programId, geometryShaderId);
    }

    glLinkProgram(programId);

    glDetachShader(programId, vertexShaderId);
    glDetachShader(programId, fragmentShaderId);
    glDetachShader(programId, geometryShaderId);

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
    glDeleteShader(geometryShaderId);

    GLenum type;
    GLsizei length;
    GLint count, size;
    GLchar name[32];

    // Read all uniforms in the program
    glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &count);
    for (GLint i = 0; i < count; i++) {
        glGetActiveUniform(programId, i, 32, &length, &size, &type, name);
        int loc = glGetUniformLocation(programId, name);
        if (loc < 0) {
            ABORT_EXIT("Uniform %s doesn't exist!\nVS: %s\nFS: %s\nGS: %s\n\n", name, vsFilepath.c_str(),
                       fsFilepath.c_str(), gsFilepath.c_str());
        }
        this->locations.insert(std::make_pair(name, loc));
    }

    this->program_id = programId;

    if (vertexShaderData)
        delete vertexShaderData;
    if (fragmentShaderData)
        delete fragmentShaderData;
    if (geometryShaderData)
        delete geometryShaderData;
}