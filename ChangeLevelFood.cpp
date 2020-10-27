#include "ChangeLevelFood.h"
#include "random.hpp"
#include "widget.h"/////////////////?Отрисовка
#include <QDebug>

ChangeLevelFood::ChangeLevelFood(int type_)
{
    currentX = effolkronium::random_static::get<float>(-maxRange, maxRange);
    currentY = effolkronium::random_static::get<float>(-maxRange, maxRange);
    this->goToRandomLocation();
    orientation = effolkronium::random_static::get<float>(0, 2*M_PI);

    canvasSize = effolkronium::random_static::get<float>(0, 1);
    canvasSnake = effolkronium::random_static::get<float>(0, 1);

    type=type_;
}

void ChangeLevelFood::goToRandomLocation(){
    aimX = effolkronium::random_static::get<float>(-maxRange, maxRange);
    aimY = effolkronium::random_static::get<float>(-maxRange, maxRange);
}

void ChangeLevelFood::updateMapPosition(float dt){
    lastPingTime=lastPingTime+dt;
    if (sqrt(pow((currentX - aimX), 2) + pow((currentX - aimX), 2)) < 5) {
        goToRandomLocation();
    }

    orientationAim = (float) atan2(aimY - currentY, aimX - currentX);

    float orientationDelta = remainderf(orientationAim - orientation, M_PI * 2);

    if (abs(orientationDelta) > turnSpeed * dt) {//Если изменение угла не слишком маленькое
        if (orientationDelta>0){
            orientation += turnSpeed * dt;
        } else {
            orientation -= turnSpeed * dt;
        }
    } else {
        orientation = orientationAim;
    }

    //Позиция
    currentX += currentSpeed * cos(orientation) * dt;
    currentY += currentSpeed * sin(orientation) * dt;

    //Обновление для канвы - изменение размера
    switch (type) {
    case 0://Переход вниз
        canvasSize = fmodf(canvasSize + dt * 0.8f, 1);
        canvasSnake = fmodf(canvasSnake + dt * currentSpeed / 100., 1);
        multiplSize=abs(canvasSize-0.5f)*2*0.3f+0.7f;
        multiplSize2=(abs(canvasSize-0.5f)*(-2)*0.3f+0.7f)*40;//вместо мулт сайз 2
        break;
    case 1://Переход наверх
        canvasSize = fmodf(canvasSize + dt * 0.8f, 1);
        canvasSnake = fmodf(canvasSnake + dt * currentSpeed / 100., 1);
        multiplSize=abs(canvasSize-0.5f)*2*0.3f+0.7f;
        multiplSize2=(abs(canvasSize-0.5f)*(-2)*0.3f+0.7f)*38;//35;//вместо мулт сайз 2
        break;
    default:
        qDebug()<<"ChangeLevelFood::updateMapPosition"<<"Тип еды задан неправильно. Обновление размеров"<<type;
        break;
    }
}

float ChangeLevelFood::getCurrentX(){
    return currentX;
}

float ChangeLevelFood::getCurrentY(){
    return currentY;
}

float ChangeLevelFood::getCurrentRadius(){
    return currentRadius;
}

int ChangeLevelFood::getType(){
    if (type==0) return 1;
    else         return -1;
}

void ChangeLevelFood::draw(Widget& widget){
    //qDebug()<<"ChangeLevelFood::draw";
    widget.setColor(1, 1, 1, 0.47);//Standart white
}
