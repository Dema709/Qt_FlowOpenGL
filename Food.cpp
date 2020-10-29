#include "Food.h"
#include "random.hpp"
#include <cmath>//pow, atan2, sqrt
#include <algorithm>//max
#include <QDebug>
#include "widget.h"/////////////////?Отрисовка

Food::Food()
{
    currentX = effolkronium::random_static::get<float>(-maxRange,maxRange);
    currentY = effolkronium::random_static::get<float>(-maxRange,maxRange);
    orientation = effolkronium::random_static::get<float>(0.,2*M_PI);
    maxSpeed = 45 + effolkronium::random_static::get<float>(0, 10);
    currentSpeed = effolkronium::random_static::get<float>(0, maxSpeed);
    turnSpeed = 50. / 180. * M_PI;
    canvasSize = effolkronium::random_static::get<float>(0, 1);
    canvasSnake = effolkronium::random_static::get<float>(0, 1);
    isEaten_=false;
    type = effolkronium::random_static::get<int>(0,6);//randomize type есть ещё в одном месте
    this->goToRandomLocation();
    this->setRadius();
}

Food::Food(float curX_, float curY_, int birdType){
    currentX = curX_ + effolkronium::random_static::get<float>(-flockieBirdRadius,flockieBirdRadius);
    currentY = curY_ + effolkronium::random_static::get<float>(-flockieBirdRadius,flockieBirdRadius);
    orientation = effolkronium::random_static::get<float>(0.,2*M_PI);
    maxSpeed = effolkronium::random_static::get<float>(245, 255);//45+(float)random()*10f+200;
    currentSpeed = effolkronium::random_static::get<float>(0, maxSpeed);
    turnSpeed = (50. / 180. * M_PI)+1;
    this->goToRandomLocation();
    canvasSize = effolkronium::random_static::get<float>(0, 1);
    canvasSnake = effolkronium::random_static::get<float>(0, 1);
    type=birdType;
    this->setRadius();
    isEaten_=false;
}//Для стайки

void Food::goToRandomLocation(){
    aimX = effolkronium::random_static::get<float>(-maxRange,maxRange);
    aimY = effolkronium::random_static::get<float>(-maxRange,maxRange);
}

void Food::setRadius(){
    switch (type){
        case 0:
            currentRadius=20;
            break;
        case 1:
            currentRadius=20;
            break;
        case 2:
            currentRadius=15;
            break;
        case 3:
            currentRadius=10;
            break;
        case 4:
            currentRadius=10;
            break;
        case 5:
            currentRadius=14;
            break;
        case 6:
            currentRadius=20;
            break;
        case 7:
            currentRadius=20;
            break;
        case 8:
            currentRadius=20;
            break;
        default:
            currentRadius=20;
            qDebug()<<"Food::setRadius"<<"Тип еды задан неправильно. Радиус";
            break;
    }
}

