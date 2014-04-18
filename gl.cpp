#include <utility>
#include <libndls.h>

#include "gl.h"
#include "fastmath.h"

#define M(m, y, x) (m.data[y][x])
#define P(m, y, x) (m->data[y][x])

MATRIX *transformation;
static COLOR color;
static GLFix u, v;
static COLOR *screen;
static GLFix *z_buffer;
static const TEXTURE *texture;
static unsigned int vertices_count = 0;
static VERTEX vertices[4];
static GLDrawMode draw_mode = GL_TRIANGLES;
static bool recording = false;
static VERTEX* record_buffer = nullptr;
static int record_length = 0;
#ifdef SAFE_MODE
    static int matrix_stack_left = MATRIX_STACK_SIZE;
#endif

#ifdef FPS_COUNTER
    volatile int fps = 0;
    static volatile int frames = 0;
    volatile bool ngl_input_changed;
    static uint32_t saved_timer_load;
    static uint32_t saved_timer_control;
    static uint32_t saved_irq_handler;
    static uint32_t saved_irq_mask;

    #define TIMER_REG(x) (*reinterpret_cast<volatile uint32_t*>(0x900d0000 + x))
    #define VIC_REG(x) (*reinterpret_cast<volatile uint32_t*>(0xdc000000 + x))
    #define IRQ_HANDLER (*reinterpret_cast<volatile uint32_t*>(0x38))

    static void __attribute__((interrupt("IRQ"))) irq_handler()
    {
        //If timer caused interrupt
        if(VIC_REG(0) & (1 << 19))
        {
            fps = frames;
            frames = 0;
            //Acknowledge
            TIMER_REG(0xc) = 1;
        }
    }
#endif

void nglInit()
{
    init_fastmath();
    transformation = new MATRIX[MATRIX_STACK_SIZE];

    //C++ <3
    z_buffer = new std::remove_reference<decltype(*z_buffer)>::type[SCREEN_WIDTH*SCREEN_HEIGHT];
    glLoadIdentity();
    color = colorRGB(0, 0, 0); //Black
    u = v = 0;

    texture = nullptr;
    vertices_count = 0;
    draw_mode = GL_TRIANGLES;

    recording = false;
    record_buffer = nullptr;
    record_length = 0;

    #ifdef SAFE_MODE
        matrix_stack_left = MATRIX_STACK_SIZE;
    #endif

    #ifdef FPS_COUNTER
        fps = frames = 0;

        //Setup timer and interrupt handler
        saved_timer_load = TIMER_REG(0x0);
        saved_timer_control = TIMER_REG(0x8);
        saved_irq_handler = IRQ_HANDLER;
        saved_irq_mask = VIC_REG(0x10);

        VIC_REG(0x14) = ~0; //Disable all IRQs
        IRQ_HANDLER = reinterpret_cast<uint32_t>(irq_handler);
        VIC_REG(0x10) = (1 << 19); //Except 19

        //Disable timer
        TIMER_REG(0x8) = 0;
        TIMER_REG(0xc) = 1;

        //Configure timer to timeout every second
        TIMER_REG(0x4) = 1;
        TIMER_REG(0x18) = TIMER_REG(0x0) = 38768;
        TIMER_REG(0x8) = (1 << 7) | (1 << 6) | (1 << 5) | (1 << 1);

        //Enable IRQs
        __asm__ volatile("mrs r0, cpsr;"
                        "bic r0, r0, #0x80;"
                        "msr cpsr_c, r0;" ::: "r0");
    #endif
}

void nglUninit()
{
    uninit_fastmath();
    delete[] transformation;
    delete[] z_buffer;

    #ifdef FPS_COUNTER
        //Restore timer and interrupt handler
        //Disable IRQs
        __asm__ volatile("mrs r0, cpsr;"
                        "orr r0, r0, #0x80;"
                        "msr cpsr_c, r0;" ::: "r0");

        VIC_REG(0x14) = ~0;
        IRQ_HANDLER = saved_irq_handler;
        VIC_REG(0x10) = saved_irq_mask;

        TIMER_REG(0x8) = 0;
        TIMER_REG(0xc) = 1;
        TIMER_REG(0x0) = saved_timer_load;
        TIMER_REG(0x8) = saved_timer_control;
    #endif
}

