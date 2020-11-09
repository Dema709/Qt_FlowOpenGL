#ifndef FOOD_H
#define FOOD_H

class Widget;
class Protagonist;

class Food
{
public:
    Food();
    Food(float curX_, float curY_, int birdType);//Для стайки
    Food(int testType);//Для тестового отображения [0+] и для босса (вместо Food(String s)) [100+]

    void updateMapPosition(float dt);
    void updateMapPositionTest(float dt);//Для тестового отображения, без перемещения
    void updateMapPositionAngryBoss(float dt, Protagonist& protagonist);
    void draw(Widget& widget);
    void drawAngryBossFood(Widget& widget);

    bool isEaten();
    float getCurrentX();
    float getCurrentY();
    float getCurrentRadius();
    void setEaten();
    bool isEatenAndNotInvisible();
    void setInvisible(float timer,float segCurrentX, float segCurrentY);

    void setOrientationAim(float orientationAim_);
    void updateMapPositionBird(float dt, bool isFlockieBirdInPanic,float curFlockieBirdX, float curFlockieBirdY);

    void setInvisibleAngryBossfood(float timer,float segCurrentX, float segCurrentY, float currentSpeed, float orientation);//boss
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
    void updateCanvasSizeAndSnake(float dt);
};

#endif // FOOD_H
