#include <nucleus.h>
#include <libndls.h>
#include <unistd.h>

#include "gl.h"
#include "fastmath.h"
#include "world.h"
#include "terrain.h"
#include "blockrenderer.h"

//Image resources
#include "textures/loading.h"
#include "textures/menu.h"
#include "textures/selection.h"

static GLFix xr = 0, yr = 0;
static GLFix x = 0, y = BLOCK_SIZE * Chunk::SIZE * World::HEIGHT + BLOCK_SIZE, z = 0;
static constexpr GLFix incr = 20;
static World world;
static int current_block_selection;
static TEXTURE *screen;

//isKeyPressed checks the hardware-type, but that's slow
static inline bool keyPressed(const t_key &key)
{
    return *reinterpret_cast<volatile uint16_t*>(0x900E0000 + key.tpad_row) & key.tpad_col;
}

static BLOCK_WDATA user_selectable[] = {
    BLOCK_STONE,
    BLOCK_COBBLESTONE,
    BLOCK_DIRT,
    BLOCK_GRASS,
    BLOCK_SAND,
    BLOCK_WOOD,
    BLOCK_LEAVES,
    BLOCK_PLANKS_NORMAL,
    BLOCK_PLANKS_DARK,
    BLOCK_PLANKS_BRIGHT,
    BLOCK_WALL,
    BLOCK_GLASS,
    BLOCK_DOOR,
    BLOCK_COAL_ORE,
    BLOCK_GOLD_ORE,
    BLOCK_IRON_ORE,
    BLOCK_DIAMOND_ORE,
    BLOCK_REDSTONE_ORE,
    BLOCK_IRON,
    BLOCK_GOLD,
    BLOCK_DIAMOND,
    BLOCK_GLOWSTONE,
    BLOCK_NETHERRACK,
    BLOCK_TNT,
    BLOCK_SPONGE,
    BLOCK_FURNACE,
    BLOCK_CRAFTING_TABLE,
    BLOCK_BOOKSHELF,
    BLOCK_PUMPKIN,
    getBLOCKWDATA(BLOCK_WATER, RANGE_WATER),
    getBLOCKWDATA(BLOCK_LAVA, RANGE_LAVA),
    getBLOCKWDATA(BLOCK_FLOWER, 0),
    getBLOCKWDATA(BLOCK_FLOWER, 1),
    getBLOCKWDATA(BLOCK_MUSHROOM, 0),
    getBLOCKWDATA(BLOCK_MUSHROOM, 1),
    BLOCK_SPIDERWEB,
    BLOCK_TORCH,
    BLOCK_CAKE
};

constexpr int user_selectable_count = sizeof(user_selectable)/sizeof(*user_selectable);

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

static constexpr int savefile_version = 3;

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
    LOAD_FROM_FILE(current_block_selection)

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
    SAVE_TO_FILE(current_block_selection)

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

//Invert pixel at (x|y) relative to the center of the screen
inline void crosshairPixel(int x, int y)
{
    int pos = SCREEN_WIDTH/2 + x + (SCREEN_HEIGHT/2 + y)*SCREEN_WIDTH;
    screen->bitmap[pos] = ~screen->bitmap[pos];
}