void nglMultMatMat(MATRIX *mat1, MATRIX *mat2)
{
    GLFix a00 = P(mat1, 0, 0), a01 = P(mat1, 0, 1), a02 = P(mat1, 0, 2), a03 = P(mat1, 0, 3);
    GLFix a10 = P(mat1, 1, 0), a11 = P(mat1, 1, 1), a12 = P(mat1, 1, 2), a13 = P(mat1, 1, 3);
    GLFix a20 = P(mat1, 2, 0), a21 = P(mat1, 2, 1), a22 = P(mat1, 2, 2), a23 = P(mat1, 2, 3);
    GLFix a30 = P(mat1, 3, 0), a31 = P(mat1, 3, 1), a32 = P(mat1, 3, 2), a33 = P(mat1, 3, 3);

    GLFix b00 = P(mat2, 0, 0), b01 = P(mat2, 0, 1), b02 = P(mat2, 0, 2), b03 = P(mat2, 0, 3);
    GLFix b10 = P(mat2, 1, 0), b11 = P(mat2, 1, 1), b12 = P(mat2, 1, 2), b13 = P(mat2, 1, 3);
    GLFix b20 = P(mat2, 2, 0), b21 = P(mat2, 2, 1), b22 = P(mat2, 2, 2), b23 = P(mat2, 2, 3);
    GLFix b30 = P(mat2, 3, 0), b31 = P(mat2, 3, 1), b32 = P(mat2, 3, 2), b33 = P(mat2, 3, 3);

    P(mat1, 0, 0) = a00*b00 + a01*b10 + a02*b20 + a03*b30;
    P(mat1, 0, 1) = a00*b01 + a01*b11 + a02*b21 + a03*b31;
    P(mat1, 0, 2) = a00*b02 + a01*b12 + a02*b22 + a03*b32;
    P(mat1, 0, 3) = a00*b03 + a01*b13 + a02*b23 + a03*b33;
    P(mat1, 1, 0) = a10*b00 + a11*b10 + a12*b20 + a13*b30;
    P(mat1, 1, 1) = a10*b01 + a11*b11 + a12*b21 + a13*b31;
    P(mat1, 1, 2) = a10*b02 + a11*b12 + a12*b22 + a13*b32;
    P(mat1, 1, 3) = a10*b03 + a11*b13 + a12*b23 + a13*b33;
    P(mat1, 2, 0) = a20*b00 + a21*b10 + a22*b20 + a23*b30;
    P(mat1, 2, 1) = a20*b01 + a21*b11 + a22*b21 + a23*b31;
    P(mat1, 2, 2) = a20*b02 + a21*b12 + a22*b22 + a23*b32;
    P(mat1, 2, 3) = a20*b03 + a21*b13 + a22*b23 + a23*b33;
    P(mat1, 3, 0) = a30*b00 + a31*b10 + a32*b20 + a33*b30;
    P(mat1, 3, 1) = a30*b01 + a31*b11 + a32*b21 + a33*b31;
    P(mat1, 3, 2) = a30*b02 + a31*b12 + a32*b22 + a33*b32;
    P(mat1, 3, 3) = a30*b03 + a31*b13 + a32*b23 + a33*b33;
}

void nglMultMatVectRes(const MATRIX *mat1, const VERTEX *vect, VERTEX *res)
{
    GLFix x = vect->x, y = vect->y, z = vect->z;

    res->x = P(mat1, 0, 0)*x + P(mat1, 0, 1)*y + P(mat1, 0, 2)*z + P(mat1, 0, 3);
    res->y = P(mat1, 1, 0)*x + P(mat1, 1, 1)*y + P(mat1, 1, 2)*z + P(mat1, 1, 3);
    res->z = P(mat1, 2, 0)*x + P(mat1, 2, 1)*y + P(mat1, 2, 2)*z + P(mat1, 2, 3);
}

void nglPerspective(VERTEX *v)
{
#ifdef BETTER_PERSPECTIVE
    float new_z = v->z;
    decltype(new_z) new_x = v->x, new_y = v->y;
    decltype(new_z) div = decltype(new_z)(NEAR_PLANE)/new_z;

    new_x *= div;
    new_y *= div;

    v->x = new_x;
    v->y = new_y;
#else
    GLFix div = GLFix(NEAR_PLANE)/v->z;

    v->x *= div;
    v->y *= div;
#endif

    // (0/0) is in the center of the screen
    v->x += SCREEN_WIDTH/2;
    v->y += SCREEN_HEIGHT/2;

    v->y = GLFix(SCREEN_HEIGHT - 1) - v->y;

#ifdef SAFE_MODE
    if(v->u > GLFix(texture->width))
    {
        printf("Warning: Texture coord out of bounds!\n");
        v->u = texture->height;
    }
    else if(res->u < GLFix(0))
    {
        printf("Warning: Texture coord out of bounds!\n");
        res->u = 0;
    }

    if(res->v > GLFix(texture->height))
    {
        printf("Warning: Texture coord out of bounds!\n");
        res->v = texture->height;
    }
    else if(res->v < GLFix(0))
    {
        printf("Warning: Texture coord out of bounds!\n");
        res->v = 0;
    }
#endif
}

void nglSetBuffer(COLOR *screenBuf)
{
    screen = screenBuf;
}

