//-----------------------------------------------------------------------------
// TextureLibrary.cpp
//
// Author:Ian Stewart
// 08/2008
//
// Ideas from Orange Book
//-----------------------------------------------------------------------------
#include "TextureLibrary.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <string.h>
#include "util/Image.h"

using namespace std;

void CreateNoise3D();
TextureLibrary* TextureLibrary::inst = 0;

// Magic Numbers from "Paul's Projects"
GLubyte celBrightnessData[16]= {127, 127, 127, 191, 191, 191, 191, 191,
                                255, 255, 255, 255, 255, 255, 255, 255};


TextureLibrary* TextureLibrary::getInstance()
{
    if (!inst)
        inst = new TextureLibrary();
    return inst;
}

TextureLibrary::TextureLibrary()
{
    memset(textures, 0, sizeof(textures));
    useMipmap = true;
    currentTexID = TEXTURE_DISABLE+1;
}

unsigned TextureLibrary::Load(std::string filename)
{
    for (int i = 0; i < MAX_TEXTURE_COUNT; i++)
    {
        Texture& tex = textures[i];
        if (tex.filename == filename)
        {
            return i;
        }
    }

    if (currentTexID >= MAX_TEXTURE_COUNT)
    {
        printf("TextureLibrary: ERROR: Max number of textures exceeded!\n");
        return 0;
    }

    Texture& texture = textures[currentTexID];

    if (texture.glid)
    {
        printf("TextureLibrary ERROR: Texture already generated!?\n");
        return 0;
    }

    glEnable(GL_TEXTURE_2D);

    texture.filename = filename;

    glGenTextures(1, &texture.glid);
    glBindTexture(GL_TEXTURE_2D, texture.glid);

    SetFilter();
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (strstr(filename.c_str(), ".tga"))
    {
        Image *img = new Image();
        //string fName = filename;
        if (!img->LoadImage(filename.c_str()))
        {
            printf("ERROR: Cannot load image: %s\n", filename);
            return 0;
        }

        texture.width = img->GetWidth();
        texture.height = img->GetHeight();

        if (useMipmap)
            //glGenerateMipmapEXT(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

        // TODO: Had to change GL_RGBA to GL_RGB to fix texture bug?
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height,
                        0, GL_RGBA, GL_UNSIGNED_BYTE, img->GetData());
    }
    else
    {
       printf("TextureLibrary ERROR: Cannot load filetype: %s\n", filename.c_str());
       return currentTexID++;
    }

    return currentTexID++;
}

unsigned TextureLibrary::getGLID(unsigned id) const
{
    return textures[id].glid;
}

//unsigned int TextureLibrary::RequestID()
//{
//    if (currentTexID >= MAX_TEXTURE_COUNT)
//    {
//        printf("TextureLibrary ERROR: Could not request ID. Max textures reached!\n");
//        return 0;
//    }
//    return currentTexID++;
//}

void TextureLibrary::Deactivate(unsigned id)
{
    if (id == TEXTURE_3D_NOISE)
        glDisable(GL_TEXTURE_3D);
    else
        glDisable(GL_TEXTURE_2D);
}

void TextureLibrary::Activate(unsigned id)
{
    if (id == TEXTURE_DISABLE || id == TEXTURE_NULL)
    {
        glDisable(GL_TEXTURE_1D);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_3D);
        return;
    }

    if (id >= MAX_TEXTURE_COUNT)
    {
        printf("TextureLibrary: Invalid texture ID! %d\n", id);
        return;
    }

    Texture& texture = textures[id];

    // GL_MODULATE is the default mode
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    if (id == TEXTURE_3D_NOISE)
    {
        glEnable(GL_TEXTURE_3D);

        if (texture.glid)
        {
            glBindTexture(GL_TEXTURE_3D, texture.glid);
            return;
        }

        glGenTextures(1, &texture.glid);
        glBindTexture(GL_TEXTURE_3D, texture.glid);
        //SetFilter();

        CreateNoise3D();
        return;
    }
    else if (id == TEXTURE_CELL_BRIGHTNESS)
    {
        //glEnable(GL_TEXTURE_1D);
        if (texture.glid)
        {
            glBindTexture(GL_TEXTURE_1D, texture.glid);
            return;
        }

        glGenTextures(1, &texture.glid);
        glBindTexture(GL_TEXTURE_1D, texture.glid);
        glTexImage1D(   GL_TEXTURE_1D, 0, GL_RGBA, 16,
                        0, GL_LUMINANCE, GL_UNSIGNED_BYTE, celBrightnessData);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);

        return;
    }

    glEnable(GL_TEXTURE_2D);

    // use the texture if already initialized
    if (texture.glid)
    {
        glBindTexture(GL_TEXTURE_2D, texture.glid);
        return;
    }
    else
    {
        printf("TextureLibrary ERROR: Texture has not been loaded!\n");
        return;
    }

    //string textureString;
    //if (id <= TEXTURE_DISABLE)
    //    textureString = TextureNames[id];
    //else
    //    textureString = id;


    //glGenTextures(1, &texture.glid);
    //glBindTexture(GL_TEXTURE_2D, texture.glid);

    //SetFilter();
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //if (strstr(textureString.c_str(), ".tga"))
    //{
    //    Image *img = new Image();
    //    if (!img->LoadImage(textureString.insert(0, TEXTURE_LIB).c_str()))
    //    {
    //        printf("ERROR: Cannot load image: %s\n", textureString);
    //        return;
    //    }

    //    texture.width = img->GetWidth();
    //    texture.height = img->GetHeight();

    //    if (useMipmap)
    //        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height,
    //                    0, GL_RGBA, GL_UNSIGNED_BYTE, img->GetData());
    //}
    //else
    //{
    //    printf("TextureLibrary ERROR: Cannot load filetype: %s\n", textureString);
    //    return;
    //}

    //return;
}

void TextureLibrary::SetFilter()
{
    if (useMipmap)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
}