void Food::updateMapPosition(float dt){
    if (isInvisible){
        if (invisibleTimer>0) {
            isInvisible=false;
            isEaten_=false;
        }
        invisibleTimer=invisibleTimer+dt;
    }

    if (!isEaten_) {
        if (sqrt(pow((currentX - aimX), 2) + pow((currentY - aimY), 2)) < 5) {
            goToRandomLocation();
        }

        //Угол
        orientationAim = (float) atan2(aimY - currentY, aimX - currentX);
        float orientationDelta = remainderf(orientationAim - orientation, M_PI * 2);//(orientationAim - orientation) % ((float) M_PI * 2);


        if (abs(orientationDelta) > turnSpeed * dt) {//Если изменение угла не слишком маленькое
            if (orientationDelta>0) {;//(orientationDelta <= -M_PI) || ((orientationDelta > 0) && (orientationDelta <= M_PI))) {
                orientation = orientation + turnSpeed * dt;
            } else {
                orientation = orientation - turnSpeed * dt;
            }
        } else {
            orientation = orientationAim;
        }

        //Скорость
        currentSpeed = std::min(maxSpeed, currentSpeed + dt * 50);

        //Позиция
        currentX = currentX + currentSpeed * (float) cos(orientation) * dt;
        currentY = currentY + currentSpeed * (float) sin(orientation) * dt;

        //Обновление для канвы - изменение размера
        switch (type) {
            case 0:
                canvasSize = fmodf(canvasSize + dt * 0.8, 1);//canvasSize = (canvasSize + dt * 0.8) % 1;
                canvasSnake = fmodf(canvasSnake + dt * currentSpeed / 100., 1);//(canvasSnake + dt * currentSpeed / 100f) % 1;
                multiplSize = abs(canvasSize - 0.5f) * 2;
                multiplSnake = abs(canvasSnake - 0.5f) * 2;
                break;
            case 1:
                canvasSize = fmodf(canvasSize + dt * 0.8, 1);//(canvasSize + dt * 0.8) % 1;
                multiplSize = (abs(canvasSize - 0.5f) * 2 * 0.3f + 0.7f)*20;
                break;
            case 2:
                canvasSize = fmodf(canvasSize + dt * 0.8, 1);//(canvasSize + dt * 0.8) % 1;
                canvasSnake = fmodf(canvasSnake + 1.8 * dt * currentSpeed / 100., 1);;//(canvasSnake + 1.8f * dt * currentSpeed / 100.) % 1;//Хвостик у головастика должен двигаться быстрее
                multiplSize = 1 - (float) pow(abs(abs(canvasSize - 0.5f) * 2 - 0.5) * 2, 2);//Вся эта конструкция для увеличения частоты вдвое и быстрого захлопывания пасти
                multiplSnake = abs(canvasSnake - 0.5f) * 2;
                break;
            case 3:
                canvasSnake = fmodf(canvasSnake + dt * currentSpeed / 100., 1);//(canvasSnake + dt * currentSpeed / 100.) % 1;
                multiplSnake = abs(canvasSnake - 0.5f) * 2;
                break;
            case 4:
                canvasSnake = fmodf(canvasSnake + 1.5 * dt * currentSpeed / 100., 1);//(canvasSnake + 1.5f * dt * currentSpeed / 100.) % 1;//Хвостик у однохвостого должен двигаться быстрее
                multiplSnake = abs(canvasSnake - 0.5f) * 2;
                break;
            case 5:
                canvasSnake = fmodf(canvasSnake + dt * currentSpeed / 100., 1);//(canvasSnake + dt * currentSpeed / 100.) % 1;
                multiplSnake = abs(canvasSnake - 0.5f) * 2;
                break;
            case 6:
                canvasSnake = fmodf(canvasSnake + dt * currentSpeed / 100., 1);;//(canvasSnake + dt * currentSpeed / 100.) % 1;
                multiplSnake = abs(canvasSnake - 0.5f) * 2;
                break;
            case 7://Переход
                canvasSize = fmodf(canvasSize + dt * 0.8, 1);//(canvasSize + dt * 0.8) % 1;
                //canvasSnake = (canvasSnake + dt * currentSpeed / 100f) % 1;
                multiplSize=abs(canvasSize-0.5f)*2*0.3f+0.7f;
                multiplSnake=(abs(canvasSize-0.5f)*(-2)*0.3f+0.7f)*40;//вместо мулт сайз 2
                break;
            case 8://Переход
                canvasSize = fmodf(canvasSize + dt * 0.8, 1);//(canvasSize + dt * 0.8) % 1;
                //canvasSnake = (canvasSnake + dt * currentSpeed / 100f) % 1;
                multiplSize=abs(canvasSize-0.5f)*2*0.3f+0.7f;
                multiplSnake=(abs(canvasSize-0.5f)*(-2)*0.3f+0.7f)*33;//вместо мулт сайз 2
                break;
            default:
                qDebug()<<"Food::updateMapPosition"<<"Тип еды задан неправильно. Обновление размеров"<<type;
                //Log.wtf(LOG_TAG,"Тип еды задан неправильно. Обновление размеров "+type);
                break;
        }
    }
}