//Will work only on CX
void nglDisplay()
{
    uint32_t *real_screen = reinterpret_cast<uint32_t*>(SCREEN_BASE_ADDRESS);
    uint32_t *buffer = reinterpret_cast<uint32_t*>(screen);
    for(unsigned int x = SCREEN_WIDTH*SCREEN_HEIGHT/8; x--;)
    {
        *(real_screen++) = *(buffer++);
        *(real_screen++) = *(buffer++);
        *(real_screen++) = *(buffer++);
        *(real_screen++) = *(buffer++);
    }

    #ifdef FPS_COUNTER
        if(frames == 0)
            printf("FPS: %d\n", fps);

        frames++;
    #endif
}

void nglRotateX(const GLFix a)
{
    MATRIX rot;

    const GLFix sina = fast_sin(a), cosa = fast_cos(a);

    M(rot, 0, 0) = 1;
    M(rot, 1, 1) = cosa;
    M(rot, 1, 2) = -sina;
    M(rot, 2, 1) = sina;
    M(rot, 2, 2) = cosa;
    M(rot, 3, 3) = 1;

    nglMultMatMat(transformation, &rot);
}

void nglRotateY(const GLFix a)
{
    MATRIX rot;

    const GLFix sina = fast_sin(a), cosa = fast_cos(a);

    M(rot, 0, 0) = cosa;
    M(rot, 0, 2) = sina;
    M(rot, 1, 1) = 1;
    M(rot, 2, 0) = -sina;
    M(rot, 2, 2) = cosa;
    M(rot, 3, 3) = 1;

    nglMultMatMat(transformation, &rot);
}

void nglRotateZ(const GLFix a)
{
    MATRIX rot;

    const GLFix sina = fast_sin(a), cosa = fast_cos(a);

    M(rot, 0, 0) = cosa;
    M(rot, 0, 1) = -sina;
    M(rot, 1, 0) = sina;
    M(rot, 1, 1) = cosa;
    M(rot, 2, 2) = 1;
    M(rot, 3, 3) = 1;

    nglMultMatMat(transformation, &rot);
}

inline void pixel(const int x, const int y, const GLFix z, const COLOR c)
{
    if(x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT)
        return;

    const int pitch = x + y*SCREEN_WIDTH;

    if(z <= GLFix(CLIP_PLANE) || z_buffer[pitch] <= z)
        return;

    z_buffer[pitch] = z;

    screen[pitch] = c;
}

RGB rgbColor(const COLOR c)
{
    const GLFix r = (c >> 11) & 0b11111;
    const GLFix g = (c >> 5) & 0b111111;
    const GLFix b = (c >> 0) & 0b11111;

    return {r / GLFix(0b11111), g / GLFix(0b111111), b / GLFix(0b11111)};
}

COLOR colorRGB(const RGB rgb)
{
    return colorRGB(rgb.r, rgb.g, rgb.b);
}

COLOR colorRGB(const GLFix r, const GLFix g, const GLFix b)
{
    const int r1 = (r * GLFix(0b11111)).round();
    const int g1 = (g * GLFix(0b111111)).round();
    const int b1 = (b * GLFix(0b11111)).round();

    return ((r1 & 0b11111) << 11) | ((g1 & 0b111111) << 5) | (b1 & 0b11111);
}

//Doesn't interpolate colors even if enabled
void nglDrawLine3D(const VERTEX *v1, const VERTEX *v2)
{    
    //TODO: Z-Clipping

    VERTEX v1_p = *v1, v2_p = *v2;
    nglPerspective(&v1_p);
    nglPerspective(&v2_p);

    const GLFix diff_x = v2_p.x - v1_p.x;
    const GLFix dy = (v2_p.y - v1_p.y) / diff_x;

    const COLOR c = v1_p.c;

    //Height > width? -> Interpolate X
    if(dy > GLFix(1) || dy < GLFix(-1))
    {
        if(v2_p.y < v1_p.y)
            std::swap(v1_p, v2_p);

        const GLFix diff_y = v2_p.y - v1_p.y;

        const GLFix dx = (v2_p.x - v1_p.x) / diff_y;
        const GLFix dz = (v2_p.z - v1_p.z) / diff_y;

        int end_y = v2_p.y;

        if(end_y >= SCREEN_HEIGHT)
            end_y = SCREEN_HEIGHT - 1;

        for(; v1_p.y <= GLFix(end_y); ++v1_p.y)
        {
            pixel(v1_p.x, v1_p.y, v1_p.z, c);

            v1_p.x += dx;
            v1_p.z += dz;
        }
    }
    else
    {
        if(v2_p.x < v1_p.x)
            std::swap(v1_p, v2_p);

        const GLFix dz = (v2_p.z - v1_p.z) / diff_x;

        int end_x = v2_p.x;

        if(end_x >= SCREEN_WIDTH)
            end_x = SCREEN_WIDTH - 1;

        for(; v1_p.x <= GLFix(end_x); ++v1_p.x)
        {
            pixel(v1_p.x, v1_p.y, v1_p.z, c);

            v1_p.y += dy;
            v1_p.z += dz;
        }
    }
}

