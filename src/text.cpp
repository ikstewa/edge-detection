
#include <GL/glut.h>
#include <string>

int frame = 0;
int my_time = 0;
int timebase = 0;
char buffer[128];
void drawText(int, int, void*, std::string);
void fps();


void fps()
{
   glClear(GL_DEPTH_BUFFER_BIT);

   // calculate fps
   frame++;
   my_time = glutGet(GLUT_ELAPSED_TIME);
   if (my_time - timebase > 1000) {
      sprintf_s(buffer, "FPS: %4.2f", frame*1000.0/(my_time-timebase));
      timebase = my_time;
      frame = 0;
   }

   drawText(87, 95, GLUT_BITMAP_HELVETICA_12, std::string(buffer));
}


// Draws text to the screen. Can be invoked from anywhere in the display loop
// Specify color via glColor()
// NOTE: coords are from lower left screen. [0,100]
void drawText(int x, int y, void * font, std::string text)
{
    // TODO: verify correct matrix mode return
    // remember the current matrix mode
    GLint matrixMode;
    glGetIntegerv(GL_MATRIX_MODE, &matrixMode);

    // remember the current lighitng status
    GLboolean flipLights;
    glGetBooleanv(GL_LIGHTING, &flipLights);

    // Change the projection to a new Ortho for screen coord drawing
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0.0, 100.0, 0.0, 100.0);

        // start with a fresh ModelView matrix to allow text drawing spontaneously
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();

            glDisable(GL_LIGHTING);

            glRasterPos2i(x,y);

            //for (c = text; *c != '\0'; c++) //Removed because we changed text to be const.
            for (unsigned int index=0; index < text.size(); ++index)
            {
                glutBitmapCharacter(font, text[index]);
            }

        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    // return to the previous state
    if (flipLights)
        glEnable(GL_LIGHTING);
    // revert matrix_mode
    glMatrixMode(matrixMode);
    //glMatrixMode(GL_MODELVIEW);
}