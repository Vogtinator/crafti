#ifndef CAKERENDERER_H
#define CAKERENDERER_H

#include "blockrenderer.h"

class CakeRenderer : public DumbBlockRenderer
{
public:
    virtual void renderSpecialBlock(const BLOCK_WDATA, GLFix x, GLFix y, GLFix z, Chunk &c) override;
    virtual void geometryNormalBlock(const BLOCK_WDATA, const int local_x, const int local_y, const int local_z, const BLOCK_SIDE side, Chunk &c) override;
    virtual bool isOpaque(const BLOCK_WDATA /*block*/) override { return false; }
    virtual bool isObstacle(const BLOCK_WDATA /*block*/) override { return true; }
    virtual bool isOriented(const BLOCK_WDATA /*block*/) override { return true; } // Oriented
    virtual bool isFullyOriented(const BLOCK_WDATA /*block*/) override { return false; } // Torch-like orientation

    virtual bool isBlockShaped(const BLOCK_WDATA /*block*/) override { return false; }
    virtual AABB getAABB(const BLOCK_WDATA, GLFix x, GLFix y, GLFix z) override;

    virtual void drawPreview(const BLOCK_WDATA block, TEXTURE &dest, int x, int y) override;

    virtual bool action(const BLOCK_WDATA block, const int local_x, const int local_y, const int local_z, Chunk &c) override;

    virtual const char* getName(const BLOCK_WDATA) override;

protected:
    GLFix getCakeSize(const BLOCK_WDATA block);

    static constexpr GLFix cake_height = BLOCK_SIZE / 16 * 9;
    static constexpr GLFix cake_width = BLOCK_SIZE / 16 * 15;

    /// Bitmap stuff
    static constexpr uint8_t cake_bit_shift = 4; // The amount to shift cake data by to give it room for the orientation or other additional data
    static constexpr uint8_t cake_data_bits = 0b111 << cake_bit_shift; // Cake uses 3 bits of data, however, orientation data is stored in the first three bits, so the cake data has to be shifted by 3 (or more)

    static constexpr uint8_t cake_max_bites = 6; // Maximum bites of cake you can have until it is eaten (after 4 bites, cake will dissapear)
};

#endif // CAKERENDERER_H
