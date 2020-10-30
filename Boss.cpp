#include "Boss.h"
#include "widget.h"/////////////////?Отрисовка
#include "Food.h"
#include "Protagonist.h"

#include "random.hpp"

Boss::Boss(int bossType_)
{
    bossType=bossType_;
    //Log.wtf(LOG_TAG,"Bosstype="+this.bossType);

    currentX = effolkronium::random_static::get<float>(-maxRange,maxRange);
    currentY = effolkronium::random_static::get<float>(-maxRange,maxRange);

    this->goToRandomLocation();

    //orientationAim = 0;
    turnSpeed = effolkronium::random_static::get<float>(40, 55) / 180. * M_PI;//(float) ((8f + (float) Math.random() * 3f) * 5 / 180f * M_PI);//*20

    //Log.wtf(LOG_TAG,"turnspeed "+turnSpeed*180/3.1415);

    maxSpeed = 140;
    maxBoostSpeed = maxSpeed*3;
    //screenSizeX = screenSizeX_;
    //screenSizeY = screenSizeY_;


    agroRadius=maxBoostSpeed/turnSpeed*1.5f;
    agroQuietRadius=agroRadius*1.2f;

    orientation = effolkronium::random_static::get<float>(0, 2 * M_PI);
    currentSpeed = effolkronium::random_static::get<float>(0, maxSpeed);


    canvasSize = canvasSnake = canvasEat = canvasSegmentMovement = 0;

    Nsegm = 12;


    //segments = new Segment[Nsegm];
    //angryFood = new Food[Nsegm-3];//Столько пустых точечных сегментов

    for (int i=0;i<Nsegm;i++){
        if (i<3){
            segments.push_back(Segment(3,1));//Три летающие точки
        }
        else{
            segments.push_back(Segment(4,1));//Мелкие сегменты
            angryFood.push_back(Food(100));//Первый тип босса, акула с злой "едой"
        }
    }
}

void Boss::goToRandomLocation(){
    aimX = effolkronium::random_static::get<float>(-maxRange,maxRange);
    aimY = effolkronium::random_static::get<float>(-maxRange,maxRange);
}

float Boss::getOrientationInDegrees(){
    return orientation * 180 / M_PI;
}

void Boss::draw(Widget& widget){
    if (isInDivision){
        //Log.wtf(LOG_TAG,"inDivision Nsegm="+Nsegm+" divTimer"+divisionTimer);
        for (int k = 0; k < Nsegm; k++) {
            if (divisionTimer-k*0.1f<0){
                segments[k].draw(widget);
            }
            else{
                segments[k].drawDivision(widget, divisionTimer-k*0.1f);
            }
        }
    }

    if (!isEaten) {

        if (isAgro){
            widget.setColor(1, 0, 0, 1);//Red
        }

        widget.drawBossBody(currentX,currentY,this->getOrientationInDegrees()+180,abs(canvasSnake - 0.5f) * 2,1);

        widget.drawSharkmouthTransfered(currentX,currentY,1*0.3f,this->getOrientationInDegrees(),294*0.97f,0,(canvasSize+canvasEat)*360+this->getOrientationInDegrees());

        widget.drawRing3Transfered(currentX,currentY,103*0.3f,this->getOrientationInDegrees(),294*0.97f,0);
        widget.drawRing3Transfered(currentX,currentY,153*0.3f,this->getOrientationInDegrees(),270,0);

        if (isAgro){
            widget.setColor(1, 1, 1, 0.47);//Standart white
        }

        for (int k = 0; k < Nsegm; k++) {
            segments[k].draw(widget);
        }


    }

    for (int i=0;i<Nsegm-3;i++){
        angryFood[i].drawAngryBossFood(widget);
    }
}

