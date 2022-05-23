#include <sys/stat.h>

#include "worldtask.h"

#include "aabb.h"
#include "blockrenderer.h"
#include "blocklisttask.h"
#include "menutask.h"
#include "settingstask.h"
#include "fastmath.h"
#include "font.h"
#include "inventory.h"

#include "textures/blockselection.h"

WorldTask world_task;

constexpr GLFix  WorldTask::player_width,  WorldTask::player_height,  WorldTask::eye_pos;

void WorldTask::makeCurrent()
{
    Task::background_saved = false;

    Task::makeCurrent();
}

//Invert pixel at (x|y) relative to the center of the screen
void WorldTask::crosshairPixel(int x, int y)
{
    int pos = SCREEN_WIDTH/2 + x + (SCREEN_HEIGHT/2 + y)*SCREEN_WIDTH;
    screen->bitmap[pos] = ~screen->bitmap[pos];
}

void WorldTask::getForward(GLFix *x, GLFix *z)
{
    *x = GLFix(fast_sin(yr)) * speed();
    *z = GLFix(fast_cos(yr)) * speed();
}

void WorldTask::getRight(GLFix *x, GLFix *z)
{
    *x = GLFix(fast_sin((yr + 90).normaliseAngle())) * speed();
    *z = GLFix(fast_cos((yr + 90).normaliseAngle())) * speed();
}

GLFix WorldTask::speed()
{
    return 10 * settings_task.getValue(SettingsTask::SPEED) + 10;
}

void WorldTask::logic()
{
    GLFix dx = 0, dz = 0;
#ifndef DEBUG
    if(keyPressed(KEY_NSPIRE_8)) //Forward
#else
    if(keyPressed(KEY_NSPIRE_8) || keyPressed(KEY_NSPIRE_W))
#endif
    {
        GLFix dx1, dz1;
        getForward(&dx1, &dz1);

        dx += dx1;
        dz += dz1;
    }
    
#ifndef DEBUG
    else if(keyPressed(KEY_NSPIRE_2)) //Backwards
#else
    else if(keyPressed(KEY_NSPIRE_2) || keyPressed(KEY_NSPIRE_S))
#endif
    {
        GLFix dx1, dz1;
        getForward(&dx1, &dz1);

        dx -= dx1;
        dz -= dz1;
    }

    
#ifndef DEBUG
    if(keyPressed(KEY_NSPIRE_4)) //Left
#else
    if(keyPressed(KEY_NSPIRE_4) || keyPressed(KEY_NSPIRE_A))
#endif
    {
        GLFix dx1, dz1;
        getRight(&dx1, &dz1);

        dx -= dx1;
        dz -= dz1;
    }
#ifndef DEBUG
    else if(keyPressed(KEY_NSPIRE_6)) //Right
#else
    else if(keyPressed(KEY_NSPIRE_6) || keyPressed(KEY_NSPIRE_D))
#endif
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

        aabb_moved = aabb;
        aabb_moved.low_y += vy;
        aabb_moved.high_y += vy;

        can_jump = world.intersect(aabb_moved);

        if(!can_jump)
        {
            y += vy;
            aabb = aabb_moved;
        }
        else if(vy > GLFix(0))
        {
            can_jump = false;
            vy = 0;
        }
        else
            vy = 0;

        vy -= 5;

        in_water = getBLOCK(world.getBlock((x / BLOCK_SIZE).floor(), ((y + eye_pos) / BLOCK_SIZE).floor(), (z / BLOCK_SIZE).floor())) == BLOCK_WATER;

        if(in_water)
            can_jump = true;
    }

#ifndef DEBUG
    if(keyPressed(KEY_NSPIRE_5) && can_jump) //Jump
#else
    if((keyPressed(KEY_NSPIRE_5) || keyPressed(KEY_NSPIRE_SPACE)) && can_jump)
