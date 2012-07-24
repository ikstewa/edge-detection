#include <stdio.h>
#include <string>
#include <GL/glew.h>
#include <GL/glut.h>
#include "TextureLibrary.h"
#include "Shader.h"

int sWidth = 800;
int sHeight = 600;

Shader mrtShader;
Shader edgeShader;
GLuint fbo;
GLuint depthBuffer;
GLuint imgTexture, outlineTexture;
GLuint depthTexture, normalTexture;

const int texWidth = 512;
const int texHeight = 512;

float threshold_d = 0.009;
float threshold_n = 0.04;
bool enable_depth = true;
bool enable_norm = true;

GLfloat xrot = 45;
GLfloat yrot = 45;
GLfloat xspeed = 0.01;
GLfloat yspeed = 0.02;


void init();
void initFBO();
void renderTextures();
void display();
void reshape(int, int);
void keyboard(unsigned char, int, int);
void idle();

// fps and text
void drawText(int, int, void*, std::string);
void fps();

void init()
{
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    GLenum status = glewInit();
    if (status != GLEW_OK)
        printf("ERROR: OpenGL 2.0 not supported!\n");
    else
        printf("OpenGL 2.0 supported!\n");

    reshape(sWidth, sHeight);
}

void initFBO()
{
   // grab the texture library
   TextureLibrary *texLib = TextureLibrary::getInstance();
   texLib->DisableMipmaps();

   GLint maxBuffers;
   glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxBuffers);
   printf("MAX_COLOR_ATTACHMENTS: %d\n", maxBuffers);

   // setup the FBO
   glGenFramebuffersEXT(1, &fbo);
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

   // create the render buffer for depth
   glGenRenderbuffersEXT(1, &depthBuffer);
   glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
   glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, texWidth, texHeight);
   // bind the render buffer
   glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBuffer);

   // IMAGE TEXTURE
   imgTexture = texLib->Load("blah");
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texLib->getGLID(imgTexture), 0);

   // DEPTH TEXTURE
   depthTexture = texLib->Load("blah1");
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, texLib->getGLID(depthTexture), 0);

   // NORMAL TEXTURE
   normalTexture = texLib->Load("blah2");
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D, texLib->getGLID(normalTexture), 0);

   // define the render targets
   GLenum mrt[] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT};
   glDrawBuffers(3, mrt);

   // check FBO status
   GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
   if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
   {
      printf("ERROR: FBO status not complete!\n");
      exit(1);
   }

   // unbind the FBO
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

   // Initialize the shaders
   mrtShader.addVert("glsl/mrt.vs");
   mrtShader.addFrag("glsl/mrt.fs");
   mrtShader.build();
   mrtShader.printProgramInfoLog();

   //edgeShader.addVert("glsl/DepthEdge.vert");
   edgeShader.addVert("glsl/DepNormEdge.vert");
   //edgeShader.addFrag("glsl/DepthEdge.frag");
   edgeShader.addFrag("glsl/DepNormEdge.frag");
   edgeShader.build();
   edgeShader.printProgramInfoLog();
}