int main(int argc, char *argv[])
{
    if(!lcd_isincolor())
    {
        show_msgbox("Error", "Your LCD hasn't got enough colors :-P");
        return 1;
    }

    //Sometimes there's a clock on screen, switch that off
    __asm__ volatile("mrs r0, cpsr;"
                    "orr r0, r0, #0x80;"
                    "msr cpsr_c, r0;" ::: "r0");

    std::copy(loading.bitmap, loading.bitmap + SCREEN_HEIGHT*SCREEN_WIDTH, reinterpret_cast<COLOR*>(SCREEN_BASE_ADDRESS));

    nglInit();

    //Allocate a buffer for nGL to render to
    screen = newTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    nglSetBuffer(screen->bitmap);

    terrainInit("/documents/ndless/crafti.ppm.tns");
    glBindTexture(terrain_current);

    glLoadIdentity();

    const char savefile_path_default[] = "/documents/ndless/crafti.map.tns";
    const char* savefile_path = savefile_path_default;

    //If crafti has been started by the file extension association, use the first argument as savefile path
    if(argc > 1)
        savefile_path = argv[1];

    FILE *savefile = fopen(savefile_path, "rb");
    if(!savefile)
        puts("No previous save found.");
    else if(loadFile(savefile))
        puts("Loaded world.");
    else
        puts("Failed to load world!");

    if(savefile)
        fclose(savefile);

    savefile = nullptr;

    GAMESTATE gamestate = WORLD;
    TEXTURE *background = newTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    bool saved_background = false;

    //State for GAMESTATE WORLD
    const GLFix player_width = BLOCK_SIZE*0.8f, player_height = BLOCK_SIZE*1.8f, eye_pos = BLOCK_SIZE*1.6f;
    bool key_held_down = true, can_jump = false, tp_had_contact = false, menu_held_down = true;
    int tp_last_x = 0, tp_last_y = 0;
    GLFix vy = 0; //Y-Velocity for gravity and jumps
    Position selection_pos; AABB::SIDE selection_side; bool do_test = true; //For intersectsRay

    //State for GAMESTATE MENU
    int menu_selected_item = 0, menu_width_visible = 0;
    bool menu_open = true;
    TEXTURE *menu_with_selection = newTexture(menu.width, menu.height);

    //State for GAMESTATE BLOCK_LIST
    TEXTURE *black = newTexture(SCREEN_WIDTH - 2*25, SCREEN_HEIGHT - 2*25);

    while(true)
    {
        //Reset loading message
        drawLoadingtext(-1);

        if(gamestate == WORLD)
        {
            AABB aabb(x - player_width/2, y, z - player_width/2, x + player_width/2, y + player_height, z + player_width/2);
            //printf("X: %d Y: %d Z: %d XR: %d YR: %d\n", x.toInt(), y.toInt(), z.toInt(), xr.toInt(), yr.toInt());

            glColor3f(0.4f, 0.6f, 0.8f); //Blue background
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();

            //Inverted rotation of the world
            nglRotateX((GLFix(359) - xr).normaliseAngle());
            nglRotateY((GLFix(359) - yr).normaliseAngle());
            //Inverted translation of the world
            glTranslatef(-x, -y - eye_pos, -z);

            world.setPosition(x, y, z);

            world.render();

            //Draw indication
            glBegin(GL_QUADS);
            const TextureAtlasEntry &tex = block_textures[BLOCK_GLASS][BLOCK_FRONT].current; //Why not. Transparent, yet visible
            const GLFix indicator_x = selection_pos.x * BLOCK_SIZE, indicator_y = selection_pos.y * BLOCK_SIZE, indicator_z = selection_pos.z * BLOCK_SIZE;
            const GLFix selection_offset = 3; //Needed to prevent Z-fighting
            switch(selection_side)
            {
            case AABB::FRONT:
                nglAddVertex({indicator_x, indicator_y, indicator_z - selection_offset, tex.left, tex.bottom, TEXTURE_TRANSPARENT});
                nglAddVertex({indicator_x, indicator_y + BLOCK_SIZE, indicator_z - selection_offset, tex.left, tex.top, TEXTURE_TRANSPARENT});
                nglAddVertex({indicator_x + BLOCK_SIZE, indicator_y + BLOCK_SIZE, indicator_z - selection_offset, tex.right, tex.top, TEXTURE_TRANSPARENT});
                nglAddVertex({indicator_x + BLOCK_SIZE, indicator_y, indicator_z - selection_offset, tex.right, tex.bottom, TEXTURE_TRANSPARENT});
                break;
            case AABB::BACK:
                nglAddVertex({indicator_x + BLOCK_SIZE, indicator_y, indicator_z + BLOCK_SIZE + selection_offset, tex.left, tex.bottom, TEXTURE_TRANSPARENT});
                nglAddVertex({indicator_x + BLOCK_SIZE, indicator_y + BLOCK_SIZE, indicator_z + BLOCK_SIZE + selection_offset, tex.left, tex.top, TEXTURE_TRANSPARENT});
                nglAddVertex({indicator_x, indicator_y + BLOCK_SIZE, indicator_z + BLOCK_SIZE + selection_offset, tex.right, tex.top, TEXTURE_TRANSPARENT});
                nglAddVertex({indicator_x, indicator_y, indicator_z + BLOCK_SIZE + selection_offset, tex.right, tex.bottom, TEXTURE_TRANSPARENT});
                break;
            case AABB::RIGHT:
                nglAddVertex({indicator_x + BLOCK_SIZE + selection_offset, indicator_y, indicator_z, tex.right, tex.bottom, TEXTURE_TRANSPARENT});
                nglAddVertex({indicator_x + BLOCK_SIZE + selection_offset, indicator_y + BLOCK_SIZE, indicator_z, tex.right, tex.top, TEXTURE_TRANSPARENT});
                nglAddVertex({indicator_x + BLOCK_SIZE + selection_offset, indicator_y + BLOCK_SIZE, indicator_z + BLOCK_SIZE, tex.left, tex.top, TEXTURE_TRANSPARENT});
                nglAddVertex({indicator_x + BLOCK_SIZE + selection_offset, indicator_y, indicator_z + BLOCK_SIZE, tex.left, tex.bottom, TEXTURE_TRANSPARENT});
                break;
            case AABB::LEFT:
                nglAddVertex({indicator_x - selection_offset, indicator_y, indicator_z + BLOCK_SIZE, tex.left, tex.bottom, TEXTURE_TRANSPARENT});
                nglAddVertex({indicator_x - selection_offset, indicator_y + BLOCK_SIZE, indicator_z + BLOCK_SIZE, tex.left, tex.top, TEXTURE_TRANSPARENT});
                nglAddVertex({indicator_x - selection_offset, indicator_y + BLOCK_SIZE, indicator_z, tex.right, tex.top, TEXTURE_TRANSPARENT});
                nglAddVertex({indicator_x - selection_offset, indicator_y, indicator_z, tex.right, tex.bottom, TEXTURE_TRANSPARENT});
                break;
            case AABB::TOP:
                nglAddVertex({indicator_x, indicator_y + BLOCK_SIZE + selection_offset, indicator_z, tex.left, tex.bottom, TEXTURE_TRANSPARENT});
                nglAddVertex({indicator_x, indicator_y + BLOCK_SIZE + selection_offset, indicator_z + BLOCK_SIZE, tex.left, tex.top, TEXTURE_TRANSPARENT});
                nglAddVertex({indicator_x + BLOCK_SIZE, indicator_y + BLOCK_SIZE + selection_offset, indicator_z + BLOCK_SIZE, tex.right, tex.top, TEXTURE_TRANSPARENT});
                nglAddVertex({indicator_x + BLOCK_SIZE, indicator_y + BLOCK_SIZE + selection_offset, indicator_z, tex.right, tex.bottom, TEXTURE_TRANSPARENT});
                break;
            case AABB::BOTTOM:
                nglAddVertex({indicator_x + BLOCK_SIZE, indicator_y - selection_offset, indicator_z, tex.left, tex.bottom, TEXTURE_TRANSPARENT});
                nglAddVertex({indicator_x + BLOCK_SIZE, indicator_y - selection_offset, indicator_z + BLOCK_SIZE, tex.left, tex.top, TEXTURE_TRANSPARENT});
                nglAddVertex({indicator_x, indicator_y - selection_offset, indicator_z + BLOCK_SIZE, tex.right, tex.top, TEXTURE_TRANSPARENT});
                nglAddVertex({indicator_x, indicator_y - selection_offset, indicator_z, tex.right, tex.bottom, TEXTURE_TRANSPARENT});
                break;
            case AABB::NONE:
                break;
            }
            glEnd();

            glPopMatrix();

            crosshairPixel(0, 0);
            crosshairPixel(-1, 0);
            crosshairPixel(-2, 0);
            crosshairPixel(0, -1);
            crosshairPixel(0, -2);
            crosshairPixel(1, 0);
            crosshairPixel(2, 0);
            crosshairPixel(0, 1);
            crosshairPixel(0, 2);

            global_block_renderer.drawPreview(user_selectable[current_block_selection], *screen, 0, 0);

            //Now display the contents of the screen buffer
            nglDisplay();

            //Movement

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

                aabb_moved.low_y += vy;
                aabb_moved.high_y += vy;

                can_jump = world.intersect(aabb_moved);

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

            //Normalisation required for rotation with nglRotate
            yr.normaliseAngle();
            xr.normaliseAngle();

            //xr and yr are normalised, so we can't test for negative values
            if(xr > GLFix(180))
                if(xr <= GLFix(270))
                    xr = 269;

            if(xr < GLFix(180))
                if(xr >= GLFix(90))
                    xr = 89;

            tp_had_contact = touchpad.contact;
            tp_last_x = touchpad.x;
            tp_last_y = touchpad.y;

            //Do test only on every second frame, it's expensive
            if(do_test)
            {
                GLFix dxa = fast_sin(yr)*fast_cos(xr), dy = -fast_sin(xr), dza = fast_cos(yr)*fast_cos(xr);
                if(!world.intersectsRay(x, y + eye_pos, z, dxa, dy, dza, selection_pos, selection_side))
                    selection_side = AABB::NONE;
            }

            do_test = !do_test;

            if(key_held_down)
                key_held_down = keyPressed(KEY_NSPIRE_7) || keyPressed(KEY_NSPIRE_9) || keyPressed(KEY_NSPIRE_1) || keyPressed(KEY_NSPIRE_3) || keyPressed(KEY_NSPIRE_PERIOD) || keyPressed(KEY_NSPIRE_MINUS) || keyPressed(KEY_NSPIRE_PLUS);

            else if(keyPressed(KEY_NSPIRE_ESC)) //Save & Exit
                break;
            else if(keyPressed(KEY_NSPIRE_7)) //Put block down
            {
                if(selection_side != AABB::NONE)
                {
                    if(!world.blockAction(selection_pos.x, selection_pos.y, selection_pos.z))
                    {
                        Position pos = selection_pos;
                        switch(selection_side)
                        {
                        case AABB::BACK:
                            ++pos.z;
                            break;
                        case AABB::FRONT:
                            --pos.z;
                            break;
                        case AABB::LEFT:
                            --pos.x;
                            break;
                        case AABB::RIGHT:
                            ++pos.x;
                            break;
                        case AABB::BOTTOM:
                            --pos.y;
                            break;
                        case AABB::TOP:
                            ++pos.y;
                            break;
                        default:
                            puts("This can't normally happen #1");
                            break;
                        }
                        if(!world.intersect(aabb))
                        {
                            //Only set the block if there's air
                            const BLOCK_WDATA current_block = world.getBlock(pos.x, pos.y, pos.z);
                            if(current_block == BLOCK_AIR)
                            {
                                if(!global_block_renderer.isOriented(user_selectable[current_block_selection]))
                                    world.changeBlock(pos.x, pos.y, pos.z, user_selectable[current_block_selection]);
                                else
                                {
                                    AABB::SIDE side = selection_side;
                                    //If the block is not fully oriented and has been placed on top or bottom of another block, determine the orientation by yr
                                    if(!global_block_renderer.isFullyOriented(user_selectable[current_block_selection]) && (side == AABB::TOP || side == AABB::BOTTOM))
                                        side = yr < GLFix(45) ? AABB::FRONT : yr < GLFix(135) ? AABB::LEFT : yr < GLFix(225) ? AABB::BACK : yr < GLFix(315) ? AABB::RIGHT : AABB::FRONT;

                                    world.changeBlock(pos.x, pos.y, pos.z, getBLOCKWDATA(user_selectable[current_block_selection], side)); //AABB::SIDE is compatible to BLOCK_SIDE
                                }

                                //If the player is stuck now, it's because of the block change, so remove it again
                                if(world.intersect(aabb))
                                    world.changeBlock(pos.x, pos.y, pos.z, BLOCK_AIR);
                            }
                        }
                    }
                }

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_9)) //Remove block
            {
                if(selection_side != AABB::NONE && world.getBlock(selection_pos.x, selection_pos.y, selection_pos.z) != BLOCK_BEDROCK)
                    world.changeBlock(selection_pos.x, selection_pos.y, selection_pos.z, BLOCK_AIR);

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_1)) //Switch block type
            {
                --current_block_selection;
                if(current_block_selection < 0)
                    current_block_selection = user_selectable_count - 1;

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_3))
            {
                ++current_block_selection;
                if(current_block_selection == user_selectable_count)
                    current_block_selection = 0;

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_PERIOD)) //Open list of blocks
            {
                gamestate = BLOCK_LIST;

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_MINUS)) //Decrease max view distance
            {
                int fov = world.fieldOfView() - 1;
                world.setFieldOfView(fov < 1 ? 1 : fov);

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_PLUS)) //Increase max view distance
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
            if(!saved_background)
            {
                copyTexture(*screen, *background);
                saved_background = true;

                menu_selected_item = SAVE_WORLD;
                menu_width_visible = 0;
                menu_open = true;
                menu_held_down = true;
            }
            else
                copyTexture(*background, *screen);

            //Slide menu
            if(menu_open && static_cast<unsigned int>(menu_width_visible) < menu_with_selection->width)
                menu_width_visible += 10;
            else if(!menu_open && menu_width_visible > 0)
                menu_width_visible -= 10;

            if(menu_width_visible < 0)
                menu_width_visible = 0;
            else if(static_cast<unsigned int>(menu_width_visible) > menu.width)
                menu_width_visible = menu.width;

            copyTexture(menu, *menu_with_selection);

            const int selection_y[MENU_ITEM_MAX] = {33, 62, 88, 116, 144};

            drawTexture(selection, 0, 0, *menu_with_selection, 0, selection_y[menu_selected_item], selection.width, selection.height);

            drawTextureOverlay(*menu_with_selection, 0, 0, *screen, SCREEN_WIDTH - menu_width_visible, 0, menu_width_visible, menu_with_selection->height);

            nglDisplay();

            //Switch to WORLD GAMESTATE if menu closed
            if(!menu_open && menu_width_visible == 0)
            {
                gamestate = WORLD;
                saved_background = false;
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
                    savefile = fopen(savefile_path, "rb");
                    if(savefile)
                    {
                        loadFile(savefile);
                        fclose(savefile);
                    }
                    break;

                case SAVE_WORLD:
                    unlink(savefile_path);
                    savefile = fopen(savefile_path, "wb");
                    if(savefile)
                    {
                        saveFile(savefile);
                        fclose(savefile);
                    }
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
            if(!saved_background)
            {
                copyTexture(*screen, *background);
                saved_background = true;
            }
            else
                copyTexture(*background, *screen);

            const int field_width = terrain_resized->width / 16 * 2;
            const int field_height = terrain_resized->width / 16 * 2;
            const int fields_x = (SCREEN_WIDTH - 50) / field_width;
            const int fields_y = (SCREEN_WIDTH - 50) / field_height;

            std::fill(black->bitmap, black->bitmap + black->width*black->height, 0x0000);
            drawTexture(selection, 0, 0, *black, (current_block_selection % fields_x) * field_width + 13 - selection.width, (current_block_selection / fields_x) * field_height + 17, selection.width, selection.height);

            drawTextureOverlay(*black, 0, 0, *screen, 25, 25, SCREEN_WIDTH - 50, SCREEN_HEIGHT - 50);

            int block = 0;
            int screen_x = 39, screen_y = 39;
            for(int y = 0; y < fields_y; y++, screen_y += field_height)
            {
                screen_x = 39;
                for(int x = 0; x < fields_x; x++, screen_x += field_width)
                {
                    //BLOCK_DOOR is twice as high, so center it manually
                    if(getBLOCK(user_selectable[block]) == BLOCK_DOOR)
                        global_block_renderer.drawPreview(user_selectable[block], *screen, screen_x, screen_y - 8);
                    else
                        global_block_renderer.drawPreview(user_selectable[block], *screen, screen_x, screen_y);

                    block++;
                    if(block == user_selectable_count)
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
                saved_background = false;

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_2))
            {
                current_block_selection += 8;
                if(current_block_selection >= user_selectable_count)
                    current_block_selection -= 8;

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_8))
            {
                if(current_block_selection >= 8)
                    current_block_selection -= 8;

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_4))
            {
                if(current_block_selection % 8 != 0)
                    current_block_selection--;

                key_held_down = true;
            }
            else if(keyPressed(KEY_NSPIRE_6))
            {
                if(current_block_selection % 8 != 7 && current_block_selection < user_selectable_count - 1)
                    current_block_selection++;

                key_held_down = true;
            }
        }
    }

    saveandexit:

    //truncate(savefile_name, 0);

    savefile = fopen(savefile_path, "wb");
    if(!savefile || !saveFile(savefile))
        printf("Failed to save world!\n");
    else
        printf("World successfully saved.\n");

    if(savefile)
        fclose(savefile);

    exit:

    nglUninit();

    deleteTexture(black);
    deleteTexture(menu_with_selection);
    deleteTexture(screen);

    terrainUninit();

    return 0;
}
