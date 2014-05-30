#ifndef MENUTASK_H
#define MENUTASK_H

#include "gl.h"

#include "task.h"

class MenuTask : public Task
{
public:
    enum MENUITEM {
        NEW_WORLD = 0,
        LOAD_WORLD,
        SAVE_WORLD,
        SETTINGS,
        EXIT,
        HELP,
        MENU_ITEM_MAX
    };

    MenuTask();
    virtual ~MenuTask();

    virtual void makeCurrent() override;

    virtual void render() override;
    virtual void logic() override;

private:
    int menu_selected_item = 0, menu_width_visible = 0;
    bool menu_open = true;
    TEXTURE *menu_with_selection;
};

extern MenuTask menu_task;

#endif // MENUTASK_H
