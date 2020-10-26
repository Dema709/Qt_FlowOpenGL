#include "Protagonist.h"
#include <cmath>//pow, atan2, sqrt
#include "widget.h"/////////////////?Отрисовка
#include <QDebug>//Временно
#include <algorithm>//max

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
    //target_x = -400; target_y = 0;

    //Вместо isDoubleTapped используется isPressed, а isPressed не используется вовсе
    //То есть всегда с ускорением
    orientationAim = atan2(target_y - currentY, target_x - currentX);// orientationAim *= -1;
    float orientationDelta = remainder(orientationAim - orientation, M_PI * 2);
    qDebug()<<"";
    qDebug()<<(int) target_x<<(int) target_y<<(int) currentX<<(int) currentY;
    qDebug()<<"orientationAim"<<orientationAim * 180 / M_PI;
    qDebug()<<"orientation"<<orientation * 180 / M_PI;
    qDebug()<<"orientationDelta"<<orientationDelta * 180 / M_PI;
    if (abs(orientationDelta) > turnSpeed * dt) {//Если изменение угла не слишком маленькое
        if (orientationDelta>0){
        //if ((orientationDelta <= -M_PI) || ((orientationDelta > 0) && (orientationDelta <= M_PI))) {
            orientation += turnSpeed * dt;
        } else {
            orientation -= turnSpeed * dt;
        }
        //orientation = remainderf(orientation, M_PI * 2);//Для избавления от накопления
    } else {
        orientation = orientationAim;
    }

    if (isPressed) {
        currentSpeed = std::min(maxBoostSpeed, currentSpeed + dt * 400);//400 - прирост скорости?
    } else {
        if (currentSpeed > maxSpeed) {//Теряем скорость после ускорения
            currentSpeed = std::max(currentSpeed * (float) pow(0.98, dt * 30), maxSpeed);//FPS=30
        } else {
            currentSpeed = std::min(maxSpeed, currentSpeed + dt * 400 * 0.5f);
        }
    }

    //orientation = 1 * M_PI / 2;// M_PI * 180.;
    //currentSpeed = 10;//maxSpeed;

    //qDebug()<<cos(0)<<cos(45)<<cos(45./180*M_PI);
    //Позиция
    currentX += currentSpeed * cos(orientation) * dt;
    currentY += currentSpeed * sin(orientation) * dt;

    canvasSize += dt*0.8f; while (canvasSize>1) canvasSize--;
    canvasSnake += dt*currentSpeed/maxSpeed; while (canvasSnake>1) canvasSnake--;
/*
    if (isEatingRightNow){
        canvasEat=(canvasEat+dt*0.8f*2);
        if (canvasEat>=2) {
            canvasEat = 0;
            if (itWasVoidFood) {//Не насыщать клетки, если перешёл на другой уровень
                itWasVoidFood=false;
            }
            else{
                this.evolveLittle();
            }
            isEatingRightNow = false;
        }
    }

    for (int k=0;k<Nsegm;k++){
        if (k==0) {
            segments[k].updateMapPosition(currentX, currentY, dt, currentSpeed);
        }
        else {
            segments[k].updateMapPosition(segments[k-1].getCurrentX(), segments[k-1].getCurrentY(), dt, currentSpeed);
        }
    }*/
}

void Protagonist::draw(Widget& widget){
    float delta = 0;
    //widget.setColor(1,0,0,1);
    //widget.drawPentagon(currentX,currentY,50,orientation * 180 / M_PI - delta);
    widget.setColor(0,1,0,1);
    widget.drawTriangle(currentX,currentY,50,orientation * 180 / M_PI - delta);
    widget.drawRing2Transfered(currentX,currentY,10,orientation * 180 / M_PI - delta, -50, 0);
}
