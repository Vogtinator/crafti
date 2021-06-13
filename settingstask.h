#ifndef SETTINGSTASK_H
#define SETTINGSTASK_H

#include "task.h"

#include <vector>

class SettingsTask : public Task
{
public:
    struct SettingsEntry {
        const char *name;
        const char **values; //If nullptr, the numeric value is used
        unsigned int values_count;
        unsigned int current_value;
        unsigned int min_value; //Only makes sense if values == nullptr
        unsigned int step;
    };

    enum Settings {
        LEAVES = 0,
        SPEED,
        DISTANCE, //Managed by World, but can be changed here as well
        FAST_MODE,
        NEARPLANE_Z,
        TICKS_ENABLED,
        SHOW_FPS,
    };

    SettingsTask();
    virtual ~SettingsTask();

    virtual void makeCurrent() override;

    virtual void render() override;
    virtual void logic() override;

    unsigned int getValue(unsigned int entry) const;

    bool loadFromFile(FILE *file);
    bool saveToFile(FILE *file);

private:
    std::vector<SettingsEntry> settings;
    static constexpr int background_width = SCREEN_WIDTH - 50, background_height = SCREEN_HEIGHT - 50;
    TEXTURE *background;
    unsigned int current_selection = 0;
    bool changed_something;
};

extern SettingsTask settings_task;

#endif // SETTINGSTASK_H
