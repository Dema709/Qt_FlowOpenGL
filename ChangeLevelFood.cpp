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

void ChangeLevelFood::updateMapPosition(float dt, float cameraX, float cameraY, float half_widht, float half_height){
    lastPingTime += dt;

    if (lastPingTime>pingPeriod+pingDuration){
        lastPingTime -= pingPeriod+pingDuration;
        shouldIPing = false;
    }

    //Если в момент начала пинга клетка вне экрана, она должна начать пинг
    if (!shouldIPing && lastPingTime>=pingPeriod && lastPingTime-dt<=pingPeriod){//Момент начала пинга
        if (abs(currentX-cameraX)>half_widht || abs(currentY-cameraY)>half_height){//Непопадание в экран
            shouldIPing=true;
        }
    }

    //Расчёт положения пинга
    if (shouldIPing){
        //qDebug()<<"Camera"<<(int) cameraX<<(int) cameraY;
        if (abs(currentX-cameraX)<=half_widht && abs(currentY-cameraY)<=half_height){//Попадание в экран
            pingX = currentX; pingY = currentY;
        } else {
            float pingSideCriticalAngle = atan2(half_height, half_widht);
            float alpha = atan2(currentY-cameraY, currentX-cameraX);
            if ((abs(alpha)<pingSideCriticalAngle) || (M_PI-abs(alpha)<pingSideCriticalAngle)){
                //qDebug()<<"Stenka";
                //Боковая стенка
                if (currentX > cameraX) {//Правая стенка
                    pingX = cameraX + half_widht;
                    pingY = cameraY + half_height * tan(alpha);
                    /*deltaX=camera.getCurrentX()+screenSizeX/2;
                    deltaY=camera.getCurrentY() + screenSizeX/2*(float)Math.tan(alpha);*/
                }
                else {
                    pingX = cameraX - half_widht;
                    pingY = cameraY - half_height * tan(alpha);
                    /*deltaX=camera.getCurrentX()-screenSizeX/2;
                    deltaY=camera.getCurrentY() - screenSizeX/2*(float)Math.tan(alpha);*/
                }
            } else {
                //qDebug()<<"Verh/niz";
                //Верх или низ
                if (currentY > cameraY) {//Низ
                    pingX = cameraX + half_widht / tan(alpha);
                    pingY = cameraY + half_height;
                    /*deltaX=camera.getCurrentX() + screenSizeY/2/(float)Math.tan(alpha);
                    deltaY=camera.getCurrentY()+screenSizeY/2;*/
                }
                else {//Верх
                    pingX = cameraX - half_widht / tan(alpha);
                    pingY = cameraY - half_height;
                    /*deltaX=camera.getCurrentX() - screenSizeY/2/(float)Math.tan(alpha);
                    deltaY=camera.getCurrentY()-screenSizeY/2;*/
                }
            }
        }
    }

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
    switch (type){
        case 0://Переход вниз
            widget.setColor(0, 1, 1, 1);//Cyan
            widget.drawLowpolyRound(currentX, currentY,5);
            widget.setColor(1, 1, 1, 0.47);//Standart white
            widget.drawRing3(currentX,currentY,28*multiplSize);

            widget.drawRoundedTriangleInCenterTransfered(currentX,currentY,multiplSize2,45+orientation * 180 / (float) M_PI);
            widget.drawRoundedTriangleInCenterTransfered(currentX,currentY,multiplSize2,45+90+orientation * 180 / (float) M_PI);
            widget.drawRoundedTriangleInCenterTransfered(currentX,currentY,multiplSize2,45+180+orientation * 180 / (float) M_PI);
            widget.drawRoundedTriangleInCenterTransfered(currentX,currentY,multiplSize2,45+270+orientation * 180 / (float) M_PI);
            widget.drawBezier5(currentX,currentY,1.5f,orientation * 180 / (float) M_PI,abs(canvasSnake-0.5f)*2,21*multiplSize-10);
            break;
        case 1://Переход наверх
            widget.setColor(1, 0, 0, 1);//Red
            widget.drawLowpolyRound(currentX, currentY,3);
            widget.setColor(1, 1, 1, 0.47);//Standart white
            widget.drawRing3(currentX,currentY,28*multiplSize);
            widget.drawRoundedTriangleOutCenterTransfered(currentX,currentY,multiplSize2,45+orientation * 180 / (float) M_PI);
            widget.drawRoundedTriangleOutCenterTransfered(currentX,currentY,multiplSize2,45+90+orientation * 180 / (float) M_PI);
            widget.drawRoundedTriangleOutCenterTransfered(currentX,currentY,multiplSize2,45+180+orientation * 180 / (float) M_PI);
            widget.drawRoundedTriangleOutCenterTransfered(currentX,currentY,multiplSize2,45+270+orientation * 180 / (float) M_PI);
            widget.drawBezier5(currentX,currentY,1.5f,orientation * 180 / (float) M_PI,abs(canvasSnake-0.5f)*2,21*multiplSize-10);
            break;
        default:
            qDebug()<<"ChangeLevelFood::draw"<<"Тип еды задан неправильно. Обновление картинки"<<type;
            break;
    }

    if (shouldIPing){
        //qDebug()<<"Ping"<<pingX<<pingY;
        if (type==0){
            widget.setColor(0, 1, 1, (1-(lastPingTime - pingPeriod) / pingDuration));//Cyan alpha
        }
        else{
            widget.setColor(1, 0, 0, (1-(lastPingTime - pingPeriod) / pingDuration));//Red alpha
        }
        widget.drawRing3(pingX, pingY, (lastPingTime - pingPeriod) / pingDuration * 40);
        //openGLRenderer.drawRing3(deltaX,deltaY,(lastPingTime - pingPeriod) / pingDuration * 40);
    }
}
