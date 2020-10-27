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
float Protagonist::getOrientationInDegrees(){
    return orientation * 180 / M_PI;
}
void Protagonist::updateMapPosition(float dt, bool isPressed, float target_x, float target_y) {
    //Вместо isDoubleTapped используется isPressed, а isPressed не используется вовсе
    //То есть всегда с ускорением
    orientationAim = atan2(target_y - currentY, target_x - currentX);// orientationAim *= -1;
    float orientationDelta = remainderf(orientationAim - orientation, M_PI * 2);
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

    /*  //Рабочий предыдущий вариант
    if (isPressed) {
        currentSpeed = std::min(maxBoostSpeed, currentSpeed + dt * 400);//400 - прирост скорости?
    } else {
        if (currentSpeed > maxSpeed) {//Теряем скорость после ускорения
            currentSpeed = std::max(currentSpeed * (float) pow(0.98, dt * 30), maxSpeed);//FPS=30
        } else {
            currentSpeed = std::min(maxSpeed, currentSpeed + dt * 400 * 0.5f);
        }
    }*/

    //Slowing-down protagonist when mouse is near
    if (isPressed) {
        currentSpeed = std::min(maxBoostSpeed, currentSpeed + dt * 400);//400 - прирост скорости?
    } else {
        float distance = sqrt(pow(target_y - currentY,2) + pow(target_x - currentX,2));
        float control_distance = 100;
        if (distance >= control_distance){
            if (currentSpeed > maxSpeed) {//Теряем скорость после ускорения
                currentSpeed = std::max(currentSpeed * (float) pow(0.98, dt * 30), maxSpeed);//FPS=30
            } else {
                currentSpeed = std::min(maxSpeed, currentSpeed + dt * 400 * 0.5f);
            }
        } else {
            //Замедление, когда близко
            float max_cur_speed = maxSpeed * distance / control_distance;//Да, вот такая кривотень с названием)
            if (currentSpeed > max_cur_speed) {//Теряем скорость после ускорения
                currentSpeed = std::max(currentSpeed * (float) pow(0.98, dt * 30), max_cur_speed);//FPS=30
            } else {
                currentSpeed = std::min(max_cur_speed, currentSpeed + dt * 400 * 0.5f);
            }
        }
        qDebug()<<(int)distance<<(int)currentSpeed;
    }


    /*if (distance < 50){
        currentSpeed *= pow(0.98, dt * 30);
    }*/
    //qDebug()<<(int)distance<<(int)currentSpeed;

    //Позиция
    currentX += currentSpeed * cos(orientation) * dt;
    currentY += currentSpeed * sin(orientation) * dt;

    canvasSize += dt*0.8f; canvasSize = fmodf(canvasSize, 1);//while (canvasSize>1) canvasSize--;
    canvasSnake += dt*currentSpeed/maxSpeed; canvasSnake = fmodf(canvasSnake, 1);//while (canvasSnake>1) canvasSnake--;
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

    //canvasEat += dt*1.6f;
    //if (canvasEat>=2) canvasEat -= 2;
}

void Protagonist::draw(Widget& widget){
    widget.setColor(1, 1, 1, 0.47);//Standart white

    widget.drawRing2(currentX,currentY,2*0.7f*5.3f);
    widget.drawSquareTransfered(currentX,currentY,3.36f,this->getOrientationInDegrees(),-21,0);

    if (canvasEat>=1){
        widget.drawMouth(currentX,currentY, this->getOrientationInDegrees(),1-abs(canvasEat-1-0.5f)*2);
    } else {
        widget.drawMouth(currentX,currentY, this->getOrientationInDegrees(),1-abs(canvasEat-0.5f)*2);
    }

}
