#include <nucleus.h>
#include <libndls.h>
#include <unistd.h>

#include "gl.h"
#include "fastmath.h"
#include "world.h"

//Image resources
#include "terrain.h"
#include "menu.h"
#include "selection.h"

//isKeyPressed checks the hardware-type, but that's slow
static inline bool keyPressed(const t_key &key)
{
    return *reinterpret_cast<volatile uint16_t*>(0x900E0000 + key.tpad_row) & key.tpad_col;
}

static GLFix xr = 0, yr = 0;
static GLFix x = 0, y = BLOCK_SIZE * Chunk::SIZE * World::HEIGHT + BLOCK_SIZE, z = 0;
static constexpr GLFix incr = 20;
static World world;
static int current_block = BLOCK_STONE;

static void getForward(GLFix *x, GLFix *z)
{
    *x = fast_sin(yr) * incr;
    *z = fast_cos(yr) * incr;
}

static void getRight(GLFix *x, GLFix *z)
{
    *x = fast_sin((yr + 90).normaliseAngle()) * incr;
    *z = fast_cos((yr + 90).normaliseAngle()) * incr;
}

static void drawTexture(TEXTURE &src, int src_x, int src_y, TEXTURE &dest, int dest_x, int dest_y, int w, int h)
{
    for(int y = 0; y < h; y++)
        for(int x = 0; x < w; x++)
            dest.bitmap[dest_x + x + (dest_y + y)*dest.width] = src.bitmap[src_x + x + (src_y + y)*src.width];
}

static void drawTransparentTexture(TEXTURE &src, int src_x, int src_y, TEXTURE &dest, int dest_x, int dest_y, int w, int h)
{
    for(int y = 0; y < h; y++)
        for(int x = 0; x < w; x++)
        {
            COLOR *old_c = dest.bitmap + (dest_x + x + (dest_y + y)*dest.width);
            COLOR new_c = src.bitmap[src_x + x + (src_y + y)*src.width];

            const unsigned int r_o = (*old_c >> 11) & 0b11111;
            const unsigned int g_o = (*old_c >> 5) & 0b111111;
            const unsigned int b_o = (*old_c >> 0) & 0b11111;

            const unsigned int r_n = (new_c >> 11) & 0b11111;
            const unsigned int g_n = (new_c >> 5) & 0b111111;
            const unsigned int b_n = (new_c >> 0) & 0b11111;

            unsigned int r = (r_n + r_o) >> 1;
            unsigned int g = (g_n + g_o) >> 1;
            unsigned int b = (b_n + b_o) >> 1;

            *old_c = (r << 11) | (g << 5) | (b << 0);
        }
}

static constexpr int savefile_version = 2;

#define LOAD_FROM_FILE(var) if(fread(&var, sizeof(var), 1, savefile) != 1) return false;
#define SAVE_TO_FILE(var) if(fwrite(&var, sizeof(var), 1, savefile) != 1) return false;

static bool loadFile(FILE *savefile)
{
    int version;
    LOAD_FROM_FILE(version);

    if(version != savefile_version)
    {
        printf("Wrong save file version %d!\n", version);
        return false;
    }

    LOAD_FROM_FILE(xr)
    LOAD_FROM_FILE(yr)
    LOAD_FROM_FILE(x)
    LOAD_FROM_FILE(y)
    LOAD_FROM_FILE(z)
    LOAD_FROM_FILE(current_block)

    return world.loadFromFile(savefile);
}

static bool saveFile(FILE *savefile)
{
    SAVE_TO_FILE(savefile_version)
    SAVE_TO_FILE(xr)
    SAVE_TO_FILE(yr)
    SAVE_TO_FILE(x)
    SAVE_TO_FILE(y)
    SAVE_TO_FILE(z)
    SAVE_TO_FILE(current_block)

    return world.saveToFile(savefile);
}

enum GAMESTATE {
    WORLD = 0,
    MENU,
    BLOCK_LIST
};

enum MENUITEM {
    NEW_WORLD = 0,
    LOAD_WORLD,
    SAVE_WORLD,
    SAVE_AND_EXIT,
    EXIT,
    MENU_ITEM_MAX
};

