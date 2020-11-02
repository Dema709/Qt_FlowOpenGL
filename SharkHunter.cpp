#include "SharkHunter.h"
#include "random.hpp"
#include "widget.h"/////////////////?Отрисовка
#include "Food.h"
#include "Protagonist.h"

SharkHunter::SharkHunter()
{
    currentX = effolkronium::random_static::get<float>(-maxRange, maxRange);
    currentY = effolkronium::random_static::get<float>(-maxRange, maxRange);

    this->goToRandomLocation();

    turnSpeed = effolkronium::random_static::get<float>(40, 55) / 180. * M_PI;

    maxSpeed = 100*(scaleForLittleOrBigFish*10);
    maxBoostSpeed = maxSpeed*3;

    agroRadius=maxBoostSpeed/turnSpeed*1.3f;
    agroQuietRadius=agroRadius*1.2f;

    orientation = effolkronium::random_static::get<float>(0, 2 * M_PI);
    currentSpeed = effolkronium::random_static::get<float>(0, maxSpeed);

    canvasSize = canvasSnake = canvasEat = 0;

    Nsegm = 5;

    segments.push_back(Segment(3,scaleForLittleOrBigFish*5));//Глаза
    segments.push_back(Segment(3,scaleForLittleOrBigFish*5));//Автоматом с насыщением и weakpoint
    segments.push_back(Segment(0,scaleForLittleOrBigFish*7));
    segments.push_back(Segment(0,scaleForLittleOrBigFish*6));//Ячейки по центру
    segments.push_back(Segment(0,scaleForLittleOrBigFish*5));
}

void SharkHunter::goToRandomLocation(){
    aimX = effolkronium::random_static::get<float>(-maxRange,maxRange);
    aimY = effolkronium::random_static::get<float>(-maxRange,maxRange);
}

