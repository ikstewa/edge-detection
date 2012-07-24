#include "Shader.h"
#include "util/textfile.h"
#include <stdlib.h>
#include <stdio.h>

Shader::Shader()
{
    shaderProg = 0;
}

bool Shader::attach()
{
    if (shaderProg)
        glUseProgram(shaderProg);
    else
        return false;
    return true;
}

bool Shader::addVert(const char * filename)
{
    // load the Vertex shader from file
    char *vs = 0;
    GLuint v = glCreateShader(GL_VERTEX_SHADER);

    vs = textFileRead(filename);
    if (!vs)
        return false;
    const char * vv = vs;

    glShaderSource(v, 1, &vv, 0);

    free(vs);

    glCompileShader(v);

    if (!shaderProg)
        shaderProg = glCreateProgram();
    glAttachShader(shaderProg, v);

    // print debug info
    printShaderInfoLog(v);
    return true;
}

bool Shader::addFrag(const char * filename)
{
    // no filename, use fixed functionality
    if (!filename)
    {
        if (!shaderProg)
            shaderProg = glCreateProgram();
        return true;
    }

    char *fs = NULL;
    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);

    fs = textFileRead(filename);
    if (!fs)
        return false;
    const char * ff = fs;

    glShaderSource(f, 1, &ff, NULL);

    free(fs);

    glCompileShader(f);

    if (!shaderProg)
        shaderProg = glCreateProgram();

    glAttachShader(shaderProg, f);

    // print debug info
    printShaderInfoLog(f);
    return true;
}

bool Shader::build()
{
    glLinkProgram(shaderProg);
    return true;
}

void Shader::printShaderInfoLog(GLuint s)
{
   int infoLogLength = 0;
   int charsWritten = 0;
   char *infoLog;

   glGetShaderiv(s, GL_INFO_LOG_LENGTH, &infoLogLength);

   if (infoLogLength > 0)
   {
      infoLog = (char*)malloc(infoLogLength);
      glGetShaderInfoLog(s, infoLogLength, &charsWritten, infoLog);
      printf("%s\n", infoLog);
      free(infoLog);
   }
}

void Shader::printProgramInfoLog()
{
   int infoLogLength = 0;
   int charsWritten  = 0;
   char *infoLog;

   glGetProgramiv(shaderProg, GL_INFO_LOG_LENGTH, &infoLogLength);

   if (infoLogLength > 0)
   {
      infoLog = (char *)malloc(infoLogLength);
      glGetProgramInfoLog(shaderProg, infoLogLength, &charsWritten, infoLog);
      printf("%s\n", infoLog);
      free(infoLog);
   }
}