#define CR_PIXEL(x, y) (screen.bitmap[SCREEN_WIDTH/2+(x) + (SCREEN_HEIGHT/2+(y))*SCREEN_WIDTH] = ~screen.bitmap[SCREEN_WIDTH/2+(x) + (SCREEN_HEIGHT/2+(y))*SCREEN_WIDTH])

int main(int argc, char *argv[])
{
    if(!lcd_isincolor())
    {
        show_msgbox("Error", "Your LCD hasn't got enough colors :-P");
        return 1;
    }

    nglInit();

    TEXTURE screen;
    screen.width = SCREEN_WIDTH;
    screen.height = SCREEN_HEIGHT;
    screen.bitmap = new COLOR[SCREEN_WIDTH*SCREEN_HEIGHT];

    nglSetBuffer(screen.bitmap);

    init_blockData(&terrain);
    glBindTexture(&terrain);

    glLoadIdentity();

    const char savefile_name_default[] = "/documents/ndless/crafti.map.tns";
    const char* savefile_name = savefile_name_default;
    if(argc > 1)
        savefile_name = argv[1];

    FILE *savefile = fopen(savefile_name, "rb");
    if(!savefile)
        printf("No previous save found.\n");
    else if(loadFile(savefile))
        printf("Loaded world.\n");
    else
        printf("Failed to load world!\n");

    fclose(savefile);
    savefile = nullptr;

    GAMESTATE gamestate = WORLD;
    COLOR *background = new COLOR[SCREEN_WIDTH*SCREEN_HEIGHT];
    bool save_background = true;

    //State for GAMESTATE WORLD
    const GLFix player_width = BLOCK_SIZE*0.9f, player_height = BLOCK_SIZE*1.8f, eye_pos = BLOCK_SIZE*1.6f;
    bool key_held_down = true, can_jump = false, tp_had_contact = false, menu_held_down = true;
    int tp_last_x = 0, tp_last_y = 0;
    GLFix vy = 0; //Y-Velocity for gravity and jumps

    //State for GAMESTATE MENU
    int menu_selected_item = 0, menu_width_visible = 0, menu_open = true;
    TEXTURE menu_with_selection;
    menu_with_selection.width = menu.width;
    menu_with_selection.height = menu.height;
    menu_with_selection.bitmap = new COLOR[menu.width * menu.height];

    //State for GAMESTATE BLOCK_LIST
    TEXTURE black;
    black.width = SCREEN_WIDTH - 2*25;
    black.height = SCREEN_HEIGHT - 2*25;
    black.bitmap = new COLOR[black.width * black.height];

    while(true)
    {
        if(gamestate == WORLD)
        {
            AABB aabb(x - player_width/2, y, z - player_width/2, x + player_width/2, y + player_height, z + player_width/2);
            //printf("X: %d Y: %d Z: %d XR: %d YR: %d\n", x.toInt(), y.toInt(), z.toInt(), xr.toInt(), yr.toInt());

            glColor3f(0.4f, 0.6f, 0.8f); //Blue background
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();

            nglRotateX((GLFix(360) - xr.normaliseAngle()).normaliseAngle());
            nglRotateY((GLFix(360) - yr.normaliseAngle()).normaliseAngle());
            glTranslatef(-x, -y - eye_pos, -z);

            world.setPosition(x, y, z);

            world.render();

            glPopMatrix();

            //Draw GUI

            CR_PIXEL(0, 0);
            CR_PIXEL(-1, 0);
            CR_PIXEL(-2, 0);
            CR_PIXEL(0, -1);
            CR_PIXEL(0, -2);
            CR_PIXEL(1, 0);
            CR_PIXEL(2, 0);
            CR_PIXEL(0, 1);
            CR_PIXEL(0, 2);

            TextureAtlasEntry &block_texture = block_textures[current_block][BLOCK_FRONT];
            drawTexture(terrain, block_texture.left - 1, block_texture.top - 1, screen, 0, 0, 16, 16);

            //Finished drawing GUI

            nglDisplay();

            if(keyPressed(KEY_NSPIRE_ESC))
                break;

            if(keyPressed(KEY_NSPIRE_7)) //Put block down
            {
                Position res; AABB::SIDE side;
                GLFix dx = fast_sin(yr)*fast_cos(xr), dy = -fast_sin(xr), dz = fast_cos(yr)*fast_cos(xr);
                if(world.intersectRay(x, y + eye_pos, z, dx, dy, dz, res, side))
                {
                    switch(side)
                    {
                    case AABB::BACK:
                        ++res.z;
                        break;
                    case AABB::FRONT:
                        --res.z;
                        break;
                    case AABB::LEFT:
                        --res.x;
                        break;
                    case AABB::RIGHT:
                        ++res.x;
                        break;
                    case AABB::BOTTOM:
                        --res.y;
                        break;
                    case AABB::TOP:
                        ++res.y;
                        break;
                    default:
                        break;
                    }
                    if(!world.intersect(aabb))
                    {
                        world.setBlock(res.x, res.y, res.z, current_block);
                        if(world.intersect(aabb))
                            world.setBlock(res.x, res.y, res.z, BLOCK_AIR);
                    }
                }
            }
            else if(keyPressed(KEY_NSPIRE_9)) //Remove block
            {
                Position res; AABB::SIDE side;
                GLFix dx = fast_sin(yr)*fast_cos(xr), dy = -fast_sin(xr), dz = fast_cos(yr)*fast_cos(xr);
                if(world.intersectRay(x, y + eye_pos, z, dx, dy, dz, res, side))
                    world.setBlock(res.x, res.y, res.z, BLOCK_AIR);
            }

            GLFix dx = 0, dz = 0;

            if(keyPressed(KEY_NSPIRE_8)) //Forward
            {
                GLFix dx1, dz1;
                getForward(&dx1, &dz1);

                dx += dx1;
                dz += dz1;
            }
            else if(keyPressed(KEY_NSPIRE_2)) //Backward
            {
                GLFix dx1, dz1;
                getForward(&dx1, &dz1);

                dx -= dx1;
                dz -= dz1;
            }

            if(keyPressed(KEY_NSPIRE_4)) //Left
            {
                GLFix dx1, dz1;
                getRight(&dx1, &dz1);

                dx -= dx1;
                dz -= dz1;
            }
            else if(keyPressed(KEY_NSPIRE_6)) //Right
            {
                GLFix dx1, dz1;
                getRight(&dx1, &dz1);

                dx += dx1;
                dz += dz1;
            }

            if(!world.intersect(aabb))
            {
                AABB aabb_moved = aabb;
                aabb_moved.low_x += dx;
                aabb_moved.high_x += dx;

                if(!world.intersect(aabb_moved))
                {
                    x += dx;
                    aabb = aabb_moved;
                }

                aabb_moved = aabb;
                aabb_moved.low_z += dz;
                aabb_moved.high_z += dz;

                if(!world.intersect(aabb_moved))
                {
                    z += dz;
                    aabb = aabb_moved;
                }

                aabb.low_y += vy;
                aabb.high_y += vy;

                can_jump = world.intersect(aabb);

                if(!can_jump)
                    y += vy;
                else if(vy > GLFix(0))
                {
                    can_jump = false;
                    vy = 0;
                }
                else
                    vy = 0;

                vy -= 5;
            }

            if(keyPressed(KEY_NSPIRE_5) && can_jump) //Jump
            {
                vy = 50;
                can_jump = false;
            }

            touchpad_report_t touchpad;
            touchpad_scan(&touchpad);

            if(touchpad.pressed)
            {
                switch(touchpad.arrow)
                {
                case TPAD_ARROW_DOWN:
                    xr += incr/3;
                    break;
                case TPAD_ARROW_UP:
                    xr -= incr/3;
                    break;
                case TPAD_ARROW_LEFT:
                    yr -= incr/3;
                    break;
                case TPAD_ARROW_RIGHT:
                    yr += incr/3;
                    break;
                case TPAD_ARROW_RIGHTDOWN:
                    xr += incr/3;
                    yr += incr/3;
                    break;
                case TPAD_ARROW_UPRIGHT:
                    xr -= incr/3;
                    yr += incr/3;
                    break;
                case TPAD_ARROW_DOWNLEFT:
                    xr += incr/3;
                    yr -= incr/3;
                    break;
                case TPAD_ARROW_LEFTUP:
                    xr -= incr/3;
                    yr -= incr/3;
                    break;
                }
            }
            else if(tp_had_contact && touchpad.contact)
            {
                yr += (touchpad.x - tp_last_x) / 50;
                xr -= (touchpad.y - tp_last_y) / 50;
            }

            tp_had_contact = touchpad.contact;
            tp_last_x = touchpad.x;
            tp_last_y = touchpad.y;

            if(key_held_down)
                key_held_down = keyPressed(KEY_NSPIRE_1) || keyPressed(KEY_NSPIRE_3) || keyPressed(KEY_NSPIRE_PERIOD) || keyPressed(KEY_NSPIRE_MINUS) || keyPressed(KEY_NSPIRE_PLUS);

            else if(keyPressed(KEY_NSPIRE_1)) //Switch block type
            {
                --current_block;
                if(current_block < 0)
                    current_block = BLOCK_MAX - 1;

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_3))
            {
                ++current_block;
                if(current_block == BLOCK_MAX)
                    current_block = 1;

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_PERIOD))
            {
                gamestate = BLOCK_LIST;

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_MINUS))
            {
                int fov = world.fieldOfView() - 1;
                world.setFieldOfView(fov < 1 ? 1 : fov);

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_PLUS))
            {
                world.setFieldOfView(world.fieldOfView() + 1);

                key_held_down = true;
            }

            if(menu_held_down)
                menu_held_down = keyPressed(KEY_NSPIRE_MENU);
            else if(keyPressed(KEY_NSPIRE_MENU))
            {
                gamestate = MENU;
                menu_held_down = true;
            }
        }
        else if(gamestate == MENU)
        {
            //Save background, we don't want to render the world
            if(save_background)
            {
                memcpy(background, screen.bitmap, sizeof(COLOR)*SCREEN_WIDTH*SCREEN_HEIGHT);
                save_background = false;

                menu_selected_item = SAVE_WORLD;
                menu_width_visible = 0;
                menu_open = true;
                menu_held_down = true;
            }

            //Slide menu
            if(menu_open && menu_width_visible < menu_with_selection.width)
                menu_width_visible += 10;
            else if(!menu_open && menu_width_visible > 0)
                menu_width_visible -= 10;

            if(menu_width_visible < 0)
                menu_width_visible = 0;
            else if(menu_width_visible > menu.width)
                menu_width_visible = menu.width;

            __builtin_memcpy(screen.bitmap, background, sizeof(COLOR)*SCREEN_WIDTH*SCREEN_HEIGHT);

            __builtin_memcpy(menu_with_selection.bitmap, menu.bitmap, sizeof(COLOR)*menu.width*menu.height);

            int selection_y;
            switch(menu_selected_item)
            {
            default:
            case NEW_WORLD:
                selection_y = 33;
                break;
            case LOAD_WORLD:
                selection_y = 62;
                break;
            case SAVE_WORLD:
                selection_y = 88;
                break;
            case SAVE_AND_EXIT:
                selection_y = 116;
                break;
            case EXIT:
                selection_y = 144;
                break;
            }

            drawTexture(selection, 0, 0, menu_with_selection, 0, selection_y, selection.width, selection.height);

            drawTransparentTexture(menu_with_selection, 0, 0, screen, SCREEN_WIDTH - menu_width_visible, 0, menu_width_visible, menu_with_selection.height);

            nglDisplay();

            //Switch to WORLD GAMESTATE if menu closed
            if(!menu_open && menu_width_visible == 0)
            {
                gamestate = WORLD;
                save_background = true;
            }

            if(key_held_down)
                key_held_down = keyPressed(KEY_NSPIRE_8) || keyPressed(KEY_NSPIRE_2) || keyPressed(KEY_NSPIRE_5);
            else if(keyPressed(KEY_NSPIRE_8))
            {
                --menu_selected_item;
                if(menu_selected_item < 0)
                    menu_selected_item = MENU_ITEM_MAX - 1;

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_2))
            {
                ++menu_selected_item;
                if(menu_selected_item == MENU_ITEM_MAX)
                    menu_selected_item = 0;

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_5))
            {
                switch(menu_selected_item)
                {
                case NEW_WORLD:
                    x = z = 0;
                    y = BLOCK_SIZE * Chunk::SIZE * World::HEIGHT + BLOCK_SIZE;
                    world.clear();
                    world.generateSeed();
                    break;

                case LOAD_WORLD:
                    savefile = fopen(savefile_name, "rb");
                    if(savefile)
                        loadFile(savefile);

                    fclose(savefile);
                    break;

                case SAVE_WORLD:
                    unlink(savefile_name);
                    savefile = fopen(savefile_name, "wb");
                    if(savefile)
                        saveFile(savefile);

                    fclose(savefile);
                    break;

                case SAVE_AND_EXIT:
                    goto saveandexit;

                case EXIT:
                    goto exit;
                }

                menu_open = false;
                key_held_down = true;
            }

            if(menu_held_down)
                menu_held_down = keyPressed(KEY_NSPIRE_MENU);
            else if(keyPressed(KEY_NSPIRE_MENU))
            {
                menu_open = false;
                menu_held_down = true;
            }
        }
        else if(gamestate == BLOCK_LIST)
        {
            if(save_background)
            {
                memcpy(background, screen.bitmap, sizeof(COLOR)*SCREEN_WIDTH*SCREEN_HEIGHT);
                save_background = false;
            }

            memcpy(screen.bitmap, background, sizeof(COLOR)*SCREEN_WIDTH*SCREEN_HEIGHT);

            memset(black.bitmap, 0, sizeof(COLOR)*black.width*black.height);
            drawTexture(selection, 0, 0, black, ((current_block-1) % 8) * 32 + 13 - selection.width, ((current_block-1) / 8) * 32 + 17, selection.width, selection.height);

            drawTransparentTexture(black, 0, 0, screen, 25, 25, SCREEN_WIDTH - 50, SCREEN_HEIGHT - 50);

            int block = BLOCK_AIR + 1;
            int screen_x = 39, screen_y = 39;
            for(int y = 0; y < 8; y++, screen_y += 32)
            {
                screen_x = 39;
                for(int x = 0; x < 8; x++, screen_x += 32)
                {
                    TextureAtlasEntry &tae = block_textures[block][BLOCK_FRONT];
                    drawTexture(terrain, tae.left - 1, tae.top - 1, screen, screen_x, screen_y, 16, 16);

                    block++;
                    if(block == BLOCK_MAX)
                        goto end;
                }
            }

            end:

            nglDisplay();

            if(key_held_down)
                key_held_down = keyPressed(KEY_NSPIRE_PERIOD) || keyPressed(KEY_NSPIRE_8) || keyPressed(KEY_NSPIRE_4) || keyPressed(KEY_NSPIRE_6) || keyPressed(KEY_NSPIRE_2);
            else if(keyPressed(KEY_NSPIRE_PERIOD))
            {
                gamestate = WORLD;
                save_background = true;

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_2))
            {
                current_block += 8;
                if((current_block-1) >= BLOCK_MAX)
                    current_block -= 8;

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_8))
            {
                if((current_block-1) >= 8)
                    current_block -= 8;

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_4))
            {
                if((current_block-1) % 8 != 0)
                    current_block--;

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_6))
            {
                if((current_block-1) % 8 != 7 && current_block < BLOCK_MAX - 1)
                    current_block++;

                key_held_down = true;
            }
        }
    }

    saveandexit:

    //truncate(savefile_name, 0);

    savefile = fopen(savefile_name, "wb");
    if(!savefile || !saveFile(savefile))
        printf("Failed to save world!\n");
    else
        printf("World successfully saved.\n");

    fclose(savefile);

    exit:

    nglUninit();

    delete[] black.bitmap;
    delete[] menu_with_selection.bitmap;
    delete[] screen.bitmap;

    return 0;
}
