#include "SnakeHunter.h"
#include "random.hpp"
#include "Food.h"
#include "widget.h"/////////////////?Отрисовка

SnakeHunter::SnakeHunter(int Nsegm_, int NsegmEvolved_)
{
    currentX = effolkronium::random_static::get<float>(-maxRange, maxRange);
    currentY = effolkronium::random_static::get<float>(-maxRange, maxRange);
    aimX = effolkronium::random_static::get<float>(-maxRange, maxRange);
    aimY = effolkronium::random_static::get<float>(-maxRange, maxRange);
    maxTurnSpeed = effolkronium::random_static::get<float>(80, 80+30) / 180. * M_PI;
    orientation = effolkronium::random_static::get<float>(0, 2 * M_PI);
    currentSpeed = effolkronium::random_static::get<float>(0, maxSpeed);

    Nsegm = std::min(Nsegm_, NsegmMax);

    for (int k = 0; k < Nsegm; k++) {
        if (k==0){
            //Первый сегмент
            segments.push_back(Segment(currentX, currentY,
                    orientation + effolkronium::random_static::get<float>(-30, 30) / 180. * M_PI,
                    0));
            segments[0].setFirst();
            segments[0].setSaturation(true);
            segments[0].setWeakPoint();
        } else if (k!=Nsegm-1){
            //Средние сегменты
            segments.push_back(Segment(segments[k-1].getCurrentX(), segments[k-1].getCurrentY(),
                    segments[k-1].getOrientation() + effolkronium::random_static::get<float>(-30, 30) / 180. * M_PI,
                    0));
        } else {
            //Хвост
            segments.push_back(Segment(segments[k-1].getCurrentX(), segments[k-1].getCurrentY(),
                    segments[k-1].getOrientation() + effolkronium::random_static::get<float>(-30, 30) / 180. * M_PI,
                    1));
        }
    }

    int currentRandom;
    for (int k = 0; k < NsegmEvolved_-1; k++) {
        currentRandom = effolkronium::random_static::get<int>(0, Nsegm - 2);//random.nextInt(Nsegm-1);
        if (!segments[currentRandom].isWeakPoint()) {
            segments[currentRandom].setWeakPoint();
        }
        else{
            for (int j = 1; j < Nsegm-1; j++) {//C 1, т.к. нулевой всегда weakpoint
                if (!segments[j].isWeakPoint()){
                    segments[j].setWeakPoint();
                    break;
                }
            }
        }
    }
}

void SnakeHunter::draw(Widget& widget){
    if (isInDivision){
        for (int k = 0; k < Nsegm; k++) {
            if (divisionTimer-k*0.1f<0){
                segments[k].drawWithScale(widget,k,Nsegm);
            }
            else{
                segments[k].drawDivision(widget,divisionTimer-k*0.1f);////////////Фуньк-пуньк с задержкой
            }
        }
    }

    if (!isEaten_) {
        widget.setColor(1, 1, 1, 0.47);//Standart white
        widget.drawRing2(currentX,currentY,2*0.7f*5.3f);
        widget.drawMouth(currentX,currentY,this->getOrientationInDegrees(),1-abs(fmodf(canvasEat, 1)-0.5f)*2);
        widget.drawSquareTransfered(currentX,currentY,3.36f,this->getOrientationInDegrees(),-21,0);

        for (int k=0;k<Nsegm;k++){
            segments[k].drawWithScale(widget,k,Nsegm);
        }
    }
}

float SnakeHunter::getOrientationInDegrees(){
    return orientation * 180 / M_PI;
}