/* Basic scanline algorithm, no perspective correct texture mapping. */
static void nglDrawTriangleXZClipped(const VERTEX *low, const VERTEX *middle, const VERTEX *high)
{
    if(middle->y > high->y)
        std::swap(middle, high);

    if(low->y > middle->y)
        std::swap(low, middle);

    if(middle->y > high->y)
        std::swap(middle, high);

    if(high->y < GLFix(0) || low->y >= GLFix(SCREEN_HEIGHT))
        return;

    int low_y = low->y, middle_y = middle->y, high_y = high->y + 1;

    const int height_upper = high_y - middle_y;

    const GLFix dx_upper = (high->x - middle->x) / height_upper;
    const GLFix dz_upper = (high->z - middle->z) / height_upper;

    const int height_lower = middle_y - low_y + 1;

    const GLFix dx_lower = (middle->x - low->x) / height_lower;
    const GLFix dz_lower = (middle->z - low->z) / height_lower;

    const int height_far = high_y - low_y;

    const GLFix dx_far = (high->x - low->x) / height_far;
    const GLFix dz_far = (high->z - low->z) / height_far;

    #ifdef TEXTURE_SUPPORT
        const GLFix du_upper = (high->u - middle->u) / height_upper;
        const GLFix dv_upper = (high->v - middle->v) / height_upper;

        const GLFix du_lower = (middle->u - low->u) / height_lower;
        const GLFix dv_lower = (middle->v - low->v) / height_lower;

        const GLFix du_far = (high->u - low->u) / height_far;
        const GLFix dv_far = (high->v - low->v) / height_far;

        GLFix ustart = low->u, uend = low->u;
        GLFix vstart = low->v, vend = low->v;
    #elif defined(INTERPOLATE_COLORS)
        const RGB high_rgb = rgbColor(high->c);
        const RGB middle_rgb = rgbColor(middle->c);
        const RGB low_rgb = rgbColor(low->c);

        const GLFix dr_upper = (high_rgb.r - middle_rgb.r) / height_upper;
        const GLFix dg_upper = (high_rgb.g - middle_rgb.g) / height_upper;
        const GLFix db_upper = (high_rgb.b - middle_rgb.b) / height_upper;

        const GLFix dr_lower = (middle_rgb.r - low_rgb.r) / height_lower;
        const GLFix dg_lower = (middle_rgb.g - low_rgb.g) / height_lower;
        const GLFix db_lower = (middle_rgb.b - low_rgb.b) / height_lower;

        const GLFix dr_far = (high_rgb.r - low_rgb.r) / height_far;
        const GLFix dg_far = (high_rgb.g - low_rgb.g) / height_far;
        const GLFix db_far = (high_rgb.b - low_rgb.b) / height_far;

        GLFix rstart = low_rgb.r, rend = low_rgb.r;
        GLFix gstart = low_rgb.g, gend = low_rgb.g;
        GLFix bstart = low_rgb.b, bend = low_rgb.b;
    #endif

    int y = low_y;
    GLFix xstart = low->x, zstart = low->z, xend = low->x, zend = low->z;

    //Vertical clipping
    if(y < 0)
    {
        const int diff = -y;
        int diff_lower = diff;
        int diff_upper = 0;
        if(diff_lower > height_lower)
        {
            diff_lower = height_lower;
            diff_upper = diff - diff_lower;
        }

        y = 0;

        xstart += dx_far * diff;
        zstart += dz_far * diff;
        xend += dx_lower * diff_lower;
        zend += dz_lower * diff_lower;
        xend += dx_upper * diff_upper;
        zend += dz_upper * diff_upper;

        #ifdef TEXTURE_SUPPORT
            ustart += du_far * diff;
            vstart += dv_far * diff;
            uend += du_lower * diff_lower;
            vend += dv_lower * diff_lower;
            uend += du_upper * diff_upper;
            vend += dv_upper * diff_upper;
        #elif defined(INTERPOLATE_COLORS)
            rstart += dr_far * diff;
            gstart += dg_far * diff;
            bstart += db_far * diff;
            rend += dr_lower * diff_lower;
            gend += dg_lower * diff_lower;
            bend += db_lower * diff_lower;
            rend += dr_upper * diff_upper;
            gend += dg_upper * diff_upper;
            bend += db_upper * diff_upper;
        #endif
    }

    if(high_y >= SCREEN_HEIGHT)
        high_y = SCREEN_HEIGHT - 1;

    int pitch = y * SCREEN_WIDTH;
    decltype(z_buffer) z_buf_line = z_buffer + pitch;
    decltype(screen) screen_buf_line = screen + pitch;

    GLFix dx_current = dx_lower, dz_current = dz_lower;
#ifdef TEXTURE_SUPPORT
    GLFix du_current = du_lower, dv_current = dv_lower;
#elif defined(INTERPOLATE_COLORS)
    GLFix dr_current = dr_lower, dg_current = dg_lower, db_current = db_lower;
#endif

    if(__builtin_expect(y > middle_y, false))
    {
        dx_current = dx_upper;
        dz_current = dz_upper;

        #ifdef TEXTURE_SUPPORT
            du_current = du_upper;
            dv_current = dv_upper;
        #elif defined(INTERPOLATE_COLORS)
            dr_current = dr_upper;
            dg_current = dg_upper;
            db_current = db_upper;
        #endif
    }

    //If xstart will get smaller than xend
    if(dx_lower < dx_far)
        goto otherway;

    for(; y <= high_y; y += 1, z_buf_line += SCREEN_WIDTH, screen_buf_line += SCREEN_WIDTH)
    {
        int line_width = xend - xstart;
        if(__builtin_expect(line_width >= 1, true))
        {
            const GLFix dz = (zend - zstart) / line_width;
            GLFix z = zstart;

            #ifdef TEXTURE_SUPPORT
                const GLFix du = (uend - ustart) / line_width;
                const GLFix dv = (vend - vstart) / line_width;
                GLFix u = ustart, v = vstart;
            #elif defined(INTERPOLATE_COLORS)
                const GLFix dr = (rend - rstart) / line_width;
                const GLFix dg = (gend - gstart) / line_width;
                const GLFix db = (bend - bstart) / line_width;

                GLFix r = rstart, g = gstart, b = bstart;
            #endif

            int x1 = xstart, x2 = xend;

            decltype(z_buffer) z_buf = z_buf_line + x1;
            decltype(screen) screen_buf = screen_buf_line + x1;
            for(int x = x1; x <= x2; x += 1, ++z_buf, ++screen_buf)
            {
                if(__builtin_expect(*z_buf > z, true))
                {
                    *z_buf = z;
                    #ifdef TEXTURE_SUPPORT
                        *screen_buf = texture->bitmap[u.floor() + v.floor()*texture->width];
                    #elif defined(INTERPOLATE_COLORS)
                        *screen_buf = colorRGB(r, g, b);
                    #else
                        *screen_buf = low->c;
                    #endif
                }

                #ifdef TEXTURE_SUPPORT
                    u += du;
                    v += dv;
                #elif defined(INTERPOLATE_COLORS)
                    r += dr;
                    g += dg;
                    b += db;
                #endif

                z += dz;
            }
        }

        xstart += dx_far;
        zstart += dz_far;

        xend += dx_current;
        zend += dz_current;

        #ifdef TEXTURE_SUPPORT
                ustart += du_far;
                vstart += dv_far;
                uend += du_current;
                vend += dv_current;
        #elif defined(INTERPOLATE_COLORS)
                rstart += dr_far;
                gstart += dg_far;
                bstart += db_far;

                rend += dr_current;
                gend += dg_current;
                bend += db_current;
        #endif

        if(__builtin_expect(y == middle_y, false))
        {
            dx_current = dx_upper;
            dz_current = dz_upper;

            #ifdef TEXTURE_SUPPORT
                du_current = du_upper;
                dv_current = dv_upper;
            #elif defined(INTERPOLATE_COLORS)
                dr_current = dr_upper;
                dg_current = dg_upper;
                db_current = db_upper;
            #endif
        }
    }

    return;

    otherway:
    for(; y <= high_y; y += 1, screen_buf_line += SCREEN_WIDTH, z_buf_line += SCREEN_WIDTH)
    {
        int line_width = xend - xstart;
        if(__builtin_expect(line_width <= -1, true))
        {
            //Here come the differences
            const GLFix dz = (zend - zstart) / line_width;
            GLFix z = zend;

            #ifdef TEXTURE_SUPPORT
                const GLFix du = (uend - ustart) / line_width;
                const GLFix dv = (vend - vstart) / line_width;
                GLFix u = uend, v = vend;
            #elif defined(INTERPOLATE_COLORS)
                const GLFix dr = (rend - rstart) / line_width;
                const GLFix dg = (gend - gstart) / line_width;
                const GLFix db = (bend - bstart) / line_width;

                GLFix r = rend, g = gend, b = bend;
            #endif

            int x1 = xend, x2 = xstart;

            decltype(z_buffer) z_buf = z_buf_line + x1;
            decltype(screen) screen_buf = screen_buf_line + x1;
            for(int x = x1; x <= x2; x += 1, ++z_buf, ++screen_buf)
            {
                if(__builtin_expect(*z_buf > z, true))
                {
                    *z_buf = z;
                    #ifdef TEXTURE_SUPPORT
                        *screen_buf = texture->bitmap[u.floor() + v.floor()*texture->width];
                    #elif defined(INTERPOLATE_COLORS)
                        *screen_buf = colorRGB(r, g, b);
                    #else
                        *screen_buf = low->c;
                    #endif
                }

                #ifdef TEXTURE_SUPPORT
                    u += du;
                    v += dv;
                #elif defined(INTERPOLATE_COLORS)
                    r += dr;
                    g += dg;
                    b += db;
                #endif

                z += dz;
            }
        }

        xstart += dx_far;
        zstart += dz_far;

        xend += dx_current;
        zend += dz_current;

        #ifdef TEXTURE_SUPPORT
                ustart += du_far;
                vstart += dv_far;

                uend += du_current;
                vend += dv_current;
        #elif defined(INTERPOLATE_COLORS)
                rstart += dr_far;
                gstart += dg_far;
                bstart += db_far;

                rend += dr_current;
                gend += dg_current;
                bend += db_current;
        #endif

        if(__builtin_expect(y == middle_y, false))
        {
            dx_current = dx_upper;
            dz_current = dz_upper;

            #ifdef TEXTURE_SUPPORT
                du_current = du_upper;
                dv_current = dv_upper;
            #elif defined(INTERPOLATE_COLORS)
                dr_current = dr_upper;
                dg_current = dg_upper;
                db_current = db_upper;
            #endif
        }
    }

    return;
}

