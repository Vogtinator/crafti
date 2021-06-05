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

    for(int dist = 1; dist <= 3; ++dist)
    {
        for(int x = -dist; x <= dist; ++x)
            for(int y = -dist; y <= dist; ++y)
                for(int z = -dist; z <= dist; ++z)
                {
                    if(local_y + y + Chunk::SIZE * c.y < 1 || local_y + y + Chunk::SIZE * c.y >= World::HEIGHT*Chunk::SIZE || round(sqrt(x*x + y*y + z*z)) != dist)
                        continue;

                    //Explode other TNT blocks
                    if(getBLOCK(c.getGlobalBlockRelative(local_x + x, local_y + y, local_z + z)) == BLOCK_TNT)
                        explode(local_x + x, local_y + y, local_z + z, c);
                    else if(getBLOCK(c.getGlobalBlockRelative(local_x + x, local_y + y, local_z + z)) != BLOCK_BEDROCK)
                        c.changeGlobalBlockRelative(local_x + x, local_y + y, local_z + z, BLOCK_AIR);
                }
    }
}
