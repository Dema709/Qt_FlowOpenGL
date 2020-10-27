#ifndef FOOD_H
#define FOOD_H

class Widget;

class Food
{
public:
    Food();
    Food(int testType);//Для тестового отображения
    void updateMapPosition(float dt);
    void updateMapPositionTest(float dt);//Для тестового отображения, без перемещения
    void draw(Widget& widget);
    bool isEaten();
    float getCurrentX();
    float getCurrentY();
    float getCurrentRadius();
    void setEaten();
private:
    float currentX,currentY;//Положение на карте
    float orientation;//Ориентация, в радианах
    float orientationAim;
    float currentSpeed, maxSpeed;//В пикселях в секунду
    float turnSpeed;//В радианах в секунду
    float aimX,aimY;
    float canvasSize;
    float canvasSnake;
    int type;
    float currentRadius;
    bool isEaten_;
    bool isInvisible=false;
    float invisibleTimer;
    float flockieBirdRadius=50;
    float multiplSize, multiplSnake;

    float maxRange=1000;

    void goToRandomLocation();
    void setRadius();
};

#endif // FOOD_H