static void interpolateVertexX(const VERTEX *from, const VERTEX *to, VERTEX *res)
{
    GLFix diff = to->x - from->x;
    if(diff < GLFix(1) && diff > GLFix(-1))
        diff = 1;

    GLFix end = from->x < GLFix(0) ? 0 : (SCREEN_WIDTH - 1);
    GLFix t = (end - from->x) / diff;

    res->x = end;
    res->y = from->y + (to->y - from->y) * t;
    res->z = from->z + (to->z - from->z) * t;

#ifdef TEXTURE_SUPPORT
    res->u = from->u + (to->u - from->u) * t;
    res->u = res->u.wholes();
    res->v = from->v + (to->v - from->v) * t;
    res->v = res->v.wholes();
#elif defined(INTERPOLATE_COLORS)
    RGB c_from = rgbColor(from->c);
    RGB c_to = rgbColor(to->c);

    res->c = colorRGB(c_from.r + (c_to.r - c_from.r) * t, c_from.r + (c_to.r - c_from.r) * t, c_from.r + (c_to.r - c_from.r) * t);
#else
    res->c = from->c;
#endif
}

void nglDrawTriangleZClipped(const VERTEX *low, const VERTEX *middle, const VERTEX *high)
{
    //If not on screen, skip
    if((low->y < GLFix(0) && middle->y < GLFix(0) && high->y < GLFix(0)) || (low->y >= GLFix(SCREEN_HEIGHT) && middle->y >= GLFix(SCREEN_HEIGHT) && high->y >= GLFix(SCREEN_HEIGHT)))
        return;

    const VERTEX* invisible[3];
    const VERTEX* visible[3];
    int count_invisible = -1, count_visible = -1;

    if(low->x < GLFix(0) || low->x >= GLFix(SCREEN_WIDTH))
        invisible[++count_invisible] = low;
    else
        visible[++count_visible] = low;

    if(middle->x < GLFix(0) || middle->x >= GLFix(SCREEN_WIDTH))
        invisible[++count_invisible] = middle;
    else
        visible[++count_visible] = middle;

    if(high->x < GLFix(0) || high->x >= GLFix(SCREEN_WIDTH))
        invisible[++count_invisible] = high;
    else
        visible[++count_visible] = high;

    //Interpolated vertices
    VERTEX v1, v2;

    switch(count_visible)
    {
    case -1:
        break;
    case 0:
        interpolateVertexX(invisible[0], visible[0], &v1);
        interpolateVertexX(invisible[1], visible[0], &v2);
        nglDrawTriangleXZClipped(visible[0], &v1, &v2);
        break;
    case 1:
        interpolateVertexX(invisible[0], visible[0], &v1);
        interpolateVertexX(invisible[0], visible[1], &v2);
        nglDrawTriangleXZClipped(visible[0], visible[1], &v1);
        nglDrawTriangleXZClipped(visible[1], &v1, &v2);
        break;
    case 2:
        nglDrawTriangleXZClipped(visible[0], visible[1], visible[2]);
        break;
    }
}

