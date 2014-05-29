#ifndef BLOCKLISTTASK_H
#define BLOCKLISTTASK_H

#include "task.h"

class BlockListTask : public Task
{
public:
    BlockListTask();
    virtual ~BlockListTask();

    virtual void makeCurrent() override;

    virtual void render() override;
    virtual void logic() override;

    int current_selection;

private:
    static const unsigned int blocklist_width = SCREEN_WIDTH - 2*25, blocklist_left = (SCREEN_WIDTH - blocklist_width) / 2;
    static unsigned int blocklist_height, blocklist_top;
    //Black texture as background
    static TEXTURE *blocklist_background;
};

extern BlockListTask block_list_task;

#endif // BLOCKLISTTASK_H
