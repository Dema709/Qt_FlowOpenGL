#ifndef LEVEL_H
#define LEVEL_H

#include <vector>


class Level
{
public:
    Level(int levelNum);
    std::vector<float> getColor();
private:
    std::vector<float> backgroundColor;
};

#endif // LEVEL_H
