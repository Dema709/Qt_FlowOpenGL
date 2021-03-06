#include "Protagonist.h"
#include "Segment.h"
#include "Food.h"
#include "ChangeLevelFood.h"
#include "SnakeHunter.h"
#include <cmath>//pow, atan2, sqrt
#include "widget.h"/////////////////?Отрисовка
#include <QDebug>//Временно
#include <algorithm>//max
#include "SharkHunter.h"
#include "FlockieBird.h"
#include "Boss.h"

Protagonist::Protagonist()
{
    Nsegm = 2;
    segments.reserve(NsegmMax);//segments = new Segment[NsegmMax];
        /*for (int k = 0; k < Nsegm; k++) {
            segments[k] = new Segment(currentX, currentY, orientation, k);
        }*/
    segments.push_back(Segment(currentX, currentY, orientation, 0));//segments[0] = Segment(currentX, currentY, orientation, 0);
    segments[0].setFirst();
    segments[0].setWeakPoint();
    //segments[1] = new Segment(segments[0].getCurrentX(), segments[0].getCurrentY(), segments[0].getOrientation(), 1);
    segments.push_back(Segment(segments[0].getCurrentX(), segments[0].getCurrentY(), segments[0].getOrientation(), 1));
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

    //Если нажата клавиша маши и гг рядом с мышкой, он замедляется
    if (isPressed) {
        currentSpeed = std::min(maxBoostSpeed, currentSpeed + dt * 400);//400 - прирост скорости?
    } else {
        float distance = sqrt(pow(target_y - currentY,2) + pow(target_x - currentX,2));
        float control_distance = 100;
        float max_cur_speed;//Да, вот такая кривотень с названием) Это временно, надеюсь
        if (distance < control_distance){
            max_cur_speed = maxSpeed * distance / control_distance;
        } else {
            max_cur_speed = maxSpeed;//Замедление, когда близко
        }
        if (currentSpeed > max_cur_speed) {//Теряем скорость после ускорения
            currentSpeed = std::max(currentSpeed * (float) pow(0.98, dt * 30), max_cur_speed);//FPS=30
        } else {
            currentSpeed = std::min(max_cur_speed, currentSpeed + dt * 400 * 0.5f);
        }
        //qDebug()<<(int)distance<<(int)currentSpeed;
    }

    //Позиция
    currentX += currentSpeed * cos(orientation) * dt;
    currentY += currentSpeed * sin(orientation) * dt;

    canvasSize += dt*0.8f; canvasSize = fmodf(canvasSize, 1);//while (canvasSize>1) canvasSize--;
    canvasSnake += dt*currentSpeed/maxSpeed; canvasSnake = fmodf(canvasSnake, 1);//while (canvasSnake>1) canvasSnake--;

    if (isEatingRightNow){
        canvasEat=(canvasEat+dt*0.8f*2);
        if (canvasEat>=2) {
            canvasEat = 0;
            if (itWasVoidFood) {//Не насыщать клетки, если перешёл на другой уровень
                itWasVoidFood=false;
            }
            else{
                this->evolveLittle();//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Реализовать
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
    }

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

    for (int k=0;k<Nsegm;k++){
        segments[k].drawWithScale(widget,k,Nsegm);
    }
}

void Protagonist::evolveLittle(){
    //Нажрать кружочки
    //Попробовать восстановить weakPoint
    //Если удалось - return
    int kT = 0;
    while (kT < Nsegm) {
        if (segments[kT].isWeakPointDamaged()) {//сработает только на weakPoint
            segments[kT].restoreWeakPoint();
            return;//break;
        }//Восстановление weakPoint
        kT++;
    }
    ////////////////////////////////////////////////Можно сделать проще - как выше
    int saturationSum = 0;
    for (int k = 0; k < Nsegm - 1; k++) {
        if (segments[k].getSaturation())
            saturationSum++;
    }
    if (saturationSum == Nsegm - 1) {
        this->evolveBig();
    } else {
        //Ищи наименьшую пустую и наполняй
        int k = 0;
        while (k < Nsegm - 1) {
            if (!segments[k].getSaturation()) {
                segments[k].setSaturation(true);
                break;
            }
            k++;
        }
    }
}

void Protagonist::evolveBig(){

    std::vector<int> k_types(3);
    //int [] k_types;
    //k_types=new int[3];//Сколько сегментов какого типа, кроме хвостика(?)
    int k2=0;//Счётчик перебора
    while (k2<Nsegm-1){
        k_types[segments[k2].getType()]++;
        k2++;
    }


    if (Nsegm<NsegmMax){

        if (Nsegm-k_types[2]>3){
            //добавить лапку
            int k0=0;
            while (k0<Nsegm-1){
                if (segments[k0].getType()==0){
                    //segments[k0].setSaturation(false);
                    segments[k0].changeType(2);//
                    break;
                }
                k0++;
            }

            for (int k = 0; k < Nsegm - 1; k++) {//Убрать насыщение
                //segments[k].changeType(0);//
                segments[k].setSaturation(false);
            }
        }
        else {

            Nsegm++;
            //segments[Nsegm - 1] = new Segment(segments[Nsegm - 2].getCurrentX(), segments[Nsegm - 2].getCurrentY(), segments[Nsegm - 2].getOrientation(), 1);//Хвост
            segments.push_back(Segment(segments[Nsegm - 2].getCurrentX(), segments[Nsegm - 2].getCurrentY(), segments[Nsegm - 2].getOrientation(), 1));
            segments[Nsegm - 2].changeType(0);//
            segments[Nsegm - 2].setWeakPoint();
/////////////////////////NET!
            for (int k = 0; k < Nsegm - 1; k++) {
                //segments[k].changeType(0);//
                segments[k].setSaturation(false);
            }

        }
    }


    else{
        int k0=0;
        while (k0<Nsegm-1){
            if (segments[k0].getType()==0){
                //segments[k0].setSaturation(false);
                segments[k0].changeType(2);//
                break;
            }
            k0++;
        }


        int segmTypeSum=0;
        for (int k=0;k<Nsegm-1;k++) {
            if (segments[k].getType()==2)
                segmTypeSum++;
        }
        if (segmTypeSum!=Nsegm-1) {

            for (int k = 0; k < Nsegm - 1; k++) {
                //segments[k].changeType(0);//
                segments[k].setSaturation(false);
            }
        }
    }
}

int Protagonist::updateEat(std::vector<ChangeLevelFood>& changeLevelFood_array, std::vector<Food>& foods_array, std::vector<SnakeHunter>& snakeHunter_array,
                           std::vector<SharkHunter>& sharkHunter_array, std::vector<FlockieBird>& flockieBird_array, std::vector<Boss>& boss_array){
    //minus lvl ot damaga
    if (levelDownCosDamaged){
        segments[0].restoreWeakPoint();
        levelDownCosDamaged = false;
        return -1;
    }

    if (isEatingRightNow) return 0;

    float mouthDist = 30;
    float mouthRadius = 20;

    //Поедание еды на смену уровня
    for (auto & t : changeLevelFood_array){
        if (pow(currentX + mouthDist * cos(orientation) - t.getCurrentX(), 2) +
                pow(currentY + mouthDist * sin(orientation) - t.getCurrentY(), 2) <
                pow(mouthRadius + t.getCurrentRadius(), 2)) {
            isEatingRightNow = true;
            itWasVoidFood = true;
            return t.getType();
        }
    }

    //Поедание мелких
    for (auto & t : foods_array){
        if (!t.isEaten()) {
            if (pow(currentX + mouthDist * cos(orientation) - t.getCurrentX(), 2) + pow(currentY + mouthDist * sin(orientation) - t.getCurrentY(), 2) <
                    pow(mouthRadius + t.getCurrentRadius(), 2)) {
                isEatingRightNow = true;
                t.setEaten();
                return 0;
            }
        }
    }

    //Поедание змейки
    for (auto & t : snakeHunter_array){
        if (!t.isEaten()){
            for (int j = 0; j < t.getNsegm(); j++) {
                if (t.isSegmentWeakPointAndUndamaged(j)) {
                    if (pow(currentX + mouthDist * cos(orientation) - t.getCurrentSegX(j), 2) +
                            pow(currentY + mouthDist * sin(orientation) - t.getCurrentSegY(j), 2) <
                            pow(mouthRadius + t.getCurrentSegRadius(j), 2)) {//Радиус
                        isEatingRightNow = true;
                        t.setDamaged(j);
                        return 0;
                    }
                }
            }
        }
    }

    //Поедание акулы
    for (auto & t : sharkHunter_array){
        if (!t.isEaten()) {
            for (int j = 0; j < t.getNsegm(); j++) {
                if (t.isSegmentWeakPointAndUndamaged(j)) {
                    if (pow(currentX + mouthDist * cos(orientation) - t.getCurrentSegX(j), 2) +
                            pow(currentY + mouthDist * sin(orientation) - t.getCurrentSegY(j), 2) <
                            pow(mouthRadius + t.getCurrentSegRadius(j), 2)) {//Радиус
                        isEatingRightNow = true;
                        t.setDamaged(j);
                        return 0;
                    }
                }
            }
        }
    }

    //Поедание стайки
    for (auto & t : flockieBird_array){
        if (!t.areEaten()){
            for (int j=0;j<t.getNbirds();j++){
                if (!t.isEaten(j)){
                    if (pow(currentX + mouthDist * cos(orientation) - t.getCurrentX(j), 2) +
                            pow(currentY + mouthDist * sin(orientation) - t.getCurrentY(j), 2) <
                            pow(mouthRadius + t.getCurrentRadius(j), 2)) {
                        isEatingRightNow = true;
                        t.setDamaged(j);
                        return 0;
                    }
                }
            }
        }
    }


    //Поедание босса
    for (auto & t : boss_array){
        if (!t.isEaten()) {
            for (int j = 0; j < t.getNsegm(); j++) {
                if (t.isSegmentWeakPointAndUndamaged(j)) {
                    if (pow(currentX + mouthDist * cos(orientation) - t.getCurrentSegX(j), 2) +
                            pow(currentY + mouthDist * sin(orientation) - t.getCurrentSegY(j), 2) <
                            pow(mouthRadius + t.getCurrentSegRadius(j), 2)) {//Радиус
                        isEatingRightNow = true;
                        t.setDamaged(j);
                        return 0;
                    }
                }
            }
        }
    }/*
    for (int i = 0; i < boss_array.length; i++) {
        if (!t.isEaten()) {


            for (int j = 0; j < t.getNsegm(); j++) {
                if (t.isSegmentWeakPointAndUndamaged(j)) {
                    if (pow(currentX + mouthDist * Math.cos(orientation) - t.getCurrentSegX(j), 2) +
                            pow(currentY + mouthDist * Math.sin(orientation) - t.getCurrentSegY(j), 2) <
                            pow(mouthRadius + t.getCurrentSegRadius(j), 2)) {//Радиус
                        isEatingRightNow = true;
                        t.setDamaged(j);
                        return 0;
                    }
                }
            }

        }
    }*/



    return 0;
}

int Protagonist::getNsegm(){
    return Nsegm;
}
bool Protagonist::isSegmentWeakPointAndUndamaged(int nSegm){
    return segments[nSegm].isSegmentWeakPointAndUndamaged();
}
float Protagonist::getCurrentSegX(int nSegm){
    return segments[nSegm].getCurrentX();
}
float Protagonist::getCurrentSegY(int nSegm){
    return segments[nSegm].getCurrentY();
}
float Protagonist::getCurrentSegRadius(int nSegm){
    return segments[nSegm].getCurrentRadius();
}
void Protagonist::setDamaged(int nSegm){
    segments[nSegm].setWeakPointDamaged();
    int healthRemain=0;
    for (int i=0;i<Nsegm;i++){
        if (segments[i].isSegmentWeakPointAndUndamaged()){
            healthRemain++;
        }
    }
    if (healthRemain==0){
        levelDownCosDamaged=true;
    }
}