void SharkHunter::updateMapPosition(float dt){

    if (isInDivision){
        if (divisionTimer>(Nsegm+1)*0.1+1) isInDivision=false;//С запасом. А так NsegmMax*0.1+1
        divisionTimer=divisionTimer+dt;
    }

    //Log.wtf(LOG_TAG,"panic "+isPanic+" agro "+isAgro);
    if (!isEaten_) {
        if (isPanic) {
            if (panicTimer > panicMaxTime) {
                isPanic = false;
                this->goToRandomLocation();
            }
            panicTimer = panicTimer + dt;
        }//Обновление таймера паники

        if (isPanic) {
            currentSpeed = std::min(maxBoostSpeed, currentSpeed + dt * 800);
        }
        else{//Паники нет
            if (hasPlayerInTarget){
                orientationAim = (float) atan2(aimY - currentY, aimX - currentX);
                float orientationDelta = remainderf(orientationAim - orientation, M_PI * 2);//(orientationAim - orientation) % ((float) M_PI * 2);
                //turnSpeed = maxTurnSpeed;
                if (abs(orientationDelta) > turnSpeed * dt) {//Если изменение угла не слишком маленькое
                    if (orientationDelta>0){//if ((orientationDelta <= -M_PI) || ((orientationDelta > 0) && (orientationDelta <= M_PI))) {
                        orientation = orientation + turnSpeed * dt;
                    } else {
                        orientation = orientation - turnSpeed * dt;
                    }
                } else {
                    orientation = orientationAim;
                }
                currentSpeed=maxBoostSpeed;
            }//Видна цель - гг. В агре движется на неё
            else{
                if (isAgro) {
                    if (agroTimer > agroMaxTimeWithoutAim) {
                        isAgro = false;

                        this->goToRandomLocation();
                    }
                    agroTimer = agroTimer + dt;
                }//Обновление таймера агрессии
                if (isAgro){
                    currentSpeed=maxBoostSpeed;//Лишнее действие. И можно замедлять чуток, если нет цели в виде гг
                }//Агрессия осталась, но пролетел мимо гг
                else{
                    if (!hasTarget){
                        if ((abs(aimX - currentX) < (200)) & (abs(aimY - currentY) < (200))) {
                            //Log.wtf(LOG_TAG, "Рандомный путь при приближении");
                            this->goToRandomLocation();
                        }
                    }//Обновление точки маршрута при приближении

                    orientationAim = (float) atan2(aimY - currentY, aimX - currentX);
                    float orientationDelta = remainderf(orientationAim - orientation, M_PI * 2);//(orientationAim - orientation) % ((float) M_PI * 2);
                    //turnSpeed = maxTurnSpeed;
                    if (abs(orientationDelta) > turnSpeed * dt) {//Если изменение угла не слишком маленькое
                        if (orientationDelta>0){//if ((orientationDelta <= -M_PI) || ((orientationDelta > 0) && (orientationDelta <= M_PI))) {
                            orientation = orientation + turnSpeed * dt;
                        } else {
                            orientation = orientation - turnSpeed * dt;
                        }
                    } else {
                        orientation = orientationAim;
                    }
                    if (currentSpeed > maxSpeed) {//Теряем скорость после ускорения
                        currentSpeed = std::max(currentSpeed * (float) powf(0.98, dt * 30), maxSpeed);//FPS=30
                    } else {
                        currentSpeed = std::min(maxSpeed, currentSpeed + dt * 400 * 0.5f);
                    }
                }//Не агрессия
            }
        }

        //Позиция
        currentX = currentX + currentSpeed * (float) cos(orientation) * dt;
        currentY = currentY + currentSpeed * (float) sin(orientation) * dt;
        //Обновление для канвы - изменение размера
        canvasSize = fmodf(canvasSize + dt * 0.8f, 1);
        canvasSnake = fmodf(canvasSnake + dt * currentSpeed / 100. * 0.5f, 1);
        if (isEatingRightNow) {
            canvasEat = (canvasEat + dt * 0.8f * 2);
            if (canvasEat >= 2) {
                canvasEat = 0;

                    this->evolve(false);


                isEatingRightNow = false;
            }
        }

        segments[0].updateMapPosition(currentX + 302 * scaleForLittleOrBigFish * (float) sin(orientation),
                currentY - 302 * scaleForLittleOrBigFish * (float) cos(orientation), orientation + (float) M_PI);
        segments[1].updateMapPosition(currentX - 302 * scaleForLittleOrBigFish * (float) sin(orientation),
                currentY + 302 * scaleForLittleOrBigFish * (float) cos(orientation), orientation + (float) M_PI);

        segments[2].updateMapPosition(currentX + 278 * scaleForLittleOrBigFish * (float) cos(orientation),
                currentY + 278 * scaleForLittleOrBigFish * (float) sin(orientation), orientation + (float) M_PI);
        segments[3].updateMapPosition(currentX - 119 * scaleForLittleOrBigFish * (float) cos(orientation),
                currentY - 119 * scaleForLittleOrBigFish * (float) sin(orientation), orientation + (float) M_PI);
        segments[4].updateMapPosition(currentX - 516 * scaleForLittleOrBigFish * (float) cos(orientation),
                currentY - 516 * scaleForLittleOrBigFish * (float) sin(orientation), orientation + (float) M_PI);

    }
}

void SharkHunter::evolve(bool firstTimeCall){
    //Первым делом необходимо восстановить "глаза" как слабые точки

    if (firstTimeCall) {
        int k = 0;
        while (k < Nsegm) {
            if (segments[k].isWeakPointDamaged()) {//сработает только на weakPoint
                segments[k].restoreWeakPoint();
                evolvedAlready = true;
                break;
            }
            k++;
        }
        evolvedAlready = false;
    }
    else{
        if (!evolvedAlready) {
            int k = 0;
            while (k < Nsegm) {
                if (segments[k].isWeakPointDamaged()) {//сработает только на weakPoint
                    segments[k].restoreWeakPoint();
                    evolvedAlready = true;
                    break;
                }//Восстановление weakPoint
            if (!segments[k].getSaturation()) {
                segments[k].setSaturation(true);
                evolvedAlready = true;
                break;
            }//Обычный нажор

                k++;
            }
        }
    }
}

