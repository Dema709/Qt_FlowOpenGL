#ifndef PROTAGONIST_H
#define PROTAGONIST_H

class Widget;

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
private:
    float currentX = 0*200, currentY = 0;//Положение на карте
    float orientation = 0;//Ориентация, в радианах
    float orientationAim = 0;
    float currentSpeed = 0, maxSpeed = 200, maxBoostSpeed = 700;//В пикселях в секунду
    float turnSpeed = 240. / 180 * 3.1415;//M_PI;//В радианах в секунду//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //float touchX = 0, touchY = 0;
    float screenSizeX, screenSizeY;
    float canvasSize = 0;
    float canvasSnake = 0;
};

#endif // PROTAGONIST_H
