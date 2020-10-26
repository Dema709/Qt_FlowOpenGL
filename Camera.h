#ifndef CAMERA_H
#define CAMERA_H

//#include "Protagonist.h"
class Protagonist;

namespace ChakaPon {//Для исключения конфликтов с уже существующим классом

class Camera
{
public:
    //Camera();
    void setScreen(float screenWidth, float screenHeight/*, float gamefieldHalfX_, float gamefieldHalfY_*/);
    float getCurrentX();
    float getCurrentY();
    //float getGamefieldHalfX();
    //float getGamefieldHalfY();
    void updateMovement(float dt, Protagonist protagonist);
private:
    float currentX = 0, currentY = 0;//Положение на карте
    float minSpeedMultiplier = 0.2f;
    float minSpeed = 40, currentSpeed;
    float targetX, targetY;
    //float screenSizeX, screenSizeY;
    float maxDistanceX = 200, maxDistanceY = 200;//Максимальное отдаление камеры, инициализация ничего не даёт
    //float gamefieldHalfX, gamefieldHalfY;
};

}
#endif // CAMERA_H
