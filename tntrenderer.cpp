#include <cstdlib>

#include "world.h"
#include "tntrenderer.h"

void TNTRenderer::tick(const BLOCK_WDATA /*block*/, int local_x, int local_y, int local_z, Chunk &c)
{
    if(c.isBlockPowered(local_x, local_y, local_z))
        explode(local_x, local_y, local_z, c);
}

void TNTRenderer::explode(const int local_x, const int local_y, const int local_z, Chunk &c)
{
    c.setGlobalBlockRelative(local_x, local_y, local_z, BLOCK_AIR);

    // Destroy everything in a sphere with a 3 block radius
    const int dist = 3;

    for(int x = -dist; x <= dist; ++x)
        for(int y = -dist; y <= dist; ++y)
            for(int z = -dist; z <= dist; ++z)
            {
                if(local_y + y + Chunk::SIZE * c.y < 1 || local_y + y + Chunk::SIZE * c.y >= World::HEIGHT*Chunk::SIZE)
                    continue;

                if(x*x + y*y + z*z > dist*dist)
                    continue;

                //Explode other TNT blocks
                auto block = getBLOCK(c.getGlobalBlockRelative(local_x + x, local_y + y, local_z + z));
                if(block == BLOCK_TNT)
                    explode(local_x + x, local_y + y, local_z + z, c);
                else if(block != BLOCK_BEDROCK && block != BLOCK_AIR)
                    c.changeGlobalBlockRelative(local_x + x, local_y + y, local_z + z, BLOCK_AIR);
            }
}
