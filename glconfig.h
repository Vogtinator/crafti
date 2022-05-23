//Colored triangles can still be used with nglForceColor(true)
#define TEXTURE_SUPPORT

//Warning, this is slower than textured rendering!
//#define INTERPOLATE_COLORS

//#define WIREFRAME_MODE

//If disabled, triangles partially behind the CLIP_PLANE will be discarded
#define Z_CLIPPING

//If some geometry inaccuracies annoy you, enable this.
//It's a bit slower though.
#define BETTER_PERSPECTIVE

#define FPS_COUNTER

#if defined(TEXTURE_SUPPORT) && defined(INTERPOLATE_COLORS)
#error "Colors and textures cannot be used simultaneously!"
#endif

#define CLIP_PLANE 25

#define MATRIX_STACK_SIZE 10
