//-----------------------------------------------------------------------------
// TextureLibrary.h
//
// Author:Ian Stewart
// 08/2008
//
// Ideas from Orange Book
//-----------------------------------------------------------------------------

#ifndef __TEXTURE_LIBRARY_H__
#define __TEXTURE_LIBRARY_H__

#include <string>

using namespace std;

//const char* TEXTURE_LIB ".//game//data//textures//";
//#define TEXTURE_LIB ".//game//data//textures//"

#define MAX_TEXTURE_COUNT 128




enum TextureId {
    TEXTURE_NULL = 0,
    TEXTURE_CELL_BRIGHTNESS,
    TEXTURE_3D_NOISE,
    TEXTURE_DISABLE
};

static const string TextureNames[TEXTURE_DISABLE] = 
{
    // External Textures
    "NULL",

    // Internal Textures
    "Cell Brightness",
    "3D Noise"
};

struct Texture {
    unsigned glid;
    //bool alpha;
    unsigned width;
    unsigned height;
    string filename;
};

class TextureLibrary {

  public:

    static TextureLibrary* getInstance();

    //void Init();
    void Activate(unsigned id);
    void Deactivate(unsigned id);
    unsigned int Load(std::string filename);
    //bool HasAlpha(TextureId id) const { return textures[id].alpha; }

    unsigned getGLID(unsigned) const;

    void DisableMipmaps() { useMipmap = false; }
    void EnableMipmaps() { useMipmap = true; }


  private:

    TextureLibrary();
    void SetFilter();

    Texture textures[MAX_TEXTURE_COUNT];
    bool useMipmap;

    static TextureLibrary* inst;
    //static string Textures[TEXTURE_DISABLE];

    // array index for the external textures
    unsigned currentTexID;
};

#endif