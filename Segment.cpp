#include "Segment.h"
#include "random.hpp"
#include <QDebug>
#include "widget.h"/////////////////?Отрисовка

Segment::Segment(float headCurrentX, float headCurrentY, float headOrientation, int type_)
{
    canvasSnake = effolkronium::random_static::get<float>(0, 1);//(float)Math.random();
    orientation = headOrientation;
    type = type_;
    this->setDistance();
    currentX = headCurrentX + distance * (float) cos(orientation+M_PI);// * dt;
    currentY = headCurrentY + distance * (float) sin(orientation+M_PI);// * dt;
    saturation=false;
}

void Segment::setDistance(){
    switch (type){
    case 0:
        currentRadius=8.4f;
        distance=33*0.8f;
        break;
    case 1:
        distance=21;
        break;
    case 2:
        currentRadius=8.4f;
        distance=33*0.8f;
        break;
    default:
        qDebug()<<"Segment::setDistance"<<"Неправильный тип"<<type;
        break;
    }
}

Segment::Segment(){
    qDebug()<<"Segment::Segment() constructor!";
}

void Segment::updateMapPosition(float headCurrentX, float headCurrentY, float dt, float headCurrentSpeed){
    canvasSnake = fmodf(canvasSnake + dt * headCurrentSpeed / 160., 1);
    orientation = atan2(currentY - headCurrentY, currentX - headCurrentX);
    currentX = headCurrentX + distance * cos(orientation);
    currentY = headCurrentY + distance * sin(orientation);
}

float Segment::getOrientationInDegrees(){
    return orientation * 180 / M_PI;
}

void Segment::drawWithScale(Widget& widget, float segNum, float NsegmMax){
    switch (type){
    case 0://Сегменты
        widget.drawSquareTransfered(currentX,currentY,3.36f,this->getOrientationInDegrees(),30*0.7f,0);
        if (isWeakPoint_){
            if (isWeakPointDamaged_)
                widget.setColor(1, 1, 1, 0.23);//Ghostly white
            widget.drawRing2(currentX, currentY,12*0.7f);
            if (saturation){
                //saturated
                widget.drawRound(currentX, currentY,5.5f*0.7f*(1+0.3773584905660377f/2));
            }
            else{
                //notSaturated
                widget.drawRing2(currentX, currentY,5.5f*0.7f);
            }
            if (isWeakPointDamaged_)
                widget.setColor(1, 1, 1, 0.47);//Вернуть цвет обратно на стандартный белый
        }
        else{
            widget.drawRing(currentX, currentY,12*0.7f);
            if (saturation){
                //saturated
                widget.drawRound(currentX, currentY,5.5f*0.7f*(1+0.3773584905660377f/2));
            }
            else{
                //notSaturated
                widget.drawRing2(currentX, currentY,5.5f*0.7f);
            }
        }
        break;
    case 1://Хвостик
        widget.drawRing2(currentX, currentY,5*0.7f);
        widget.drawSquareTransfered(currentX,currentY,3.36f,this->getOrientationInDegrees(),15*0.7f,0);
        break;
    case 2://Лапки
        widget.drawSquareTransfered(currentX,currentY,3.36f,this->getOrientationInDegrees(),30*0.7f,0);
        if (isWeakPoint_){
            if (isWeakPointDamaged_)
                widget.setColor(1, 1, 1, 0.23);//Ghostly white
            widget.drawRing2(currentX, currentY,12*0.7f);
            if (saturation){
                //saturated
                widget.drawRound(currentX, currentY,5.5f*0.7f*(1+0.3773584905660377f/2));
            }
            else{
                //notSaturated
                widget.drawRing2(currentX, currentY,5.5f*0.7f);
            }
            if (isWeakPointDamaged_)
                widget.setColor(1, 1, 1, 0.47);//Вернуть цвет обратно на стандартный белый
        }
        else{
            widget.drawRing(currentX, currentY,12*0.7f);
            if (saturation){
                //saturated
                widget.drawRound(currentX, currentY,5.5f*0.7f*(1+0.3773584905660377f/2));
            }
            else{
                //notSaturated
                widget.drawRing2(currentX, currentY,5.5f*0.7f);
            }
        }
        widget.drawSquaresTransferedWithScale(currentX,currentY,this->getOrientationInDegrees(),(abs(canvasSnake-0.5f)*2*0.3f*2+0.7f)*30*2,NsegmMax/9.*150*(float)(pow(1/(segNum+5),1)));
        break;
    /*case 3://Сегменты
        widget.drawRing2(currentX, currentY,12*0.7f);
        widget.drawLowpolyRound(currentX, currentY,5.5f*0.7f);

        break;*/
    default:
        qDebug()<<"Segment::drawWithScale"<<"Тип еды задан неправильно. Обновление картинки drawWithScale";
        break;
    }
}

void Segment::setFirst(){
    isFirst=true;
    distance=33;
}

void Segment::setWeakPoint(){
    isWeakPoint_ = true;
}

float Segment::getCurrentX(){
    return currentX;
}

float Segment::getCurrentY(){
    return currentY;
}

float Segment::getOrientation(){
    return orientation;
}

bool Segment::isWeakPoint(){
    return isWeakPoint_;
}

bool Segment::isWeakPointDamaged(){
    return isWeakPointDamaged_;
}

void Segment::restoreWeakPoint(){
    isWeakPointDamaged_ = false;
}

bool Segment::getSaturation(){
    return saturation;
}

void Segment::setSaturation(bool saturation_){
    saturation=saturation_;
}

int Segment::getType() {
    return type;
}

void Segment::changeType(int type_){
    type=type_;
    if (!isFirst)
    {
        this->setDistance();
    }
}

void Segment::drawDivision(Widget& widget, float dt){
    if (dt>0 && dt<1) {
        widget.setColor(1, 1, 1, (1 - dt)*0.47);//AlphaWhite((1 - (dt) / 1f)*0.47f);
        widget.drawRing(currentX,currentY,howMuchIsTheFish*dt*25);
        //widget.setColor(0);
    }
}

bool Segment::isSegmentWeakPointAndUndamaged(){
    return isWeakPoint_&&!isWeakPointDamaged_;
}

float Segment::getCurrentRadius(){return currentRadius;}

void Segment::setWeakPointDamaged(){
    isWeakPointDamaged_=true;
    saturation=false;
}

bool Segment::hasSaturationOrIsWeakPoint(){
    return saturation||isWeakPoint_;
}

Segment::Segment(int type_, float howMuchIsTheFish_){
    type = type_;
    /////////////////////////////////////////////////////////////////////////////////добавить ещё curx, cury
            howMuchIsTheFish=howMuchIsTheFish_;
            saturation=false;
            if (type==0) {
                //currentRadius=8.4f;

                //saturation=true;
                //distance=33;
            }//Центральные три точки. Стандартные
            if (type==3){
                saturation=true;
                currentRadius=16.8f;
                isWeakPoint_=true;
                //distance=21;
            }//Глаза. Кружок с ободом

            currentRadius=currentRadius*howMuchIsTheFish;///////////////////////////////////////////////////////////////////////////////////
}//Сегмент акулы

void Segment::updateMapPosition(float currentX_, float currentY_, float orientation_){
    orientation = orientation_;
    currentX = currentX_;
    currentY = currentY_;
}//Сегмент акулы
