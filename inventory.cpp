#include "inventory.h"

#include "texturetools.h"
#include "blockrenderer.h"

#include "textures/inventory.h"

Inventory current_inventory;

Inventory::Inventory()
{
}

void Inventory::draw(TEXTURE &tex)
{
    drawTextureOverlay(inventory, 0, 0, tex, (SCREEN_WIDTH - inventory.width) / 2, SCREEN_HEIGHT - inventory.height, inventory.width, inventory.height);
    for(unsigned int i = 0; i < 5; ++i)
    {
        const BLOCK_WDATA block = entries[i];
        global_block_renderer.drawPreview(block, tex, (SCREEN_WIDTH - inventory.width) / 2 + 10 + i * 34, SCREEN_HEIGHT - inventory.height + (getBLOCK(block) == BLOCK_DOOR ? 2 : 10));
    }
    drawTexture(*glass_big, tex, 0, 0, 32, 32, (SCREEN_WIDTH - inventory.width) / 2 + 2 + current_slot * 34, SCREEN_HEIGHT - inventory.height + 2, 32, 32);
}

unsigned int Inventory::height()
{
    return inventory.height;
}

BLOCK_WDATA& Inventory::currentSlot()
{
    return entries[current_slot];
}

void Inventory::previousSlot()
{
    --current_slot;
    if(current_slot < 0)
        current_slot = slot_count - 1;
}

void Inventory::nextSlot()
{
    ++current_slot;
    if(current_slot >= slot_count)
        current_slot = 0;
}
