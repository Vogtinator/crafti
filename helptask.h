#ifndef HELPTASK_H
#define HELPTASK_H

#include "task.h"

#include "gl.h"

class HelpTask : public Task
{
public:
    HelpTask();
    virtual ~HelpTask();

    virtual void makeCurrent() override;

    virtual void render() override;
    virtual void logic() override;

private:
    static constexpr int background_width = SCREEN_WIDTH - 40, background_height = SCREEN_HEIGHT - 40;
    TEXTURE *background;
};

extern HelpTask help_task;

#endif // HELPTASK_H
