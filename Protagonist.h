#ifndef PROTAGONIST_H
#define PROTAGONIST_H

#include <vector>

class Segment;
class Food;
class ChangeLevelFood;
class SnakeHunter;
class SharkHunter;
class FlockieBird;

class Widget;
//class Segment;

class Protagonist
{
public:
    Protagonist();
    float getCurrentX();
    float getCurrentY();
    float getCurrentSpeed();
    float getMaxSpeed();
    void updateMapPosition(float dt, bool isPressed, float target_x, float target_y);
    void draw(Widget& widget);
    float getOrientationInDegrees();
    int updateEat(std::vector<ChangeLevelFood>& changeLevel, std::vector<Food>& food, std::vector<SnakeHunter>& snakeHunter_array,
                  std::vector<SharkHunter>& sharkHunter_array, std::vector<FlockieBird>& flockieBird_array);

    int getNsegm();
    bool isSegmentWeakPointAndUndamaged(int nSegm);
    float getCurrentSegX(int nSegm);
    float getCurrentSegY(int nSegm);
    float getCurrentSegRadius(int nSegm);
    void setDamaged(int nSegm);
private:
    float currentX = 200, currentY = 0;//Положение на карте
    float orientation = 90 * 3.1415 / 180;//Ориентация, в радианах
    float orientationAim = 0;
    float currentSpeed = 0, maxSpeed = 200, maxBoostSpeed = 700;//В пикселях в секунду
    float turnSpeed = 240. / 180 * 3.1415;//M_PI;//В радианах в секунду//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //float touchX = 0, touchY = 0;
    float screenSizeX, screenSizeY;
    float canvasSize = 0;
    float canvasSnake = 0;
    float canvasEat = 0;
    bool isEatingRightNow = false;
    bool itWasVoidFood = false;
    bool levelDownCosDamaged = false;


    std::vector<Segment> segments;
    int Nsegm;int NsegmMax=8;

    void evolveLittle();
    void evolveBig();
};

#endif // PROTAGONIST_H
