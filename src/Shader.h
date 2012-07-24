#ifndef __SHADER_H__
#define __SHADER_H__

#include <stdlib.h>
#include <GL/glew.h>

class Shader
{

public:
    Shader();

    virtual bool attach();

    bool addVert(const char*);
    bool addFrag(const char*);
    bool build();

    void printProgramInfoLog();
    void printShaderInfoLog(GLuint);


    GLuint shaderProg;


};

#endif