void Food::draw(Widget& widget){
    if (isEaten_) return;

    //if (abs(currentX-camera.getCurrentX())<camera.getGamefieldHalfX()&&abs(currentY-camera.getCurrentY())<camera.getGamefieldHalfY())

    widget.setColor(1, 1, 1, 0.47);//Standart white
    switch (type){
        case 0:
            widget.drawEllipse(currentX, currentY,1+0.2f*multiplSize, orientation * 180 / (float) M_PI);
            widget.drawLowpolyRound(currentX, currentY,3.5f);
            widget.drawLowpolyRoundTransfered(currentX, currentY,3.5f,orientation * 180 / (float) M_PI,8.4f,0);
            widget.drawLowpolyRoundTransfered(currentX, currentY,3.5f,orientation * 180 / (float) M_PI,-8.4f,0);
            widget.drawBezier(currentX, currentY,0.7f*1.5f,orientation * 180 / (float) M_PI, multiplSnake);
            break;
        case 1:
            widget.drawPlus(currentX, currentY, 1.5f, orientation * 180 / (float) M_PI);
            widget.drawPentagon(currentX, currentY, multiplSize, orientation * 180 / (float) M_PI);
            break;
        case 2:
            widget.drawHalfRings(currentX, currentY,9.35f,orientation * 180 / (float) M_PI,multiplSize * 25 + 2);
            widget.drawBezier3(currentX, currentY,1.05f,orientation * 180 / (float) M_PI, multiplSnake);
            break;
        case 3:
            widget.drawLowpolyRound(currentX, currentY,3.5f);
            widget.drawRing(currentX, currentY,9.35f);
            widget.drawBezier2(currentX, currentY,1.05f,orientation * 180 / (float) M_PI, multiplSnake);
            break;
        case 4:
            widget.drawLowpolyRound(currentX, currentY,3.5f);
            widget.drawRing(currentX, currentY,9.35f);
            widget.drawBezier3(currentX, currentY,1.05f,orientation * 180 / (float) M_PI, multiplSnake);
            break;
        case 5:
            widget.drawTriangle(currentX, currentY,13,orientation * 180 / (float) M_PI);
            widget.drawLowpolyRound(currentX, currentY,3.5f);
            widget.drawBezier4(currentX, currentY,1.05f,orientation * 180 / (float) M_PI, multiplSnake);
            break;
        case 6:
            widget.drawEllipse(currentX, currentY,1.09f,orientation * 180 / (float) M_PI+90);
            widget.drawLowpolyRoundTransfered(currentX, currentY,4,orientation * 180 / (float) M_PI,0,6.2f);
            widget.drawLowpolyRoundTransfered(currentX, currentY,4,orientation * 180 / (float) M_PI,0,-6.2f);
            widget.drawBezier4(currentX, currentY,1.05f,orientation * 180 / (float) M_PI, multiplSnake);
            break;
        case 7://Переход вниз
            widget.setColor(0, 1, 1, 1);//Cyan
            widget.drawLowpolyRound(currentX, currentY,5);
            widget.setColor(1, 1, 1, 0.47);//Standart white
            widget.drawRing3(currentX,currentY,28*multiplSize);
            widget.drawRoundedTriangleInCenterTransfered(currentX,currentY,multiplSnake,45+orientation * 180 / (float) M_PI);
            widget.drawRoundedTriangleInCenterTransfered(currentX,currentY,multiplSnake,45+90+orientation * 180 / (float) M_PI);
            widget.drawRoundedTriangleInCenterTransfered(currentX,currentY,multiplSnake,45+180+orientation * 180 / (float) M_PI);
            widget.drawRoundedTriangleInCenterTransfered(currentX,currentY,multiplSnake,45+270+orientation * 180 / (float) M_PI);

            break;
        case 8://Переход наверх
            widget.setColor(1, 0, 0, 1);//Red
            widget.drawLowpolyRound(currentX, currentY,3);
            widget.setColor(1, 1, 1, 0.47);//Standart white
            widget.drawRing3(currentX,currentY,28*multiplSize);
            widget.drawRoundedTriangleOutCenterTransfered(currentX,currentY,multiplSnake,45+orientation * 180 / (float) M_PI);
            widget.drawRoundedTriangleOutCenterTransfered(currentX,currentY,multiplSnake,45+90+orientation * 180 / (float) M_PI);
            widget.drawRoundedTriangleOutCenterTransfered(currentX,currentY,multiplSnake,45+180+orientation * 180 / (float) M_PI);
            widget.drawRoundedTriangleOutCenterTransfered(currentX,currentY,multiplSnake,45+270+orientation * 180 / (float) M_PI);

            break;
        default:
            qDebug()<<"Food::draw"<<"Тип еды задан неправильно. Обновление картинки";
            //Log.wtf(LOG_TAG,"Тип еды задан неправильно. Обновление картинки");
            break;
    }

}

