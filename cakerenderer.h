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
    virtual void addedBlock(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c) override;

    virtual const char* getName(const BLOCK_WDATA) override;

protected:
    void setCakeEaten(const BLOCK_WDATA block, int local_x, int local_y, int local_z, Chunk &c, const uint8_t cake_eaten);

    static constexpr uint8_t cake_remaining_bits = 0b111 << 3; // It is a 3 bit value which is shifted 3 bits (3 bits 0-7, 3 bits shifted because of orientation)
    static constexpr uint8_t total_cake = 0b100;
    static constexpr GLFix cake_height = BLOCK_SIZE / 16 * 9;
    static constexpr GLFix cake_width = BLOCK_SIZE / 16 * 15;

    enum {
        CAKE_NONE=0b000,
        CAKE_ONE,
        CAKE_TWO,
        CAKE_THREE,
        CAKE_FOUR,
        CAKE_FIVE,
        CAKE_SIX,
        CAKE_SEVEN
    }
};

#endif // CAKERENDERER_H
