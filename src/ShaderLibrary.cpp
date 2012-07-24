//-----------------------------------------------------------------------------
// ShaderLibrary.cpp
//
// Author:Ian Stewart
// 08/2008
//
//-----------------------------------------------------------------------------


#include "ShaderLibrary.h"
#include "TextureLibrary.h"

#include <stdio.h>
#include <string>
#include <GL/glut.h>

using namespace std;

ShaderLibrary* ShaderLibrary::inst = 0;

//-----------------------------------------------------------------------------
// Public methods
ShaderLibrary* ShaderLibrary::getInstance()
{
    if (inst == 0)
        inst = new ShaderLibrary();

    return inst;
}

bool ShaderLibrary::attachShader(ShaderType t)
{
    if (!enableShaders)
        t = SHADER_FIXED;

    lastAttatch = t;

    if (t == SHADER_FIXED)
    {
        glUseProgram(0);
        return true;
    }

    // valid shader type
    if (t < 0 || t > SHADER_FIXED)
        return false;

    if (!shader[t]->attach())
        return false;

    // setup shader variables
    if (t == SHADER_CLOUD)
    {
        TextureLibrary::getInstance()->Activate(TEXTURE_3D_NOISE);

        //GLint LightPos = glGetUniformLocation(shaderProg, "LightPos");
        GLint Scale = glGetUniformLocation(shader[t]->shaderProg, "Scale");
        GLint SkyColor = glGetUniformLocation(shader[t]->shaderProg, "SkyColor");
        GLint CloudColor = glGetUniformLocation(shader[t]->shaderProg, "CloudColor");
        GLint Noise = glGetUniformLocation(shader[t]->shaderProg, "Noise");
        GLint timeFromInit = glGetUniformLocation(shader[t]->shaderProg, "TIME_FROM_INIT");

        //glUniform3f(LightPos, 0,4,4);
        glUniform1f(Scale, .0005);
        glUniform3f(SkyColor, 0, 0, 0.8);
        glUniform3f(CloudColor, 0.8, 0.8, 0.8);
        glUniform1i(timeFromInit, glutGet(GLUT_ELAPSED_TIME));

    }

    if (t == SHADER_TOON_V2 || SHADER_TOON_COLOR || SHADER_TOON_TEXTURE)
    {

        // apply the texture
        glActiveTexture(GL_TEXTURE1);
        TextureLibrary::getInstance()->Activate(TEXTURE_CELL_BRIGHTNESS);
        glActiveTexture(GL_TEXTURE0);

        GLint cel = glGetUniformLocation(shader[t]->shaderProg, "cel");

        glUniform1i(cel, 1);
    }

    return shader[t]->attach();
}

//void ShaderLibrary::RegisterTextures(int cnt)
//{
//    if (lastAttatch == SHADER_TOON_TEXTURE)
//    {
//        
//        for (int i = 0; i < cnt; i++)
//        {
//            string var = "tex";
//            char idx[] = "0";
//            GLint loc = glGetUniformLocation(shader[lastAttatch]->shaderProg, var.append(idx+i).c_str());
//
//            glUniform1i(loc, i+1);
//        }
//    }
//    else
//    {
//        printf("ShaderLibrary ERROR: Cannot support multitexture for current setup!\n");
//    }
//
//}


//-----------------------------------------------------------------------------
// Protected methods
ShaderLibrary::ShaderLibrary()
{
    lastAttatch = SHADER_TEST;
    memset(shader, 0, SHADER_FIXED*sizeof(Shader*));

    GLenum err = glewInit();

    if (err != GLEW_OK || !glewIsSupported("GL_VERSION_2_0"))
    {
        printf("OpenGL 2.0 not supported. No shaders!\n");
        printf("%s\n", glewGetErrorString(err));
        printf("%s\n", (char*)glGetString( GL_VERSION ) );

        enableShaders = false;
        return;
    }

    enableShaders = true;
    printf("OpenGL 2.0 supported.\n");
    init();
}

void ShaderLibrary::init()
{
    lastAttatch = SHADER_TEST;
    for ( int i = SHADER_TEST; i <= SHADER_FIXED; i++ )
    {
        shader[i] = loadShader((ShaderType)(i));
    }
}

Shader* ShaderLibrary::loadShader(ShaderType t)
{
    if (!enableShaders)
        return 0;

    bool success = true;
    char *name;
    switch(t)
    {
        // blank shader, do not use
        case SHADER_TEST:
            name = "SHADER_TEST";
            shader[t] = 0;
            break;
        // toon shader v1
        case SHADER_TOON_V1:
            name = "SHADER_TOON_V1";
            shader[t] = new Shader();
            success &= shader[t]->addVert(((string)SHADER_LIB).append("toon_v1.vert").c_str());
            success &= shader[t]->addFrag(((string)SHADER_LIB).append("toon_v1.frag").c_str());
            success &= shader[t]->build();
            break;
        case SHADER_TOON_V2:
        case SHADER_TOON_COLOR:
            name = "SHADER_TOON_V2";
            shader[t] = new Shader();
            success &= shader[t]->addVert(((string)SHADER_LIB).append("toon_v2.vert").c_str());
            success &= shader[t]->addFrag(((string)SHADER_LIB).append("toon_v2_color.frag").c_str());
            success &= shader[t]->build();
            break;
        case SHADER_TOON_TEXTURE:
            name = "SHADER_TOON_TEXTURE";
            shader[t] = new Shader();
            success &= shader[t]->addVert(((string)SHADER_LIB).append("toon_v2.vert").c_str());
            success &= shader[t]->addFrag(((string)SHADER_LIB).append("toon_v2_tex.frag").c_str());
            success &= shader[t]->build();
            break;
        // cloud shader
        case SHADER_CLOUD:
            name = "SHADER_CLOUD";
            shader[t] = new Shader();
            success &= shader[t]->addVert(((string)SHADER_LIB).append("clouds.vert").c_str());
            success &= shader[t]->addFrag(((string)SHADER_LIB).append("clouds.frag").c_str());
            success &= shader[t]->build();
            break;
        // fixed pipeline functionality
        case SHADER_FIXED:
            name = "SHADER_FIXED";
            break;
    }

    if (success)
        return shader[t];
    else
    {
        printf("ShaderLibrary ERROR: Could not load shader %s!\n", name);
        return 0;
    }
}