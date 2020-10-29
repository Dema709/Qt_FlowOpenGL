#include "FlockieBird.h"
#include "Food.h"
#include "widget.h"/////////////////?Отрисовка
#include "random.hpp"

FlockieBird::FlockieBird(int Nbirds_)
{
    Nbirds = Nbirds_;
    NbirdsNow = Nbirds_;

    currentX = effolkronium::random_static::get<float>(-maxRange,maxRange);
    currentY = effolkronium::random_static::get<float>(-maxRange,maxRange);

    int birdType_ = effolkronium::random_static::get<int>(0, 6);

    for (int i=0; i<Nbirds; i++){
        foods_array.push_back(Food(currentX,currentY,birdType_));
    }
}

void FlockieBird::updateMapPosition(float dt){
    if (areEaten_) return;

    if (isPanic) {
        if (panicTimer > panicMaxTime) {
            isPanic = false;
            currentX=0;currentY=0;
            for (int j=0;j<Nbirds;j++){
                if (!foods_array[j].isEaten()) {
                    currentX = currentX + foods_array[j].getCurrentX();
                    currentY = currentY + foods_array[j].getCurrentY();
                }
            }
            currentX=currentX/NbirdsNow;
            currentY=currentY/NbirdsNow;
            this->goToRandomLocation();
        }
        panicTimer = panicTimer + dt;
    }//Обновление таймера паники
    if (!isPanic){
        if (abs(currentX - aimX)<10&&abs(currentY - aimY)<10){
            this->goToRandomLocation();
        }

        //Обновление передвижения центра стаи
        float orientationAim = (float) atan2(aimY - currentY, aimX - currentX);
        currentX = currentX + speed * (float) cos(orientationAim) * dt;
        currentY = currentY + speed * (float) sin(orientationAim) * dt;
    }
    else{
        for (int j=0;j<Nbirds;j++){
            if (!foods_array[j].isEaten())
                foods_array[j].setOrientationAim((float) (atan2(foods_array[j].getCurrentY() - panicCenterY, foods_array[j].getCurrentX() - panicCenterX) +
                                                          //(Math.random()-0.5)*2*0.17  ));
                                                          effolkronium::random_static::get<float>(-0.17, 0.17)));
        }//+-10 градусов
    }

    for (int j=0;j<Nbirds;j++){
        foods_array[j].updateMapPositionBird(dt,isPanic,currentX,currentY);
    }
}

void FlockieBird::goToRandomLocation(){
    aimX = effolkronium::random_static::get<float>(-maxRange,maxRange);
    aimY = effolkronium::random_static::get<float>(-maxRange,maxRange);
}

void FlockieBird::draw(Widget& widget){
    for (auto & t : foods_array){
        if (!t.isEaten()){
            t.draw(widget);
        }
    }
}

bool FlockieBird::areEaten(){
    return areEaten_;
}

int FlockieBird::getNbirds(){
    return Nbirds;
}

bool FlockieBird::isEaten(int N_){
    return foods_array[N_].isEaten();
}

float FlockieBird::getCurrentX(int N_){
    return foods_array[N_].getCurrentX();
}
float FlockieBird::getCurrentY(int N_){
    return foods_array[N_].getCurrentY();
}
float FlockieBird::getCurrentRadius(int N_){
    return foods_array[N_].getCurrentRadius();
}

void FlockieBird::setDamaged(int nBird){
    foods_array[nBird].setEaten();
            NbirdsNow--;
            if (NbirdsNow==0){
                areEaten_=true;
                return;
            }
    /////////Паника

            isPanic=true;
            panicTimer=0;
            panicCenterX=foods_array[nBird].getCurrentX();
            panicCenterY=foods_array[nBird].getCurrentY();
}
