#ifndef INVENTORY_H
#define INVENTORY_H

#include "gl.h"
#include "terrain.h"

class Inventory
{
public:
    Inventory();

    void draw(TEXTURE &tex);

    static unsigned int height();
    BLOCK_WDATA &currentSlot();

    void previousSlot();
    void nextSlot();

    static constexpr int slot_count = 5;
    BLOCK_WDATA entries[slot_count] = { BLOCK_STONE, BLOCK_GRASS, BLOCK_PLANKS_NORMAL, BLOCK_TORCH, BLOCK_FLOWER };
    int current_slot = 0;
};

extern Inventory current_inventory;

#endif // INVENTORY_H