void Food::updateMapPositionTest(float dt){
    //Только обновление для канвы - изменение размера. Без перемещения
    switch (type) {
        case 0:
            canvasSize = fmodf(canvasSize + dt * 0.8, 1);//canvasSize = (canvasSize + dt * 0.8) % 1;
            canvasSnake = fmodf(canvasSnake + dt * currentSpeed / 100., 1);//(canvasSnake + dt * currentSpeed / 100f) % 1;
            multiplSize = abs(canvasSize - 0.5f) * 2;
            multiplSnake = abs(canvasSnake - 0.5f) * 2;
            break;
        case 1:
            canvasSize = fmodf(canvasSize + dt * 0.8, 1);//(canvasSize + dt * 0.8) % 1;
            multiplSize = (abs(canvasSize - 0.5f) * 2 * 0.3f + 0.7f)*20;
            break;
        case 2:
            canvasSize = fmodf(canvasSize + dt * 0.8, 1);//(canvasSize + dt * 0.8) % 1;
            canvasSnake = fmodf(canvasSnake + 1.8 * dt * currentSpeed / 100., 1);;//(canvasSnake + 1.8f * dt * currentSpeed / 100.) % 1;//Хвостик у головастика должен двигаться быстрее
            multiplSize = 1 - (float) pow(abs(abs(canvasSize - 0.5f) * 2 - 0.5) * 2, 2);//Вся эта конструкция для увеличения частоты вдвое и быстрого захлопывания пасти
            multiplSnake = abs(canvasSnake - 0.5f) * 2;
            break;
        case 3:
            canvasSnake = fmodf(canvasSnake + dt * currentSpeed / 100., 1);//(canvasSnake + dt * currentSpeed / 100.) % 1;
            multiplSnake = abs(canvasSnake - 0.5f) * 2;
            break;
        case 4:
            canvasSnake = fmodf(canvasSnake + 1.5 * dt * currentSpeed / 100., 1);//(canvasSnake + 1.5f * dt * currentSpeed / 100.) % 1;//Хвостик у однохвостого должен двигаться быстрее
            multiplSnake = abs(canvasSnake - 0.5f) * 2;
            break;
        case 5:
            canvasSnake = fmodf(canvasSnake + dt * currentSpeed / 100., 1);//(canvasSnake + dt * currentSpeed / 100.) % 1;
            multiplSnake = abs(canvasSnake - 0.5f) * 2;
            break;
        case 6:
            canvasSnake = fmodf(canvasSnake + dt * currentSpeed / 100., 1);;//(canvasSnake + dt * currentSpeed / 100.) % 1;
            multiplSnake = abs(canvasSnake - 0.5f) * 2;
            break;
        case 7://Переход
            canvasSize = fmodf(canvasSize + dt * 0.8, 1);//(canvasSize + dt * 0.8) % 1;
            //canvasSnake = (canvasSnake + dt * currentSpeed / 100f) % 1;
            multiplSize=abs(canvasSize-0.5f)*2*0.3f+0.7f;
            multiplSnake=(abs(canvasSize-0.5f)*(-2)*0.3f+0.7f)*40;//вместо мулт сайз 2
            break;
        case 8://Переход
            canvasSize = fmodf(canvasSize + dt * 0.8, 1);//(canvasSize + dt * 0.8) % 1;
            //canvasSnake = (canvasSnake + dt * currentSpeed / 100f) % 1;
            multiplSize=abs(canvasSize-0.5f)*2*0.3f+0.7f;
            multiplSnake=(abs(canvasSize-0.5f)*(-2)*0.3f+0.7f)*33;//вместо мулт сайз 2
            break;
        default:
            qDebug()<<"Food::updateMapPosition"<<"Тип еды задан неправильно. Обновление размеров"<<type;
            //Log.wtf(LOG_TAG,"Тип еды задан неправильно. Обновление размеров "+type);
            break;
    }
}