void renderTextures(GLuint fb)
{
   // bind the FBO for rendering
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);

   // setup the projection matix!!
   glPushAttrib(GL_VIEWPORT_BIT);
   glViewport(0, 0, texWidth, texHeight);
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluPerspective(45, (float)sWidth/(float)sHeight, 1.0, 5000.0);
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();


   // render to the FBO
   glClearColor( 0.0f, 0.0f, 0.0f, 0.5f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();

   glTranslatef(0.0f, 0.0f, -3.0f);
   glRotatef(xrot, 1.0f, 0.0f, 0.0f);
   glRotatef(yrot, 0.0f, 1.0f, 0.0f);

   // enable the shader program
   mrtShader.attach();

   glColor4f(0.0, 1.0f, 0.0f, 1.0f);
   glutSolidCone(1.0, 1.0, 10, 10);
   glTranslatef(0.7, 0.0, 0.0);
   glutSolidCube(1.0);
   glTranslatef(-1.4f, 0.0, 0.0);
   glutSolidSphere(0.5, 10, 10);

   /*
   // draw the cube
   glBegin(GL_QUADS);
      // front face
      glNormal3f( 0.0f, 0.0f, 1.0);
      glColor4f( 0.0f, 1.0f, 0.0f, 1.0f );
      glVertex3f(-0.5f, -0.5f,  0.5f);
      glVertex3f( 0.5f, -0.5f,  0.5f);
      glVertex3f( 0.5f,  0.5f,  0.5f);
      glVertex3f(-0.5f,  0.5f,  0.5f);
      // Back face
      glNormal3f( 0.0f, 0.0f,-1.0);
      glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f,  0.5f, -0.5f);
      glVertex3f( 0.5f,  0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f, -0.5f);
      // Top face
      glNormal3f( 0.0f, 1.0, 0.0f);
      glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
      glVertex3f(-0.5f,  0.5f, -0.5f);
      glVertex3f(-0.5f,  0.5f,  0.5f);
      glVertex3f( 0.5f,  0.5f,  0.5f);
      glVertex3f( 0.5f,  0.5f, -0.5f);
      // Bottom face
      glNormal3f( 0.0f,-1.0, 0.0f);
      glColor4f(0.0f,1.0f,1.0f,1.0f);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f,  0.5f);
      glVertex3f(-0.5f, -0.5f,  0.5f);
      // Right face
      glNormal3f( 1.0, 0.0f, 0.0f);
      glColor4f(1.0f,1.0f,0.0f,1.0f);
      glVertex3f( 0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f,  0.5f, -0.5f);
      glVertex3f( 0.5f,  0.5f,  0.5f);
      glVertex3f( 0.5f, -0.5f,  0.5f);
      // Left Face
      glNormal3f(-1.0, 0.0f, 0.0f);
      glColor4f(1.0f,1.0f,1.0f,1.0f);
      glVertex3f(-0.5, -0.5f, -0.5f);
      glVertex3f(-0.5, -0.5f,  0.5f);
      glVertex3f(-0.5,  0.5f,  0.5f);
      glVertex3f(-0.5,  0.5f, -0.5f);
   glEnd();
   //*/

   // disable the shader program
   glUseProgram(0);

   // restore old viewport and projection
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPopAttrib();

   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void display()
{

    renderTextures(fbo);


    TextureLibrary *texLib = TextureLibrary::getInstance();

    glClearColor(0.2, 0.2, 0.2, 0.5);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glColor4f(1.0,1.0,1.0,1.0);
    // DRAW TOP LEFT
    texLib->Activate(depthTexture);
    glBegin(GL_QUADS);

        //glNormal3f( 0.0f, 0.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-99.0, 1.0);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(-1.0, 1.0);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(-1.0, 99.0);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-99.0, 99.0);

    glEnd();
    texLib->Deactivate(depthTexture);

    // DRAW TOP RIGHT
    texLib->Activate(normalTexture);
    glBegin(GL_QUADS);

        //glNormal3f( 0.0f, 0.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(1.0, 1.0);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(99.0, 1.0);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(99.0, 99.0);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(1.0, 99.0);

    glEnd();
    texLib->Deactivate(normalTexture);

    // DRAW BOTTOM LEFT
    texLib->Activate(imgTexture);

    glBegin(GL_QUADS);

        //glNormal3f( 0.0f, 0.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-99.0, -99.0);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(-1.0, -99.0);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(-1.0, -1.0);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-99.0, -1.0);

    glEnd();

    texLib->Deactivate(imgTexture);


    // DRAW BOTTOM RIGHT
    edgeShader.attach();

    texLib->Activate(imgTexture);
    glActiveTexture(GL_TEXTURE1);
    if (enable_depth)
       texLib->Activate(depthTexture);
    glActiveTexture(GL_TEXTURE2);
    if (enable_norm)
       texLib->Activate(normalTexture);

    GLint wid = glGetUniformLocation(edgeShader.shaderProg, "width");
    GLint hei = glGetUniformLocation(edgeShader.shaderProg, "height");
    GLint thresh_d = glGetUniformLocation(edgeShader.shaderProg, "threshold_d");
    GLint thresh_n = glGetUniformLocation(edgeShader.shaderProg, "threshold_n");
    GLint img = glGetUniformLocation(edgeShader.shaderProg, "imgColor");
    GLint dep = glGetUniformLocation(edgeShader.shaderProg, "depColor");
    GLint norm = glGetUniformLocation(edgeShader.shaderProg, "normColor");
    GLint edgeC = glGetUniformLocation(edgeShader.shaderProg, "edgeColor");
    glUniform1f(wid, texWidth);
    glUniform1f(hei, texHeight);
    glUniform1f(thresh_d, threshold_d);
    glUniform1f(thresh_n, threshold_n);
    glUniform4f(edgeC, 1.0, 1.0, 1.0, 1.0);
    
    glUniform1i(img, 0);

    if (enable_depth)
       glUniform1i(dep, 1);
    else
       glUniform1i(dep, 3);
    if (enable_norm)
       glUniform1i(norm, 2);
    else
       glUniform1i(norm, 3);

    glBegin(GL_QUADS);

        //glNormal3f( 0.0f, 0.0f, -1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(1.0, -99.0);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(99.0, -99.0);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(99.0, -1.0);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(1.0, -1.0);

    glEnd();

    texLib->Deactivate(normalTexture);
    glActiveTexture(GL_TEXTURE1);
    texLib->Deactivate(depthTexture);
    glActiveTexture(GL_TEXTURE0);
    texLib->Deactivate(imgTexture);

    glUseProgram(0);

    glColor3f(1.0,1.0,0.0);
    fps();

    xrot += xspeed;
    yrot += yspeed;

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    sWidth = w;
    sHeight = h;

    glViewport( 0, 0, sWidth, sHeight );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-100, 100, -100, 100);
    /*if ( h == 0 )
        gluPerspective(45, (float)sWidth, 1.0, 5000.0);
    else
        gluPerspective(45, (float)sWidth/(float)sHeight, 1.0, 5000.0);*/
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard( unsigned char key, int x, int y )
{
    switch (key)
    {
        // ESC
        case 27:
            exit(0);
            break;
        case 'q':
           threshold_d += 0.0001;
           printf("thresh_d:%f\n", threshold_d);
           break;
        case 'w':
           threshold_n += 0.001;
           printf("thresh_n:%f\n", threshold_n);
           break;
        case 'a':
           threshold_d -= 0.0001;
           printf("thresh_d:%f\n", threshold_d);
           break;
        case 's':
           threshold_n -= 0.001;
           printf("thresh_n:%f\n", threshold_n);
           break;
        case 'z':
           enable_depth = !enable_depth;
           printf("enable_depth: %d\n", enable_depth);
           break;
        case 'x':
           enable_norm = !enable_norm;
           printf("enable_norm: %d\n", enable_norm);
           break;
        default:
            break;
    }
}

void idle()
{
    glutPostRedisplay();
}


int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
    glutInitWindowSize(sWidth, sHeight);
    glutCreateWindow( "Texture Library" );

    init();
    initFBO();

    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( keyboard );
    glutIdleFunc( idle );
    glutMainLoop();

    return 0;
}