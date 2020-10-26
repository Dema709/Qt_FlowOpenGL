#include "Camera.h"
#include <cmath>//pow
#include <algorithm>//max

#include <QDebug>//Временно
//Camera::Camera(){}
#include "Protagonist.h"

namespace ChakaPon {

float Camera::getCurrentX(){
    return currentX;
}
float Camera::getCurrentY(){
    return currentY;
}
void Camera::setScreen(float half_widht, float half_height/*, float gamefieldHalfX_, float gamefieldHalfY_*/){
    //screenSizeX=screenWidth; screenSizeY=screenHeight;
    /*float mult=1.4f;//Множитель на границу игровой зоны (для отображения)
    gamefieldHalfX=gamefieldHalfX_*mult;
    gamefieldHalfY=gamefieldHalfY_*mult;*/
    maxDistanceX=half_widht*0.14f*2;
    maxDistanceY=half_height*0.14f*2;
}

void Camera::updateMovement(float dt, Protagonist protagonist){
    //currentX += dt * 100;
    currentSpeed=protagonist.getCurrentSpeed();
    targetX=protagonist.getCurrentX();
    targetY=protagonist.getCurrentY();

    //Если расстояние от центра выше чем допустимое
    float distanceEllips=(float)sqrt(pow((targetX-currentX)/maxDistanceX,2)+pow((targetY-currentY)/maxDistanceY,2));
    float distance=(float)sqrt((targetX-currentX)*(targetX-currentX)+(targetY-currentY)*(targetY-currentY));
    float angle=(float) atan2(targetY - currentY, targetX - currentX);

   // qDebug()<<distanceEllips<<distance<<angle;

    if (distance>minSpeedMultiplier*protagonist.getMaxSpeed()*dt*1.5) {//if Для исключения дёргания на месте
        currentSpeed = std::max(currentSpeed * 1.0f * distanceEllips, minSpeed);///Чуток другой вариант приближения камеры, побыстрее
        //currentSpeed = Math.max(currentSpeed * 1.0f * distanceEllips, minSpeed*distanceEllips);
        currentX = currentX + currentSpeed * (float) cos(angle) * dt;
        currentY = currentY + currentSpeed * (float) sin(angle) * dt;
    }
    qDebug()<<"CameraC:"<<currentX<<currentY;
    qDebug()<<"CameraT:"<<targetX<<targetY;
}

}