Food::Food(int testType){
    currentX=testType*50-300;// (float) (random()-0.5)*2*1000;
    currentY=400;// (float) (random()-0.5)*2*1000;
    orientation= (float) (M_PI/2);//(float) (random()*PI*2f);
    maxSpeed=45+effolkronium::random_static::get<float>(0, 10);
    currentSpeed = maxSpeed;
    turnSpeed=0;
    canvasSize=effolkronium::random_static::get<float>(0, 1);
    canvasSnake=effolkronium::random_static::get<float>(0, 1);

    type=testType;//this.randomizeType();

    this->setRadius();
    isEaten_=false;
}

bool Food::isEaten(){
    return isEaten_;
}

float Food::getCurrentX(){
    return currentX;
}

float Food::getCurrentY(){
    return currentY;
}

float Food::getCurrentRadius(){
    return currentRadius;
}

void Food::setEaten(){
    isEaten_ = true;
}

bool Food::isEatenAndNotInvisible(){
    return isEaten_&&!isInvisible;
}

void Food::setInvisible(float timer,float segCurrentX, float segCurrentY){
    invisibleTimer=-timer;
    currentX=segCurrentX;
    currentY=segCurrentY;
    isInvisible=true;
    this->goToRandomLocation();
    currentSpeed=0;
    type = effolkronium::random_static::get<int>(0,6);//this->randomizeType();

    isEaten_=false;
}

void Food::setOrientationAim(float orientationAim_){
    orientationAim=orientationAim_;
}

