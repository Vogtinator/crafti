#include <cstdlib>

#include "woolrenderer.h"

void WoolRenderer::geometryNormalBlock(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c)
{
    //TerrainAtlasEntry &tae = terrain_atlas[0][4];

    switch (static_cast<COLOUR>(getBLOCKDATA(block))) {
        case WHITE:
            TerrainAtlasEntry &tae = terrain_atlas[0][4];
            break;
        case ORANGE:
            TerrainAtlasEntry &tae = terrain_atlas[2][13];
            break;
        case MAGENTA:
            TerrainAtlasEntry &tae = terrain_atlas[2][12];
            break;
        case LIGHT_BLUE:
            TerrainAtlasEntry &tae = terrain_atlas[2][11];
            break;
        case YELLOW:
            TerrainAtlasEntry &tae = terrain_atlas[2][10];
            break;
        case LIME:
            TerrainAtlasEntry &tae = terrain_atlas[2][9];
            break;
        case PINK:
            TerrainAtlasEntry &tae = terrain_atlas[2][8];
            break;
        case GRAY:
            TerrainAtlasEntry &tae = terrain_atlas[2][7];
            break;
        case LIGHT_GRAY:
            TerrainAtlasEntry &tae = terrain_atlas[1][14];
            break;
        case CYAN:
            TerrainAtlasEntry &tae = terrain_atlas[1][13];
            break;
        case PURPLE:
            TerrainAtlasEntry &tae = terrain_atlas[1][12];
            break;
        case BLUE:
            TerrainAtlasEntry &tae = terrain_atlas[1][11];
            break;
        case BROWN:
            TerrainAtlasEntry &tae = terrain_atlas[1][10];
            break;
        case GREEN:
            TerrainAtlasEntry &tae = terrain_atlas[1][9];
            break;
        case RED:
            TerrainAtlasEntry &tae = terrain_atlas[1][8];
            break;
        case BLACK:
            TerrainAtlasEntry &tae = terrain_atlas[1][7];
            break;
    }

    BlockRenderer::renderNormalBlockSide(local_x, local_y, local_z, side, tae.current, c);
}

void WoolRenderer::drawPreview(const BLOCK_WDATA /*block*/, TEXTURE &dest, int x, int y)
{
    BlockRenderer::drawTextureAtlasEntry(*terrain_resized, terrain_atlas[4][13].resized, dest, x, y);
}

const char *WoolRenderer::getName(const BLOCK_WDATA block)
{
    switch (static_cast<COLOUR>(getBLOCKDATA(block))) {
        case WHITE:
            return "White Wool";
            break;
        case ORANGE:
            return "Orange Wool";
            break;
        case MAGENTA:
            return "Magenta Wool";
            break;
        case LIGHT_BLUE:
            return "Light Blue Wool";
            break;
        case YELLOW:
            return "Yellow Wool";
            break;
        case LIME:
            return "Lime Wool";
            break;
        case PINK:
            return "Pink Wool";
            break;
        case GRAY:
            return "Gray Wool";
            break;
        case LIGHT_GRAY:
            return "Light Gray Wool";
            break;
        case CYAN:
            return "Cyan Wool";
            break;
        case PURPLE:
            return "Purple Wool";
            break;
        case BLUE:
            return "Blue Wool";
            break;
        case BROWN:
            return "Brown Wool";
            break;
        case GREEN:
            return "Green Wool";
            break;
        case RED:
            return "Red Wool";
            break;
        case BLACK:
            return "Black Wool";
            break;
    }
}