void SnakeHunter::updateMapPosition(float dt){


    if (isInDivision){
        if (divisionTimer>(NsegmMax+1)*0.1+1) isInDivision=false;//С запасом. А так NsegmMax*0.1+1
        //Log.wtf(LOG_TAG, "Разделение на части в процессе! "+divisionTimer);
        divisionTimer=divisionTimer+dt;
    }

    if (isEaten_) return;

    if (isPanic) {
        if (panicTimer > panicMaxTime) {
            isPanic = false;
            this->goToRandomLocation();
        }
        panicTimer = panicTimer + dt;
    }


    if (!isPanic) {
        if (!hasTarget) {
            if ((abs(aimX - currentX) < (200)) & (abs(aimY - currentY) < (200))) {
                this->goToRandomLocation();
            }
            //Если цель близко
            //Рандомить цель (и при задании!)
            //Если приближается к цели или время прошло - менять (время - постоянная неспокойного поиска))
        } else {
            //Уменьшать скорость при приближении?
        }



        //Угол
        orientationAim = (float) atan2(aimY - currentY, aimX - currentX);

        float orientationDelta = remainderf(orientationAim - orientation, M_PI * 2);//(orientationAim - orientation) % ((float) M_PI * 2);

        if (abs(orientationDelta) > maxTurnSpeed * dt) {//Если изменение угла не слишком маленькое
            if (orientationDelta>0){//if ((orientationDelta <= -M_PI) || ((orientationDelta > 0) && (orientationDelta <= M_PI))) {
                orientation = orientation + maxTurnSpeed * dt;
            } else {
                orientation = orientation - maxTurnSpeed * dt;
            }
        } else {
            orientation = orientationAim;
        }

    }
    //Скорость

    //if (isPressed) {
    if (isPanic) {
        currentSpeed = std::min(maxBoostSpeed, currentSpeed + dt * 400);//400 - прирост скорости?
    } else {
        if (currentSpeed > maxSpeed) {//Теряем скорость после ускорения
            //currentSpeed = std::max(currentSpeed*0.98f*dt*30,maxSpeed);//FPS=30
            currentSpeed = std::max(currentSpeed * powf(0.98, dt * 30), maxSpeed);//FPS=30//////////////////////////////////////////////////////Зависимость от фпс! (?)
        } else {
            currentSpeed = std::min(maxSpeed, currentSpeed + dt * 400 * 0.5f);
        }
    }




    // currentSpeed=std::max(maxSpeed*0.1f,currentSpeed);Ненулевая минимальная скорость

    //Позиция
    currentX = currentX + currentSpeed * (float) cos(orientation) * dt;
    currentY = currentY + currentSpeed * (float) sin(orientation) * dt;

    //Log.wtf(LOG_TAG, "touch ("+touchX+","+touchY+") curX=("+currentX+","+currentY+")");
    //Log.wtf(LOG_TAG, "addX="+currentSpeed * (float) cos(orientation) * dt+" addY="+currentSpeed * (float) sin(orientation) * dt);
    //return "touchX="+(int)touchX+" touchY="+(int)touchY;


    //Обновление для канвы - изменение размера
    canvasSize = fmodf(canvasSize + dt * 0.8, 1);
    canvasSnake = fmodf(canvasSnake + dt * currentSpeed / maxSpeed, 1);
    if (isEatingRightNow) {
        canvasEat = (canvasEat + dt * 0.8f * 2);
        if (canvasEat >= 2) {
            canvasEat = 0;
            if (itWasVoidFood) {//Не насыщать клетки, если перешёл на другой уровень
                itWasVoidFood = false;
            } else {
                this->evolveLittle();
            }
            isEatingRightNow = false;
        }
    }


    for (int k = 0; k < Nsegm; k++) {
        if (k == 0) {
            segments[k].updateMapPosition(currentX, currentY, dt, currentSpeed);
        } else {
            segments[k].updateMapPosition(segments[k - 1].getCurrentX(), segments[k - 1].getCurrentY(), dt, currentSpeed);
        }
    }

}

void SnakeHunter::goToRandomLocation(){
    aimX = effolkronium::random_static::get<float>(-maxRange,maxRange);
    aimY = effolkronium::random_static::get<float>(-maxRange,maxRange);
}

