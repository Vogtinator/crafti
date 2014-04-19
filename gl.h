#ifndef GL_H
#define GL_H

//nGL version 0.5.6
#include "fix.h"

//Either colors or textures, but not both
#define TEXTURE_SUPPORT

//Warning, this will be slower than textures!
//#define INTERPOLATE_COLORS

//#define WIREFRAME_MODE

//If disabled, triangles partially behind the CLIP_PLANE will be discarded
#define Z_CLIPPING

//Deactivate after you don't get any warnings.
//Otherwise it may corrupt random memory and crash.
//#define SAFE_MODE

//If some geometry inaccuracies annoy you, enable this.
//It's a bit slower though.
//#define BETTER_PERSPECTIVE

//#define FPS_COUNTER

#if defined(TEXTURE_SUPPORT) && defined(INTERPOLATE_COLORS)
#error "Colors and textures cannot be used simultaneously!"
#endif

#define NEAR_PLANE 256
#define CLIP_PLANE 25

#define MATRIX_STACK_SIZE 10

//These values are used to calculate offsets into the buffer.
//If you want something like FBOs, make them variables and set them accordingly.
//Watch out for different buffer sizes!
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

//Change if you want to, but it may not work.
typedef Fix<8, int_fast32_t> GLFix;

/* Column vectors and matrices:
 * [ [0][0] [0][1] [0][2] [0][3] ]   [x]
 * [ [1][0] [1][1] [1][2] [1][3] ] * [y]
 * [ [2][0] [2][1] [2][2] [2][3] ]   [z]
 * [ [3][0] [3][1] [3][2] [3][3] ]   [1] (not used anywhere)
 */

/* If TEXTURE_SUPPORT is enabled and a VERTEX has 0xFFFF as COLOR black pixels of the texture won't be drawn */

typedef uint16_t COLOR;
typedef struct VERTEX { GLFix x; GLFix y; GLFix z; GLFix u; GLFix v; COLOR c; } VERTEX;
typedef struct TEXTURE { int width; int height; COLOR *bitmap; } TEXTURE;

class MATRIX {
public:
    MATRIX() {}
    GLFix data[4][4] = {};
};

#define GL_COLOR_BUFFER_BIT 1<<0
#define GL_DEPTH_BUFFER_BIT 1<<1

enum GLDrawMode
{
    GL_TRIANGLES,
    GL_QUADS,
    GL_QUAD_STRIP, //Not really tested
    GL_LINE_STRIP
};

//Range [0-1]
typedef struct RGB { GLFix r; GLFix g; GLFix b; } RGB;

extern volatile int fps;
extern MATRIX *transformation;

RGB rgbColor(const COLOR c);
COLOR colorRGB(const RGB rgb);
COLOR colorRGB(const GLFix r, const GLFix g, const GLFix b);

//Invoke once before using any other functions
void nglInit();
void nglUninit();
//The buffer to render to
void nglSetBuffer(COLOR *screenBuf);
//Display the buffer
void nglDisplay();
void nglSetColor(const COLOR c);
void nglRotateX(const GLFix a);
void nglRotateY(const GLFix a);
void nglRotateZ(const GLFix a);
//Every vertex added will be appended to this buffer. It has to be allocated with enough capacity
void nglStartRecording(VERTEX *buffer);
//This returns the count of vertices added to the buffer
int nglStopRecording();
void nglAddVertices(VERTEX *buffer, unsigned int length);
void nglAddVertex(const VERTEX *vertex);
//Warning: The nglDraw*-Functions apply perspective projection!
//Returns whether the triangle is front-facing
bool nglDrawTriangle(const VERTEX *low, const VERTEX *middle, const VERTEX *high, bool backface_culling = true);
bool nglIsBackface(const VERTEX *v1, const VERTEX *v2, const VERTEX *v3);
void nglDrawTriangleZClipped(const VERTEX *low, const VERTEX *middle, const VERTEX *high);
void nglInterpolateVertexZ(const VERTEX *from, const VERTEX *to, VERTEX *res);
void nglDrawLine3D(const VERTEX *v1, const VERTEX *v2);

void nglPerspective(VERTEX *v);
void nglMultMatVectRes(const MATRIX *mat1, const VERTEX *vect, VERTEX *res);
void nglMultMatMat(MATRIX *mat1, const MATRIX *mat2);
const TEXTURE *nglGetTexture();

void glLoadIdentity();
void glBegin(const GLDrawMode mode);
inline void glEnd() { }
void glClear(const int buffers);
void glTranslatef(const GLFix x, const GLFix y, const GLFix z);

void glBindTexture(const TEXTURE *tex);
void glTexCoord2f(const GLFix nu, const GLFix nv);
void glColor3f(const GLFix r, const GLFix g, const GLFix b);
void glVertex3f(const GLFix x, const GLFix y, const GLFix z);
void glScale3f(const GLFix x, const GLFix y, const GLFix z);
void glPushMatrix();
void glPopMatrix();

#ifdef FPS_COUNTER

//This fixes some calls. printf seems to disable IRQs, which is not really what we need.
#define printf(...) do { printf(__VA_ARGS__); __asm__ volatile("mrs r0, cpsr; bic r0, r0, #0x80; msr cpsr_c, r0;" ::: "r0"); } while(0)

//puts seems to do the same
#define fputs(s,f) irq_fputs(s,f)

inline void irq_fputs(char *s, FILE *f)
{
    fputs(s, f);

    __asm__ volatile("mrs r0, cpsr;"
                    "bic r0, r0, #0x80;"
                    "msr cpsr_c, r0;" ::: "r0");
}

#endif

#endif
