#include "Protagonist.h"
#include <cmath>//pow, atan2, sqrt
#include "widget.h"/////////////////?Отрисовка
#include <QDebug>//Временно

Protagonist::Protagonist()
{

}

float Protagonist::getCurrentX(){
    return currentX;
}
float Protagonist::getCurrentY(){
    return currentY;
}
float Protagonist::getCurrentSpeed(){
    return currentSpeed;
}
float Protagonist::getMaxSpeed(){
    return maxSpeed;
}

void Protagonist::updateMapPosition(float dt, bool isPressed, float target_x, float target_y) {
    //Вместо isDoubleTapped используется isPressed, а isPressed не используется вовсе
    //То есть всегда с ускорением
    orientationAim = atan2(target_x - currentY, target_y - currentX);
    qDebug()<<orientationAim * 180 / M_PI;
}

void Protagonist::draw(Widget& widget){
    widget.setColor(1,0,0,1);
    widget.drawPentagon(currentX,currentY,50,orientationAim * 180 / M_PI-90);
}