void SnakeHunter::evolveLittle(){
    int saturationSum=0;
    for (int k=0;k<Nsegm-1;k++) {
        if (segments[k].getSaturation())
            saturationSum++;
    }
    if (saturationSum==Nsegm-1){
        this->evolveBig();
    }
    else{
        //Ищи наименьшую пустую и наполняй
        int k=0;
        while (k<Nsegm-1){
            if (!segments[k].getSaturation()){
                segments[k].setSaturation(true);
                break;
            }
            k++;
        }
    }
}

void SnakeHunter::evolveBig(){
    //А вот здесь пошла реальная эволюция xD

    std::vector<int> k_types(3);//int [] k_types;
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
            //segments[Nsegm - 1] = new Segment(currentX, currentY, segments[Nsegm - 2].getOrientation(), 1);//Хвост
            //segments[Nsegm - 1] = new Segment(segments[Nsegm - 2].getCurrentX(), segments[Nsegm - 2].getCurrentY(), segments[Nsegm - 2].getOrientation(), 1);//Хвост
            segments.push_back(Segment(segments[Nsegm - 2].getCurrentX(), segments[Nsegm - 2].getCurrentY(), segments[Nsegm - 2].getOrientation(), 1));
            segments[Nsegm - 2].changeType(0);//
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

bool SnakeHunter::isEaten(){
    return isEaten_;
}

int SnakeHunter::getNsegm(){
    return Nsegm;
}

bool SnakeHunter::isSegmentWeakPointAndUndamaged(int nSegm){
    return segments[nSegm].isSegmentWeakPointAndUndamaged();
}

float SnakeHunter::getCurrentSegX(int nSegm){return segments[nSegm].getCurrentX();}
float SnakeHunter::getCurrentSegY(int nSegm){return segments[nSegm].getCurrentY();}
float SnakeHunter::getCurrentSegRadius(int nSegm){return segments[nSegm].getCurrentRadius();}

void SnakeHunter::setDamaged(int nSegm){
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
        panicTimer=0;
    }//Больно! Надо ускориться! Паника!
}

void SnakeHunter::findNearFood(std::vector<Food>& foods_array){
    if (!isEaten_){
        if (!isPanic) {
            if (!isEatingRightNow) {
                float mouthDist = 30;
                float mouthRadius = 20;
                hasTarget = false;

                //Поедание мелких
                for (int i = 0; i < foods_array.size(); i++) {
                    if (!foods_array[i].isEaten()) {
                        if (pow(currentX + mouthDist * cos(orientation) - foods_array[i].getCurrentX(), 2) +
                                pow(currentY + mouthDist * sin(orientation) - foods_array[i].getCurrentY(), 2) <
                                pow(mouthRadius + foods_array[i].getCurrentRadius(), 2)) {
                            isEatingRightNow = true;
                            foods_array[i].setEaten();
                            return;
                        }//Съедена еда

                        if (pow(currentX - foods_array[i].getCurrentX(), 2) + pow(currentY - foods_array[i].getCurrentY(), 2) < pow(agroRadius, 2)) {
                            if (hasTarget) {
                                if (pow(currentX - foods_array[i].getCurrentX(), 2) + pow(currentY - foods_array[i].getCurrentY(), 2) < pow(currentX - aimX, 2) + pow(currentY - aimY, 2)) {
                                    aimX = foods_array[i].getCurrentX();
                                    aimY = foods_array[i].getCurrentY();
                                }//Проверка, что ближе: старая цель или новая
                            } else {
                                aimX = foods_array[i].getCurrentX();
                                aimY = foods_array[i].getCurrentY();
                                hasTarget = true;
                            }
                        }//Еда попала в агрорадиус
                    }
                }
            }
        }
    }
    else {
        if (!isDivisionWrittenInFood){
            //Log.wtf(LOG_TAG,"Разваливается на еду");
            int k=0;
            for (int i=0;i<Nsegm;i++){
                if (segments[i].hasSaturationOrIsWeakPoint()){
                    while (k<foods_array.size()){
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
