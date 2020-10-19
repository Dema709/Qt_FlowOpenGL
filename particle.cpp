#include "particle.h"
#include "random.hpp"

#include <QDebug>//Временно

Particle::Particle(int count_, float maxLifeTime_, float maxRange_)
    : count(count_), maxLifeTime(maxLifeTime_), maxRange(maxRange_)
{
    pos_n_lifetime.reserve(count);
    for (int i=0; i<count; i++){
        pos_n_lifetime.push_back({
                                     effolkronium::random_static::get(-maxRange,maxRange),//x
                                     effolkronium::random_static::get(-maxRange,maxRange),//y
                                     i*maxLifeTime/count//static_cast<float>(i)+1//lifetime
                                 });
    }

    if (false)//Вывод сгенерированного массива для положения частиц и их времени жизни
    for (int i=0; i<count; i++){
        qDebug()<<pos_n_lifetime[i][0]<<pos_n_lifetime[i][1]<<pos_n_lifetime[i][2];
    }
}

int Particle::getCount(){
    return count;
}
float Particle::getX(int pos){
    return pos_n_lifetime[pos][0];
}
float Particle::getY(int pos){
    return pos_n_lifetime[pos][1];
}
float Particle::getLifetime(int pos){
    return pos_n_lifetime[pos][2];
}

float Particle::getMaxLifeTime(){
    return maxLifeTime;
}

void Particle::updatePosition(float dt){
    for (int i=0;i<count;i++){
        pos_n_lifetime[i][2] += dt;
        if (pos_n_lifetime[i][2]>maxLifeTime){//Если текущее время жизни больше максимально отведённого
            pos_n_lifetime[i][2] -= maxLifeTime;
            pos_n_lifetime[i][0] = effolkronium::random_static::get(-maxRange,maxRange);
            pos_n_lifetime[i][1] = effolkronium::random_static::get(-maxRange,maxRange);
            //Добавить сюда зависимость от положения гг
        }

    }

}
