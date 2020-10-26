#include "Protagonist.h"
#include "Camera.h"

Protagonist::Protagonist()
{

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

void Protagonist::updateMapPosition(ChakaPon::Camera camera){
    ChakaPon::Camera c;
    c.getCurrentX();
    camera.getCurrentX();
}