void Food::updateMapPositionBird(float dt, bool isFlockieBirdInPanic,float curFlockieBirdX, float curFlockieBirdY){
    if (isEaten_) return;

    if (!isFlockieBirdInPanic){
        if (sqrt(pow((currentX - curFlockieBirdX), 2) + pow((currentY - curFlockieBirdY), 2)) > 300) {
            aimX = curFlockieBirdX + effolkronium::random_static::get<float>(-flockieBirdRadius, flockieBirdRadius);
            aimY = curFlockieBirdY + effolkronium::random_static::get<float>(-flockieBirdRadius, flockieBirdRadius);
            /*aimX = (float) (random()-0.5)*2*flockieBirdRadius+curFlockieBirdX;
            aimY = (float) (random()-0.5)*2*flockieBirdRadius+curFlockieBirdY;*/
        }//Если улетел далеко от стайки
        else if((abs(aimX - currentX) < 5) && (abs(aimY - currentY) < 5)){
            aimX = curFlockieBirdX + effolkronium::random_static::get<float>(-flockieBirdRadius, flockieBirdRadius);
            aimY = curFlockieBirdY + effolkronium::random_static::get<float>(-flockieBirdRadius, flockieBirdRadius);
            /*aimX = (float) (random()-0.5)*2*flockieBirdRadius+curFlockieBirdX;
            aimY = (float) (random()-0.5)*2*flockieBirdRadius+curFlockieBirdY;*/
        }//Если достиг текущей цели
        orientationAim = (float) atan2(aimY - currentY, aimX - currentX);
    }

    float orientationDelta = remainderf(orientationAim - orientation, 2 * M_PI);

    if (abs(orientationDelta) > turnSpeed * dt) {//Если изменение угла не слишком маленькое
        if (orientationDelta>0){//if ((orientationDelta <= -PI) || ((orientationDelta > 0) && (orientationDelta <= PI))) {
            orientation = orientation + turnSpeed * dt;
        } else {
            orientation = orientation - turnSpeed * dt;
        }
    } else {
        orientation = orientationAim;
    }

    //Скорость


    currentSpeed = std::min(maxSpeed, currentSpeed + dt * 50);



    //Позиция
    currentX = currentX + currentSpeed * (float) cos(orientation) * dt;
    currentY = currentY + currentSpeed * (float) sin(orientation) * dt;

    switch (type) {
        case 0:
            canvasSize = fmodf(canvasSize + dt * 0.8f, 1);
            canvasSnake = fmodf(canvasSnake + dt * currentSpeed / 100., 1);
            multiplSize = abs(canvasSize - 0.5f) * 2;
            multiplSnake = abs(canvasSnake - 0.5f) * 2;
            break;
        case 1:
            canvasSize = fmodf(canvasSize + dt * 0.8f, 1);
            multiplSize = (abs(canvasSize - 0.5f) * 2 * 0.3f + 0.7f)*20;
            break;
        case 2:
            canvasSize = fmodf(canvasSize + dt * 0.8f, 1);
            canvasSnake = fmodf(canvasSnake + 1.8f * dt * currentSpeed / 100., 1);//Хвостик у головастика должен двигаться быстрее
            multiplSize = 1 - (float) pow(abs(abs(canvasSize - 0.5f) * 2 - 0.5f) * 2, 2);//Вся эта конструкция для увеличения частоты вдвое и быстрого захлопывания пасти
            multiplSnake = abs(canvasSnake - 0.5f) * 2;
            break;
        case 3:
            canvasSnake = fmodf(canvasSnake + dt * currentSpeed / 100., 1);
            multiplSnake = abs(canvasSnake - 0.5f) * 2;
            break;
        case 4:
            canvasSnake = fmodf(canvasSnake + 1.5f * dt * currentSpeed / 100., 1);//Хвостик у однохвостого должен двигаться быстрее
            multiplSnake = abs(canvasSnake - 0.5f) * 2;
            break;
        case 5:
            canvasSnake = fmodf(canvasSnake + dt * currentSpeed / 100., 1);
            multiplSnake = abs(canvasSnake - 0.5f) * 2;
            break;
        case 6:
            canvasSnake = fmodf(canvasSnake + dt * currentSpeed / 100., 1);
            multiplSnake = abs(canvasSnake - 0.5f) * 2;
            break;
        case 7://Переход
            canvasSize = fmodf(canvasSize + dt * 0.8f, 1);
            //canvasSnake = (canvasSnake + dt * currentSpeed / 100f) % 1;
            multiplSize=abs(canvasSize-0.5f)*2*0.3f+0.7f;
            multiplSnake=(abs(canvasSize-0.5f)*(-2)*0.3f+0.7f)*40;//вместо мулт сайз 2
            break;
        case 8://Переход
            canvasSize = fmodf(canvasSize + dt * 0.8f, 1);
            //canvasSnake = (canvasSnake + dt * currentSpeed / 100f) % 1;
            multiplSize=abs(canvasSize-0.5f)*2*0.3f+0.7f;
            multiplSnake=(abs(canvasSize-0.5f)*(-2)*0.3f+0.7f)*33;//вместо мулт сайз 2
            break;
        default:
            qDebug()<<"Food::updateMapPositionBird"<<"Тип еды задан неправильно. Обновление размеров"<<type;
            //Log.wtf(LOG_TAG,"Тип еды задан неправильно. Обновление размеров "+type);
            break;
    }
}