#ifdef Z_CLIPPING
    void nglInterpolateVertexZ(const VERTEX *from, const VERTEX *to, VERTEX *res)
    {
        GLFix diff = to->z - from->z;
        if(diff < GLFix(1) && diff > GLFix(-1))
            diff = 1;

        GLFix t = (GLFix(CLIP_PLANE) - from->z) / diff;

        res->x = from->x + (to->x - from->x) * t;
        res->y = from->y + (to->y - from->y) * t;
        res->z = CLIP_PLANE;

        #ifdef TEXTURE_SUPPORT
            res->u = from->u + (to->u - from->u) * t;
            res->u = res->u.wholes();
            res->v = from->v + (to->v - from->v) * t;
            res->v = res->v.wholes();
        #elif defined(INTERPOLATE_COLORS)
            RGB c_from = rgbColor(from->c);
            RGB c_to = rgbColor(to->c);

            res->c = colorRGB(c_from.r + (c_to.r - c_from.r) * t, c_from.r + (c_to.r - c_from.r) * t, c_from.r + (c_to.r - c_from.r) * t);
        #else
            res->c = from->c;
        #endif
    }
#endif

bool nglIsBackface(const VERTEX *v1, const VERTEX *v2, const VERTEX *v3)
{
    int x1 = v2->x - v1->x, x2 = v3->x - v1->x;
    int y1 = v2->y - v1->y, y2 = v3->y - v1->y;

    return x1 * y2 < x2 * y1;
}

