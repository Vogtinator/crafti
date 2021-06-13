//Colored triangles can still be used with nglForceColor(true)
#define TEXTURE_SUPPORT

//Warning, this is slower than textured rendering!
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

//Crashes sometimes, only works on CX
#define FPS_COUNTER

#if defined(TEXTURE_SUPPORT) && defined(INTERPOLATE_COLORS)
#error "Colors and textures cannot be used simultaneously!"
#endif

#define CLIP_PLANE 25

#define MATRIX_STACK_SIZE 10
