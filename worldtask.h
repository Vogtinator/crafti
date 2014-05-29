#ifndef WORLDTASK_H
#define WORLDTASK_H

#include "task.h"
#include "world.h"
#include "gl.h"
#include "aabb.h"

class WorldTask : public Task
{
public:
    WorldTask();

    virtual void makeCurrent() override;

    virtual void logic() override;
    virtual void render() override;

    void resetWorld();

    World world;
    GLFix x, y, z, xr, yr;

    static constexpr GLFix player_width = BLOCK_SIZE*0.8f, player_height = BLOCK_SIZE*1.8f, eye_pos = BLOCK_SIZE*1.6f, incr = 20;

private:
    void crosshairPixel(int x, int y);

    void getForward(GLFix *x, GLFix *z);
    void getRight(GLFix *x, GLFix *z);

    AABB aabb;
    bool can_jump = false, tp_had_contact = false;
    int tp_last_x = 0, tp_last_y = 0;
    GLFix vy = 0; //Y-Velocity for gravity and jumps
    bool in_water = false;
    Position selection_pos; AABB::SIDE selection_side; Position selection_pos_abs; bool do_test = true; //For intersectsRay
};

extern WorldTask world_task;

#endif // WORLDTASK_H
