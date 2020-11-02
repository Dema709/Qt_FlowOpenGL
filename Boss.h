#ifndef BOSS_H
#define BOSS_H


#include "Segment.h"
#include <vector>

class Widget;
class Food;
class Protagonist;

class Boss
{
public:
    Boss(int bossType_);
    void updateMapPosition(float dt, Protagonist&);
    void draw(Widget& widget);
    void findNearFood(std::vector<Food>&, Protagonist&);

    bool isEaten();
    int getNsegm();//Здесь - вернуть количество съедобных сегментов (съедобны не все)
    bool isSegmentWeakPointAndUndamaged(int nSegm);
    float getCurrentSegX(int nSegm);
    float getCurrentSegY(int nSegm);
    float getCurrentSegRadius(int nSegm);
    void setDamaged(int nSegm);
private:
    int bossType;//Пока нигде не используется

    float currentX, currentY;//Положение на карте
    float orientation;//Ориентация, в радианах
    float orientationAim;
    float currentSpeed, maxSpeed, maxBoostSpeed;//В пикселях в секунду
    float turnSpeed;//,maxTurnSpeed;//В радианах в секунду

    float canvasSize;
    float canvasSnake;

    std::vector<Segment> segments;//Segment[] segments;
    int Nsegm;

    bool isEatingRightNow = true;//Чтобы не съел сразу!
    float canvasEat;//Для отображения анимации поедания
    float canvasSegmentMovement;

    bool isPanic = false;
    float panicTimer = 0, panicMaxTime = 2;

    bool hasTarget = false;
    bool hasPlayerInTarget=false;
    bool isEaten_ = false;
    float aimX, aimY;


    bool isInDivision = false;
    float divisionTimer = 0;
    bool isDivisionWrittenInFood = false;


    float agroTimer = 8, agroMaxTimeWithoutAim = 2, agroAngle = (float) (60. / 180. * 3.1415), agroRadius, agroQuietRadius;// = 600;//Угол в каждую сторону
    bool isAgro = false;

    std::vector<Food> angryFood;

    float maxRange=1000;

    void goToRandomLocation();
    float getOrientationInDegrees();
};

#endif // BOSS_H
