#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
class Widget;
class Protagonist;
class ChangeLevelFood;
class Food;
class SnakeHunter;

class Level
{
public:
    Level(int levelNum);
    std::vector<float> getColor();
    int updateFoodMapPosition(float dt, Protagonist& protagonist, float cameraX, float cameraY, float half_widht, float half_height);
    void draw(Widget& widget);
private:
    std::vector<float> backgroundColor;
    std::vector<ChangeLevelFood> changeLevelFood_array;
    std::vector<Food> food_array;
    std::vector<SnakeHunter> snakeHunter_array;
};

#endif // LEVEL_H
