#ifndef SNAKEHUNTER_H
#define SNAKEHUNTER_H

#include <vector>
#include "Segment.h"
#include "widget.h"/////////////////?Отрисовка

class Widget;

class SnakeHunter
{
public:
    SnakeHunter(int Nsegm_, int NsegmEvolved_);
    void updateMapPosition(float dt);
    void draw(Widget& widget);
    bool isEaten();
    int getNsegm();
    bool isSegmentWeakPointAndUndamaged(int nSegm);
    float getCurrentSegX(int nSegm);
    float getCurrentSegY(int nSegm);
    float getCurrentSegRadius(int nSegm);
    void setDamaged(int nSegm);
private:
    float currentX,currentY;//Положение на карте
    float orientation;//Ориентация, в радианах
    float orientationAim = 0;
    float currentSpeed, maxSpeed = 120, maxBoostSpeed = 250;//В пикселях в секунду
    float maxTurnSpeed;//В радианах в секунду
    float canvasSize = 0;
    float canvasSnake = 0;
    float canvasEat = 0;//Для отображения анимации поедания

    std::vector<Segment> segments;
    int Nsegm;

    bool isEatingRightNow = false;

    int NsegmMax=16;

    bool itWasVoidFood=false;

    bool isPanic=false;
    float panicTimer=0, panicMaxTime=2;

    bool hasTarget=false;
    bool isEaten_=false;
    float aimX, aimY;

    float agroRadius=200;

    bool isInDivision=false;
    float divisionTimer=0;
    bool isDivisionWrittenInFood=false;

    float maxRange=1000;

    float getOrientationInDegrees();
    void goToRandomLocation();
    void evolveLittle();
    void evolveBig();
};

#endif // SNAKEHUNTER_H
