#ifndef SHARKHUNTER_H
#define SHARKHUNTER_H

#include <vector>
#include "Segment.h"

class Widget;

class SharkHunter
{
public:
    SharkHunter();
    void updateMapPosition(float dt);
    void draw(Widget& widget);
private:
    float currentX, currentY;//Положение на карте
    float orientation;//Ориентация, в радианах
    float orientationAim = 0;//Перенести в .cpp файл
    float currentSpeed, maxSpeed, maxBoostSpeed;//В пикселях в секунду
    float turnSpeed;//,maxTurnSpeed;//В радианах в секунду

    float canvasSize;
    float canvasSnake;

    std::vector<Segment> segments;
    int Nsegm;

    bool isEatingRightNow = false;;
    float canvasEat;//Для отображения анимации поедания

    bool isPanic = false;
    float panicTimer = 0, panicMaxTime = 2;

    bool hasTarget = false; bool hasPlayerInTarget=false;
    bool isEaten_ = false;
    float aimX, aimY;


    bool isInDivision = false;
    float divisionTimer = 0;
    bool isDivisionWrittenInFood = false;

    float maxRange=1000;

    float scaleForLittleOrBigFish = 0.1f;//Убрать полностью (?)

    float agroTimer = 8, agroMaxTimeWithoutAim = 2, agroAngle = (float) (60. / 180. * 3.1415), agroRadius, agroQuietRadius;// = 600;//Угол в каждую сторону
    bool isAgro = false;//, shouldILoseAgro = false;
    bool evolvedAlready=false;

    void goToRandomLocation();
    void evolve(bool firstTimeCall);
    float getOrientationInDegrees();
};

#endif // SHARKHUNTER_H
