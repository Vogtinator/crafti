#pragma once
#ifdef _TINSPIRE
    #include_next <libndls.h>
#else
    #include <SDL/SDL_keysym.h>
    #include <stdint.h>

    typedef enum {
        SCR_TYPE_INVALID=-1,
        SCR_320x240_565=0,
        SCR_320x240_4=1,
        SCR_240x320_565=2,
        SCR_320x240_16=3,
        SCR_320x240_8=4,
        SCR_TYPE_COUNT=5
    } scr_type_t;

    #define isKeyPressed(x) 0
    #define lcd_type(x) SCR_320x240_565

    typedef enum {
        KEY_NSPIRE_ESC = SDLK_ESCAPE,
        KEY_NSPIRE_CLICK = SDLK_KP_ENTER,
        KEY_NSPIRE_UP = SDLK_UP,
        KEY_NSPIRE_DOWN = SDLK_DOWN,
        KEY_NSPIRE_RIGHT = SDLK_RIGHT,
        KEY_NSPIRE_LEFT = SDLK_LEFT,
        KEY_NSPIRE_1 = SDLK_1,
        KEY_NSPIRE_2 = SDLK_2,
        KEY_NSPIRE_3 = SDLK_3,
        KEY_NSPIRE_4 = SDLK_4,
        KEY_NSPIRE_5 = SDLK_5,
        KEY_NSPIRE_6 = SDLK_6,
        KEY_NSPIRE_7 = SDLK_7,
        KEY_NSPIRE_8 = SDLK_8,
        KEY_NSPIRE_9 = SDLK_9,
        KEY_NSPIRE_0 = SDLK_0,
        KEY_NSPIRE_MENU = SDLK_TAB,
        KEY_NSPIRE_PERIOD = SDLK_PERIOD,
        KEY_NSPIRE_MINUS = SDLK_MINUS,
        KEY_NSPIRE_PLUS = SDLK_PLUS,
        KEY_NSPIRE_CTRL = SDLK_F1,
    } t_key;

    #define is_classic 0
    #define is_cx 1
    #define is_touchpad 1

    typedef enum tpad_arrow
    {
        TPAD_ARROW_NONE,
        TPAD_ARROW_UP, TPAD_ARROW_UPRIGHT,
        TPAD_ARROW_RIGHT, TPAD_ARROW_RIGHTDOWN,
        TPAD_ARROW_DOWN, TPAD_ARROW_DOWNLEFT,
        TPAD_ARROW_LEFT, TPAD_ARROW_LEFTUP,
        TPAD_ARROW_CLICK
    } tpad_arrow_t;

    typedef struct {
        unsigned char contact;
        uint16_t x;
        uint16_t y;
        unsigned char pressed;
        tpad_arrow_t arrow;
    } touchpad_report_t;

    extern touchpad_report_t tpad_report;
    inline bool touchpad_scan(touchpad_report_t *r) { *r = tpad_report; return true; }
#endif