bool nglDrawTriangle(const VERTEX *low, const VERTEX *middle, const VERTEX *high, bool backface_culling)
{
#ifndef Z_CLIPPING
    if(low->z < GLFix(CLIP_PLANE) || middle->z < GLFix(CLIP_PLANE) || high->z < GLFix(CLIP_PLANE))
        return true;

    VERTEX low_p = *low, middle_p = *middle, high_p = *high;

    nglPerspective(&low_p);
    nglPerspective(&middle_p);
    nglPerspective(&high_p);

    if(backface_culling && nglIsBackface(&low_p, &middle_p, &high_p))
        return false;

    nglDrawTriangleZClipped(&low_p, &middle_p, &high_p);

    return true;
#else

    VERTEX invisible[3];
    VERTEX visible[3];
    int count_invisible = -1, count_visible = -1;

    if(low->z < GLFix(CLIP_PLANE))
        invisible[++count_invisible] = *low;
    else
        visible[++count_visible] = *low;

    if(middle->z < GLFix(CLIP_PLANE))
        invisible[++count_invisible] = *middle;
    else
        visible[++count_visible] = *middle;

    if(high->z < GLFix(CLIP_PLANE))
        invisible[++count_invisible] = *high;
    else
        visible[++count_visible] = *high;

    //Interpolated vertices
    VERTEX v1, v2;

    switch(count_visible)
    {
    case -1:
        return true;

    case 0:
        nglInterpolateVertexZ(&invisible[0], &visible[0], &v1);
        nglInterpolateVertexZ(&invisible[1], &visible[0], &v2);

        nglPerspective(&visible[0]);
        nglPerspective(&v1);
        nglPerspective(&v2);

        if(backface_culling && nglIsBackface(&visible[0], &v1, &v2))
            return false;

        nglDrawTriangleZClipped(&visible[0], &v1, &v2);
        return true;

    case 1:
        nglInterpolateVertexZ(&visible[0], &invisible[0], &v1);
        nglInterpolateVertexZ(&visible[1], &invisible[0], &v2);

        nglPerspective(&visible[0]);
        nglPerspective(&visible[1]);
        nglPerspective(&v1);

        if(backface_culling && nglIsBackface(&visible[0], &visible[1], &v1))
            return false;

        nglPerspective(&v2);
        nglDrawTriangleZClipped(&visible[0], &visible[1], &v1);
        nglDrawTriangleZClipped(&visible[1], &v1, &v2);
        return true;

    case 2:
        nglPerspective(&visible[0]);
        nglPerspective(&visible[1]);
        nglPerspective(&visible[2]);

        if(backface_culling && nglIsBackface(&visible[0], &visible[1], &visible[2]))
            return false;

        nglDrawTriangleZClipped(&visible[0], &visible[1], &visible[2]);
        return true;

    default:
        return true;
    }
#endif
}

void nglSetColor(const COLOR c)
{
    color = c;
}

void nglStartRecording(VERTEX* buffer)
{
    record_buffer = buffer;

    recording = true;
    record_length = 0;
}

int nglStopRecording()
{
    recording = false;

    return record_length;
}

void nglAddVertices(VERTEX *buffer, unsigned int length)
{
    while(length--)
        nglAddVertex(buffer++);
}

