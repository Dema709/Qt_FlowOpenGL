#include "SharkHunter.h"
#include "random.hpp"
#include "widget.h"/////////////////?Отрисовка

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

}