void SharkHunter::draw(Widget& widget){
    widget.setColor(1, 1, 1, 0.47);//Standart white
    if (isInDivision){
        for (int k = 0; k < Nsegm; k++) {
            if (divisionTimer-k*0.1f<0){
                segments[k].draw(widget);
            }
            else{
                segments[k].drawDivision(widget,divisionTimer-k*0.1f);
            }
        }
    }

    if (!isEaten_) {
        //if ((Math.abs(currentX - camera.getCurrentX()) < camera.getGamefieldHalfX() + 560 * scaleForLittleOrBigFish) && (Math.abs(currentY - camera.getCurrentY()) < camera.getGamefieldHalfY() + 560 * scaleForLittleOrBigFish)){
            if (isAgro){
                widget.setColor(1, 0, 0, 1);//Red
            }
            widget.drawSharkBody(currentX,currentY,this->getOrientationInDegrees(),abs(canvasSnake - 0.5f) * 2,0.1f);
            widget.drawRing3Transfered(currentX,currentY,103*0.1f,this->getOrientationInDegrees(),774*0.1f,0);
            widget.drawRing3Transfered(currentX,currentY,153*0.1f,this->getOrientationInDegrees(),724*0.1f,0);
            widget.drawSharkmouthTransfered(currentX,currentY,0.1f,this->getOrientationInDegrees(),774*0.1f,0,(canvasSize+canvasEat)*360+this->getOrientationInDegrees());
            if (isAgro){
                widget.setColor(1, 1, 1, 0.47);//Standart white
            }

            for (int k = 0; k < Nsegm; k++) {
                segments[k].draw(widget);
            }

        //}
    }
}

float SharkHunter::getOrientationInDegrees(){
    return orientation * 180 / M_PI;
}

bool SharkHunter::isEaten(){
    return isEaten_;
}

int SharkHunter::getNsegm(){
    return Nsegm;
}

bool SharkHunter::isSegmentWeakPointAndUndamaged(int nSegm){
    return segments[nSegm].isSegmentWeakPointAndUndamaged();
}

float SharkHunter::getCurrentSegX(int nSegm){return segments[nSegm].getCurrentX();}
float SharkHunter::getCurrentSegY(int nSegm){return segments[nSegm].getCurrentY();}
float SharkHunter::getCurrentSegRadius(int nSegm){return segments[nSegm].getCurrentRadius();}

void SharkHunter::setDamaged(int nSegm){
    segments[nSegm].setWeakPointDamaged();///////////////////////////////////////////////Можно будет объединить


    int weakUndamagedSum=0;
    for (int k=0;k<Nsegm-1;k++) {
        if (segments[k].isSegmentWeakPointAndUndamaged())///////////////////////////////Тут должна быть проверка на слабые точки аля weakpoints
            weakUndamagedSum++;
    }

    if (weakUndamagedSum==0){
        isInDivision=true;
        divisionTimer=0;
        isEaten_=true;/////////////////////////////////////////////////////////////////////////////И вызов ф-ии распада
    }//Вас сожрали нафиг ;р
    else{
        isPanic=true;
        isAgro=false;
        panicTimer=0;
        //Log.wtf(LOG_TAG, "Паника!");
    }//Больно! Надо ускориться! Паника!
}

