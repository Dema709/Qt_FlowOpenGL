#ifndef CHANGELEVELFOOD_H
#define CHANGELEVELFOOD_H

//#include <cmath>//M_PI
class Widget;

class ChangeLevelFood
{
public:
    ChangeLevelFood(int type_);
    void updateMapPosition(float dt, float cameraX, float cameraY, float half_widht, float half_height);
    float getCurrentX();
    float getCurrentY();
    float getCurrentRadius();
    int getType();
    void draw(Widget& widget);
private:
    float currentX, currentY;//Положение на карте
    float orientation;//Ориентация, в радианах
    float orientationAim;
    float currentSpeed = 100;
    float turnSpeed = 20 / 180. * 3.1415;//В радианах в секунду
    float aimX, aimY;
    float canvasSize, canvasSnake;
    float multiplSize, multiplSize2;
    int type;
    float currentRadius = 20;
    float lastPingTime = 0;
    float pingPeriod = 2, pingDuration = 1;//Время неактивности (?) и активности пинга
    bool shouldIPing = false;
    //float pingSideCriticalAngle;//Для определения, пинг на какой из стенок
    //float alpha, deltaX, deltaY;
    float pingX, pingY;//Центр пинга, имеет смысл только при shouldIPing

    float maxRange=1000;
    //float screenSizeX, screenSizeY;

    void goToRandomLocation();
};

#endif // CHANGELEVELFOOD_H