#endif
    {
        vy = 50;
        can_jump = false;
    }

    if(has_touchpad)
    {
        touchpad_report_t touchpad;
        touchpad_scan(&touchpad);

        if(touchpad.pressed)
        {
            switch(touchpad.arrow)
            {
            case TPAD_ARROW_DOWN:
                xr += speed()/3;
                break;
            case TPAD_ARROW_UP:
                xr -= speed()/3;
                break;
            case TPAD_ARROW_LEFT:
                yr -= speed()/3;
                break;
            case TPAD_ARROW_RIGHT:
                yr += speed()/3;
                break;
            case TPAD_ARROW_RIGHTDOWN:
                xr += speed()/3;
                yr += speed()/3;
                break;
            case TPAD_ARROW_UPRIGHT:
                xr -= speed()/3;
                yr += speed()/3;
                break;
            case TPAD_ARROW_DOWNLEFT:
                xr += speed()/3;
                yr -= speed()/3;
                break;
            case TPAD_ARROW_LEFTUP:
                xr -= speed()/3;
                yr -= speed()/3;
                break;
            }
        }
        else if(tp_had_contact && touchpad.contact)
        {
            yr += (touchpad.x - tp_last_x) / (1000 / speed());
            xr -= (touchpad.y - tp_last_y) / (1000 / speed());
        }

        tp_had_contact = touchpad.contact;
        tp_last_x = touchpad.x;
        tp_last_y = touchpad.y;
    }
    else
    {
        if(keyPressed(KEY_NSPIRE_UP))
            xr -= speed()/3;
        else if(keyPressed(KEY_NSPIRE_DOWN))
            xr += speed()/3;

        if(keyPressed(KEY_NSPIRE_LEFT))
            yr -= speed()/3;
        else if(keyPressed(KEY_NSPIRE_RIGHT))
            yr += speed()/3;
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

    //Do test only on every second frame, it's expensive
    if(do_test)
    {
        GLFix dx = fast_sin(yr)*fast_cos(xr), dy = -fast_sin(xr), dz = fast_cos(yr)*fast_cos(xr);
        GLFix dist;
        if(!world.intersectsRay(x, y + eye_pos, z, dx, dy, dz, selection_pos, selection_side, dist, in_water))
            selection_side = AABB::NONE;
        else
            selection_pos_abs = {x + dx * dist, y + eye_pos + dy * dist, z + dz * dist};
    }

    world.setPosition(x, y, z);

    do_test = !do_test;

    if(key_held_down)        
#ifndef DEBUG
    key_held_down = keyPressed(KEY_NSPIRE_ESC) || keyPressed(KEY_NSPIRE_7) || keyPressed(KEY_NSPIRE_9) || keyPressed(KEY_NSPIRE_1) || keyPressed(KEY_NSPIRE_3) || keyPressed(KEY_NSPIRE_PERIOD) || keyPressed(KEY_NSPIRE_MINUS) || keyPressed(KEY_NSPIRE_PLUS) || keyPressed(KEY_NSPIRE_MENU);
#else
    key_held_down = keyPressed(KEY_NSPIRE_ESC) || keyPressed(KEY_NSPIRE_7) || keyPressed(KEY_NSPIRE_9) || keyPressed(KEY_NSPIRE_1) || keyPressed(KEY_NSPIRE_3) || keyPressed(KEY_NSPIRE_PERIOD) || keyPressed(KEY_NSPIRE_MINUS) || keyPressed(KEY_NSPIRE_PLUS) || keyPressed(KEY_NSPIRE_MENU) || keyPressed(KEY_NSPIRE_Q) || keyPressed(KEY_NSPIRE_E) || keyPressed(KEY_NSPIRE_Q) || keyPressed(KEY_NSPIRE_W) || keyPressed(KEY_NSPIRE_A) || keyPressed(KEY_NSPIRE_S) || keyPressed(KEY_NSPIRE_D) || keyPressed(KEY_NSPIRE_Z) || keyPressed(KEY_NSPIRE_X) || keyPressed(KEY_NSPIRE_C);
#endif

    else if(keyPressed(KEY_NSPIRE_ESC) || keyPressed(KEY_NSPIRE_HOME)) //Save & Exit
    {
        save();
        Task::running = false;
        return;
    }
#ifndef DEBUG
    else if(keyPressed(KEY_NSPIRE_7)) //Put block down
#else
    else if(keyPressed(KEY_NSPIRE_7) || keyPressed(KEY_NSPIRE_Q))
#endif
    {
        key_held_down = true;

        if(selection_side == AABB::NONE)
            return;

        if(world.intersect(aabb))
            return;

        if(world.blockAction(selection_pos.x, selection_pos.y, selection_pos.z))
            return;

        BLOCK_WDATA current_block = world.getBlock(selection_pos.x, selection_pos.y, selection_pos.z),
                    block_to_place = current_inventory.currentSlot();

        // When placing fluid onto a non-full fluid block of the same type, "fill" it
        if(current_block != block_to_place
           && ((getBLOCK(current_block) == BLOCK_WATER && getBLOCK(block_to_place) == BLOCK_WATER)
               || (getBLOCK(current_block) == BLOCK_LAVA && getBLOCK(block_to_place) == BLOCK_LAVA)))
        {
            world.changeBlock(selection_pos.x, selection_pos.y, selection_pos.z, block_to_place);
            return;
        }

        VECTOR3 pos = selection_pos;
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

        current_block = world.getBlock(pos.x, pos.y, pos.z);

        //Only set the block if there's air
        if(current_block == BLOCK_AIR || (in_water && getBLOCK(current_block) == BLOCK_WATER))
        {
            if(!global_block_renderer.isOriented(block_to_place))
                world.changeBlock(pos.x, pos.y, pos.z, block_to_place);
            else
            {
                AABB::SIDE side = selection_side;
                //If the block is not fully oriented and has been placed on top or bottom of another block, determine the orientation by yr
                if(!global_block_renderer.isFullyOriented(block_to_place) && (side == AABB::TOP || side == AABB::BOTTOM))
                    side = yr < GLFix(45) ? AABB::FRONT : yr < GLFix(135) ? AABB::LEFT : yr < GLFix(225) ? AABB::BACK : yr < GLFix(315) ? AABB::RIGHT : AABB::FRONT;

                world.changeBlock(pos.x, pos.y, pos.z, getBLOCKWDATA(block_to_place, side)); //AABB::SIDE is compatible to BLOCK_SIDE
            }

            //If the player is stuck now, it's because of the block change, so remove it again
            if(world.intersect(aabb))
                world.changeBlock(pos.x, pos.y, pos.z, current_block);
        }
    }
#ifndef DEBUG
    else if(keyPressed(KEY_NSPIRE_9)) //Remove block
#else
    else if(keyPressed(KEY_NSPIRE_9) || keyPressed(KEY_NSPIRE_E))
#endif
    {
        if(selection_side != AABB::NONE && world.getBlock(selection_pos.x, selection_pos.y, selection_pos.z) != BLOCK_BEDROCK)
        {
            world.spawnDestructionParticles(selection_pos.x, selection_pos.y, selection_pos.z);
            world.changeBlock(selection_pos.x, selection_pos.y, selection_pos.z, BLOCK_AIR);
        }

        key_held_down = true;
    }
#ifndef DEBUG
    else if(keyPressed(KEY_NSPIRE_1)) //Switch inventory slot
#else
    else if(keyPressed(KEY_NSPIRE_Z))
#endif
    {
        current_inventory.previousSlot();

        key_held_down = true;
    }
#ifndef DEBUG
    else if(keyPressed(KEY_NSPIRE_3))
#else
    else if(keyPressed(KEY_NSPIRE_3) || keyPressed(KEY_NSPIRE_C))
#endif
    {
        current_inventory.nextSlot();

        key_held_down = true;
    }
#ifndef DEBUG
    else if(keyPressed(KEY_NSPIRE_PERIOD)) //Open list of blocks (or take screenshot with Ctrl + .)
#else
    else if(keyPressed(KEY_NSPIRE_PERIOD) || keyPressed(KEY_NSPIRE_X))
#endif
    {
        if(keyPressed(KEY_NSPIRE_CTRL))
        {
            //Find a filename that doesn't exist
            char buf[45];

            unsigned int i;
            for(i = 0; i <= 999; ++i)
            {
                snprintf(buf, sizeof(buf), "/documents/ndless/screenshot_%d.ppm.tns", i);

                struct stat stat_buf;
                if(stat(buf, &stat_buf) != 0)
                    break;
            }

            if(i > 999 || !saveTextureToFile(*screen, buf))
                setMessage("Screenshot failed!");
            else
            {
                snprintf(message, sizeof(message), "Screenshot taken (%d)!", i);
                message_timeout = 20;
            }
        }
        else
        {
            draw_inventory = false;
            render();
            draw_inventory = true;
            block_list_task.makeCurrent();
        }

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
    else if(keyPressed(KEY_NSPIRE_MENU))
    {
        menu_task.makeCurrent();

        key_held_down = true;
    }
}

void WorldTask::render()
{
    aabb = {x - player_width/2, y, z - player_width/2, x + player_width/2, y + player_height, z + player_width/2};
    //printf("X: %f Y: %f Z: %f XR: %d YR: %d\n", x.toFloat(), y.toFloat(), z.toFloat(), xr.toInt(), yr.toInt());

    glColor3f(0.4f, 0.6f, 0.8f); //Blue background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    //Inverted rotation of the world
    nglRotateX((GLFix(359) - xr).normaliseAngle());
    nglRotateY((GLFix(359) - yr).normaliseAngle());
    //Inverted translation of the world
    glTranslatef(-x, -y - eye_pos, -z);

    glBindTexture(terrain_current);

    world.render();

    //Draw indication
    glBindTexture(&blockselection);

    glBegin(GL_QUADS);

    //Do a quick animation
    const unsigned int blockselection_frame_width = blockselection.width / blockselection_frames;
    TextureAtlasEntry tex = textureArea(0, 0, blockselection_frame_width, blockselection.height);
    tex.left += blockselection_frame_width * blockselection_frame;
    tex.right += blockselection_frame_width * blockselection_frame;

    //Only increment the frame nr each 5 frames
    if(++blockselection_frame_fraction == 5)
    {
        blockselection_frame_fraction = 0;

        if(++blockselection_frame == blockselection_frames)
            blockselection_frame = 0;
    }

    const GLFix indicator_x = selection_pos.x * BLOCK_SIZE, indicator_y = selection_pos.y * BLOCK_SIZE, indicator_z = selection_pos.z * BLOCK_SIZE;
    const GLFix selection_offset = 3; //Needed to prevent Z-fighting
    switch(selection_side)
    {
    case AABB::FRONT:
        nglAddVertex({indicator_x, indicator_y, selection_pos_abs.z - selection_offset, tex.left, tex.bottom, TEXTURE_TRANSPARENT});
        nglAddVertex({indicator_x, indicator_y + BLOCK_SIZE, selection_pos_abs.z - selection_offset, tex.left, tex.top, TEXTURE_TRANSPARENT});
        nglAddVertex({indicator_x + BLOCK_SIZE, indicator_y + BLOCK_SIZE, selection_pos_abs.z - selection_offset, tex.right, tex.top, TEXTURE_TRANSPARENT});
        nglAddVertex({indicator_x + BLOCK_SIZE, indicator_y, selection_pos_abs.z - selection_offset, tex.right, tex.bottom, TEXTURE_TRANSPARENT});
        break;
    case AABB::BACK:
        nglAddVertex({indicator_x + BLOCK_SIZE, indicator_y, selection_pos_abs.z + selection_offset, tex.left, tex.bottom, TEXTURE_TRANSPARENT});
        nglAddVertex({indicator_x + BLOCK_SIZE, indicator_y + BLOCK_SIZE, selection_pos_abs.z + selection_offset, tex.left, tex.top, TEXTURE_TRANSPARENT});
        nglAddVertex({indicator_x, indicator_y + BLOCK_SIZE, selection_pos_abs.z + selection_offset, tex.right, tex.top, TEXTURE_TRANSPARENT});
        nglAddVertex({indicator_x, indicator_y, selection_pos_abs.z + selection_offset, tex.right, tex.bottom, TEXTURE_TRANSPARENT});
        break;
    case AABB::RIGHT:
        nglAddVertex({selection_pos_abs.x + selection_offset, indicator_y, indicator_z, tex.right, tex.bottom, TEXTURE_TRANSPARENT});
        nglAddVertex({selection_pos_abs.x + selection_offset, indicator_y + BLOCK_SIZE, indicator_z, tex.right, tex.top, TEXTURE_TRANSPARENT});
        nglAddVertex({selection_pos_abs.x + selection_offset, indicator_y + BLOCK_SIZE, indicator_z + BLOCK_SIZE, tex.left, tex.top, TEXTURE_TRANSPARENT});
        nglAddVertex({selection_pos_abs.x + selection_offset, indicator_y, indicator_z + BLOCK_SIZE, tex.left, tex.bottom, TEXTURE_TRANSPARENT});
        break;
    case AABB::LEFT:
        nglAddVertex({selection_pos_abs.x - selection_offset, indicator_y, indicator_z + BLOCK_SIZE, tex.left, tex.bottom, TEXTURE_TRANSPARENT});
        nglAddVertex({selection_pos_abs.x - selection_offset, indicator_y + BLOCK_SIZE, indicator_z + BLOCK_SIZE, tex.left, tex.top, TEXTURE_TRANSPARENT});
        nglAddVertex({selection_pos_abs.x - selection_offset, indicator_y + BLOCK_SIZE, indicator_z, tex.right, tex.top, TEXTURE_TRANSPARENT});
        nglAddVertex({selection_pos_abs.x - selection_offset, indicator_y, indicator_z, tex.right, tex.bottom, TEXTURE_TRANSPARENT});
        break;
    case AABB::TOP:
        nglAddVertex({indicator_x, selection_pos_abs.y + selection_offset, indicator_z, tex.left, tex.bottom, TEXTURE_TRANSPARENT});
        nglAddVertex({indicator_x, selection_pos_abs.y + selection_offset, indicator_z + BLOCK_SIZE, tex.left, tex.top, TEXTURE_TRANSPARENT});
        nglAddVertex({indicator_x + BLOCK_SIZE, selection_pos_abs.y + selection_offset, indicator_z + BLOCK_SIZE, tex.right, tex.top, TEXTURE_TRANSPARENT});
        nglAddVertex({indicator_x + BLOCK_SIZE, selection_pos_abs.y + selection_offset, indicator_z, tex.right, tex.bottom, TEXTURE_TRANSPARENT});
        break;
    case AABB::BOTTOM:
        nglAddVertex({indicator_x + BLOCK_SIZE, selection_pos_abs.y - selection_offset, indicator_z, tex.left, tex.bottom, TEXTURE_TRANSPARENT});
        nglAddVertex({indicator_x + BLOCK_SIZE, selection_pos_abs.y - selection_offset, indicator_z + BLOCK_SIZE, tex.left, tex.top, TEXTURE_TRANSPARENT});
        nglAddVertex({indicator_x, selection_pos_abs.y - selection_offset, indicator_z + BLOCK_SIZE, tex.right, tex.top, TEXTURE_TRANSPARENT});
        nglAddVertex({indicator_x, selection_pos_abs.y - selection_offset, indicator_z, tex.right, tex.bottom, TEXTURE_TRANSPARENT});
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

    //Don't draw the inventory when drawing the background for BlockListTask
    if(draw_inventory)
    {
        current_inventory.draw(*screen);
        drawStringCenter(global_block_renderer.getName(current_inventory.currentSlot()), 0xFFFF, *screen, SCREEN_WIDTH / 2, SCREEN_HEIGHT - current_inventory.height() - fontHeight());
    }

    if(message_timeout > 0)
    {
        drawString(message, 0xFFFF, *screen, 2, 5);
        --message_timeout;
    }

    #ifdef FPS_COUNTER
        if(message_timeout == 0 && settings_task.getValue(SettingsTask::SHOW_FPS))
        {
            snprintf(this->message, sizeof(this->message), "FPS: %u", fps);
            message_timeout = 20;
        }
    #endif

    frame_counter++;
}

void WorldTask::resetWorld()
{
    x = z = 0;
    y = World::HEIGHT * Chunk::SIZE * BLOCK_SIZE;
    xr = yr = 0;
    world.generateSeed();
    world.clear();
}

void WorldTask::setMessage(const char *message)
{
    if(strlen(message) >= sizeof(this->message))
        return;

    strcpy(this->message, message);
    message_timeout = 20;
}