void SharkHunter::findNearFood(std::vector<Food>& foods_array, Protagonist& protagonist){



    if (!isEaten_) {
        hasPlayerInTarget=false;
        hasTarget=false;
        if (!isPanic) {
            if (!isEatingRightNow) {
                float mouthDist = 774 * scaleForLittleOrBigFish;
                float curCheckX, curCheckY;//Положение текущей цели

                //Попытка съесть протагониста или сагриться на него
                for (int i = 0; i < protagonist.getNsegm(); i++) {
                    if (protagonist.isSegmentWeakPointAndUndamaged(i)) {
                        curCheckX=protagonist.getCurrentSegX(i);
                        curCheckY=protagonist.getCurrentSegY(i);


                        if (pow(currentX + mouthDist * cos(orientation) - curCheckX, 2) +
                                pow(currentY + mouthDist * sin(orientation) - curCheckY, 2) <
                                pow(103 * scaleForLittleOrBigFish + protagonist.getCurrentSegRadius(i), 2)) {
                            protagonist.setDamaged(i);
                            isEatingRightNow = true;this->evolve(true);
                            return;
                        }//Если попало в рот

                        if (!isAgro){
                            if (!hasTarget) {//Первый раз за цикл
                                if (pow(currentX - curCheckX, 2) + pow(currentY - curCheckY, 2) < pow(agroQuietRadius, 2)) {
                                    //Если в большом агрорадиусе
                                    aimX = curCheckX;
                                    aimY = curCheckY;
                                    hasTarget = true;
                                }
                            } else {
                                if (pow(currentX - curCheckX, 2) + pow(currentY - curCheckY, 2) <
                                        pow(currentX - aimX, 2) + pow(currentY - aimY, 2)) {
                                    //Новое расстояние меньше
                                    aimX = curCheckX;
                                    aimY = curCheckY;
                                }
                            }
                        }//Если не в агре - проверка на ближайшую цель в большом агрорадиусе


                        if (pow(currentX - curCheckX, 2) + pow(currentY - curCheckY, 2) < pow(agroRadius, 2)){
                            //Если попало в малый агрорадиус
                            if (abs(remainderf(atan2(curCheckY - currentY, curCheckX - currentX) - orientation, M_PI * 2)) < agroAngle){
                            //if (abs(((float) atan2(curCheckY - currentY, curCheckX - currentX) - orientation) % ((float) M_PI * 2)) < agroAngle){
                                //Если попало в агроугол
                                if (!hasPlayerInTarget) {//Первый раз за цикл
                                    aimX = curCheckX;
                                    aimY = curCheckY;
                                    isAgro = true;
                                    hasPlayerInTarget = true;
                                    agroTimer = 0;
                                } else {
                                    if (pow(currentX - curCheckX, 2) + pow(currentY - curCheckY, 2) < pow(currentX - aimX, 2) + pow(currentY - aimY, 2)) {
                                        //Новое расстояние меньше
                                        aimX = curCheckX;
                                        aimY = curCheckY;
                                    }
                                }
                            }
                        }








                    }
                }

                if (isAgro){
                    return;
                }//Если агры, нет смысла проверять еду

                for (int i = 0; i < foods_array.size(); i++){
                    if (!foods_array[i].isEaten()){
                        curCheckX=foods_array[i].getCurrentX();
                        curCheckY=foods_array[i].getCurrentY();

                        if (pow(currentX + mouthDist * cos(orientation) - curCheckX, 2) +
                                pow(currentY + mouthDist * sin(orientation) - curCheckY, 2) <
                                pow(103 * scaleForLittleOrBigFish + foods_array[i].getCurrentRadius(), 2)) {
                            foods_array[i].setEaten();
                            isEatingRightNow = true;this->evolve(true);
                            return;
                        }//Если попало в рот

                        if ((pow(currentX - curCheckX, 2) + pow(currentY - curCheckY, 2) < pow(agroRadius, 2)) &&
                                (abs(remainderf(atan2(curCheckY - currentY, curCheckX - currentX) - orientation, M_PI * 2)) < agroAngle)){
                                //(abs(((float) atan2(curCheckY - currentY, curCheckX - currentX) - orientation) % ((float) M_PI * 2)) < agroAngle)) {
                            //Если попало в агрорадиус и агроугол
                            if (!hasTarget) {//Первый раз за цикл
                                aimX = curCheckX;
                                aimY = curCheckY;
                                hasTarget = true;
                            } else {
                                if (pow(currentX - curCheckX, 2) + pow(currentY - curCheckY, 2) < pow(currentX - aimX, 2) + pow(currentY - aimY, 2)) {
                                    //Новое расстояние меньше
                                    aimX = curCheckX;
                                    aimY = curCheckY;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else{
        if (!isDivisionWrittenInFood){
            //Log.wtf(LOG_TAG,"Разваливается на еду");
            int k=0;
            for (int i=0;i<Nsegm;i++){
                if (segments[i].hasSaturationOrIsWeakPoint()){
                    while (k<foods_array.size()){//-1 справа лишнее
                        if (foods_array[k].isEatenAndNotInvisible()){
                            foods_array[k].setInvisible((float)i*0.1f+0.1f,segments[i].getCurrentX(),segments[i].getCurrentY());
                            k++;
                            break;
                        }
                        k++;
                    }
                }
                isDivisionWrittenInFood=true;
            }
        }
    }
}