void Boss::updateMapPosition(float dt, Protagonist protagonist){


    if (isInDivision){
        if (divisionTimer>(Nsegm+1)*0.1+1) isInDivision=false;//С запасом. А так NsegmMax*0.1+1
        //Log.wtf(LOG_TAG, "Разделение на части в процессе! "+divisionTimer);
        divisionTimer=divisionTimer+dt;


    }



    if (!isEaten) {
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
                                        /*float orientationDelta = remainderf(orientationAim - orientation, M_PI * 2);
                                        if (abs(orientationDelta) > turnSpeed * dt) {//Если изменение угла не слишком маленькое*/
                orientationAim = (float) atan2(aimY - currentY, aimX - currentX);
                float orientationDelta = remainderf(orientationAim - orientation, M_PI * 2);
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

                        //Log.wtf(LOG_TAG, "Рандомный путь. Cброс агры по времени");
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
                    float orientationDelta = remainderf(orientationAim - orientation, M_PI * 2);
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
        canvasSize = fmodf(canvasSize + dt * 0.4f, 1);
        canvasSnake = fmodf(canvasSnake + dt * currentSpeed / 100. * 0.5f, 1);
        if (isEatingRightNow) {
            canvasEat = (canvasEat + dt * 0.2f * 2);
            if (canvasEat >= 2) {
                canvasEat = 0;
                isEatingRightNow = false;
            }
        }
        canvasSegmentMovement = fmodf(canvasSegmentMovement + dt * 0.4f, 3);
        //Log.wtf(LOG_TAG,"canvasSM ");
        float time=fmodf(canvasSegmentMovement, 1);
        time=0.5f-0.5f*(float)cos(M_PI*time);
        for (int i=0;i<3;i++) {
            if (fmodf(canvasSegmentMovement + 3 - i, 3) < 1) {
                //Первое движение
                segments[i].updateMapPosition(currentX + (294.*0.97f*(1-time)+80.*time) * (float) cos(orientation) - (206*time) * (float) sin(orientation),
                        currentY + (294.*0.97f*(1-time)+80.*time) * (float) sin(orientation) + (206*time) * (float) cos(orientation),
                        orientation + (float) M_PI);
                //Log.wtf(LOG_TAG,"canvasSM "+i+" "+time);
            } else {
                if (fmodf(canvasSegmentMovement + 3 - i, 3) < 2) {
                    //Второе движение
                    segments[i].updateMapPosition(currentX + (80.) * (float) cos(orientation) - (206*(1-time)+(-206)*time) * (float) sin(orientation),
                            currentY + (80.) * (float) sin(orientation) + (206*(1-time)+(-206)*time) * (float) cos(orientation),
                            orientation + (float) M_PI);
                } else {
                    //Третье движение
                    segments[i].updateMapPosition(currentX + (294.*0.97f*time+80.*(1-time)) * (float) cos(orientation) - (-206*(1-time)) * (float) sin(orientation),
                            currentY + (294.*0.97f*time+80.*(1-time)) * (float) sin(orientation) + (-206*(1-time)) * (float) cos(orientation),
                            orientation + (float) M_PI);
                }
            }
        }



        //Обновление сегментов
        /*
        segments[0].updateMapPosition(currentX + 206 * (float) sin(orientation) + 80f * (float) cos(orientation),
                currentY - 206 * (float) cos(orientation)+ 80f * (float) sin(orientation), orientation + (float) M_PI);
        segments[1].updateMapPosition(currentX - 206 * (float) sin(orientation) + 80f * (float) cos(orientation),
                currentY + 206 * (float) cos(orientation)+ 80f * (float) sin(orientation), orientation + (float) M_PI);
        segments[2].updateMapPosition(currentX + 294f*0.97f * (float) cos(orientation),
                currentY + 294f*0.97f * (float) sin(orientation), orientation + (float) M_PI);
*/


        segments[3].updateMapPosition(currentX + 246 * (float) cos(orientation),
                currentY + 246 * (float) sin(orientation), orientation + (float) M_PI);
        segments[4].updateMapPosition(currentX + (-40) * (float) cos(orientation)- (16) * (float) sin(orientation),
                currentY + (-40) * (float) sin(orientation) + (16) * (float) cos(orientation),
                orientation + (float) M_PI);
        segments[5].updateMapPosition(currentX + (52) * (float) cos(orientation)- (68) * (float) sin(orientation),
                currentY + (52) * (float) sin(orientation) + (68) * (float) cos(orientation),
                orientation + (float) M_PI);
        segments[6].updateMapPosition(currentX + (118) * (float) cos(orientation)- (121) * (float) sin(orientation),
                currentY + (118) * (float) sin(orientation) + (121) * (float) cos(orientation),
                orientation + (float) M_PI);
        segments[7].updateMapPosition(currentX + (174) * (float) cos(orientation)- (62) * (float) sin(orientation),
                currentY + (174) * (float) sin(orientation) + (62) * (float) cos(orientation),
                orientation + (float) M_PI);
        segments[8].updateMapPosition(currentX + (15) * (float) cos(orientation)- (-75) * (float) sin(orientation),
                currentY + (15) * (float) sin(orientation) + (-75) * (float) cos(orientation),
                orientation + (float) M_PI);
        segments[9].updateMapPosition(currentX + (82) * (float) cos(orientation)- (-49) * (float) sin(orientation),
                currentY + (82) * (float) sin(orientation) + (-49) * (float) cos(orientation),
                orientation + (float) M_PI);
        segments[10].updateMapPosition(currentX + (148) * (float) cos(orientation)- (-96) * (float) sin(orientation),
                currentY + (148) * (float) sin(orientation) + (-96) * (float) cos(orientation),
                orientation + (float) M_PI);
        segments[11].updateMapPosition(currentX + (184) * (float) cos(orientation)- (-44) * (float) sin(orientation),
                currentY + (184) * (float) sin(orientation) + (-44) * (float) cos(orientation),
                orientation + (float) M_PI);
        /*
        segments[3].updateMapPosition(currentX - 119 * scaleForLittleOrBigFish * (float) cos(orientation),
                currentY - 119 * scaleForLittleOrBigFish * (float) sin(orientation), orientation + (float) M_PI);
        segments[4].updateMapPosition(currentX - 516 * scaleForLittleOrBigFish * (float) cos(orientation),
                currentY - 516 * scaleForLittleOrBigFish * (float) sin(orientation), orientation + (float) M_PI);
*/
    }

    for (int i=0;i<Nsegm-3;i++){
        angryFood[i].updateMapPositionAngryBoss(dt,protagonist);
    }
}