void nglAddVertex(const VERTEX* vertex)
{
    #ifdef SAFE_MODE
        if(texture == nullptr)
        {
            printf("ngl.lang.NoTextureException: Please, don't make me dereference the nullptr!\n");
            return;
        }
    #endif

    if(recording)
    {
        *record_buffer++ = *vertex;
        ++record_length;
        return;
    }

    VERTEX *current_vertex = &vertices[vertices_count];

    current_vertex->c = vertex->c;
    #ifdef TEXTURE_SUPPORT
        current_vertex->u = vertex->u;
        current_vertex->v = vertex->v;
    #endif

    nglMultMatVectRes(transformation, vertex, current_vertex);

    ++vertices_count;

    GLFix x1, y1, x2, y2;

    switch(draw_mode)
    {
    case GL_TRIANGLES:
        if(vertices_count != 3)
            break;

        vertices_count = 0;

#ifdef WIREFRAME_MODE
        nglDrawLine3D(&vertices[0], &vertices[1]);
        nglDrawLine3D(&vertices[0], &vertices[2]);
        nglDrawLine3D(&vertices[2], &vertices[1]);
#else
        nglDrawTriangle(&vertices[0], &vertices[1], &vertices[2]);
#endif
        break;

    case GL_QUADS:
        if(vertices_count != 4)
            break;

        vertices_count = 0;

#ifdef WIREFRAME_MODE
        nglDrawLine3D(&vertices[0], &vertices[1]);
        nglDrawLine3D(&vertices[1], &vertices[2]);
        nglDrawLine3D(&vertices[2], &vertices[3]);
        nglDrawLine3D(&vertices[3], &vertices[0]);
#else
        if(nglDrawTriangle(&vertices[0], &vertices[1], &vertices[2]))
            nglDrawTriangle(&vertices[2], &vertices[3], &vertices[0]);
#endif
        break;

    case GL_QUAD_STRIP:
        if(vertices_count != 4)
            break;

        vertices_count = 2;

#ifdef WIREFRAME_MODE
        nglDrawLine3D(&vertices[0], &vertices[1]);
        nglDrawLine3D(&vertices[1], &vertices[2]);
        nglDrawLine3D(&vertices[2], &vertices[3]);
        nglDrawLine3D(&vertices[3], &vertices[0]);
#else
        if(nglDrawTriangle(&vertices[0], &vertices[1], &vertices[2]))
            nglDrawTriangle(&vertices[2], &vertices[3], &vertices[0]);
#endif

        vertices[0] = vertices[2];
        vertices[1] = vertices[3];
        break;
    case GL_LINE_STRIP:
        if(vertices_count != 2)
            break;

        vertices_count = 1;

        nglDrawLine3D(&vertices[0], &vertices[1]);

        vertices[0] = vertices[1];
        break;
    }
}

const TEXTURE *nglGetTexture()
{
    return texture;
}

void glBindTexture(const TEXTURE *tex)
{
    texture = tex;
}

void glColor3f(const GLFix r, const GLFix g, const GLFix b)
{
    color = colorRGB(r, g, b);
}

void glTexCoord2f(const GLFix nu, const GLFix nv)
{
    u = nu;
    v = nv;
}

void glVertex3f(const GLFix x, const GLFix y, const GLFix z)
{
    const VERTEX ver = {
            .x = x,
            .y = y,
            .z = z,
            .u = u,
            .v = v,
            .c = color
    };

    nglAddVertex(&ver);
}

void glBegin(GLDrawMode mode)
{
    vertices_count = 0;
    draw_mode = mode;
}

//Could be faster than memset and allows us to specify a 32-bit value instead of 8-bit
template <typename T>
void memclear(const T value, T *buffer, uint32_t size)
{
    while(size--)
        *buffer++ = value;
}

void glClear(const int buffers)
{
    if(buffers & GL_COLOR_BUFFER_BIT)
        memclear(color, screen, SCREEN_WIDTH*SCREEN_HEIGHT);

    //The depth buffer uses unsigned 32-bit ints internally
    if(buffers & GL_DEPTH_BUFFER_BIT)
        memclear(z_buffer->maxValue(), z_buffer, SCREEN_WIDTH*SCREEN_HEIGHT);
}

void glLoadIdentity()
{
    *transformation = {}; // Copy empty matrix into transformation
    P(transformation, 0, 0) = P(transformation, 1, 1) = P(transformation, 2, 2) = P(transformation, 3, 3) = 1;
}

void glTranslatef(const GLFix x, const GLFix y, const GLFix z)
{
    MATRIX trans;

    M(trans, 0, 0) = M(trans, 1, 1) = M(trans, 2, 2) = M(trans, 3, 3) = 1;
    M(trans, 0, 3) = x;
    M(trans, 1, 3) = y;
    M(trans, 2, 3) = z;

    nglMultMatMat(transformation, &trans);
}

void glScale3f(const GLFix x, const GLFix y, const GLFix z)
{
    MATRIX scale;

    M(scale, 0, 0) = x;
    M(scale, 1, 1) = y;
    M(scale, 2, 2) = z;
    M(scale, 3, 3) = 1;

    nglMultMatMat(transformation, &scale);
}

void glPopMatrix()
{
    #ifdef SAFE_MODE
        if(matrix_stack_left == MATRIX_STACK_SIZE)
        {
            printf("Error: No matrix left on the stack anymore!\n");
            return;
        }
        ++matrix_stack_left;
    #endif

    --transformation;
}

void glPushMatrix()
{
    #ifdef SAFE_MODE
        if(matrix_stack_left == 0)
        {
            printf("Error: Matrix stack limit reached!\n");
            return;
        }
        matrix_stack_left--;
    #endif

    ++transformation;
    *transformation = *(transformation - 1);
}
