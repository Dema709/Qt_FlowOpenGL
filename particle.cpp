#include "particle.h"

#include <QDebug>//Временно

Particle::Particle(int count_) : count(count_)//, randomGenerator(QRandomGenerator::securelySeeded())
{
    //QRandomGenerator randomGenerator = QRandomGenerator::securelySeeded();
    pos_n_lifetime.reserve(count);
    for (int i=0; i<count; i++){
        //generateDouble()
        //Generates one random qreal in the canonical range [0, 1) (that is, inclusive of zero and exclusive of 1).
             //return QColor::fromRgb(QRandomGenerator::global()->generate());
        float x = (QRandomGenerator::global()->generateDouble()*2-1)*maxRange;
        float y = (QRandomGenerator::global()->generateDouble()*2-1)*maxRange;
        /*float x = (randomGenerator.generateDouble()*2-1)*maxRange;
        float y = (randomGenerator.generateDouble()*2-1)*maxRange;*/
        pos_n_lifetime.push_back({x,y,static_cast<float>(i)+1});
    }

    for (int i=0; i<count; i++){
        //qDebug()<<pos_n_lifetime[i][0]<<pos_n_lifetime[i][1]<<pos_n_lifetime[i][2];
    }
    /*
    std::random_device rd;
    std::mt19937 gen(rd());//Объявление рандомного генератора другим генератором
    std::uniform_real_distribution<float> uid(-maxRange,maxRange);

    pos_n_lifetime.reserve(count);
    for (int i=0; i<count; i++){
        pos_n_lifetime.push_back({uid(gen),uid(gen),static_cast<float>(i)+1});
    }

    for (int i=0; i<count; i++){
        qDebug()<<pos_n_lifetime[i][0]<<pos_n_lifetime[i][1]<<pos_n_lifetime[i][2];
    }*/
}

int Particle::getCount(){
    return count;
}
float Particle::getX(int pos){
    return pos_n_lifetime[pos][0];
};
float Particle::getY(int pos){
    return pos_n_lifetime[pos][1];
};
float Particle::getLifetime(int pos){
    return pos_n_lifetime[pos][2];
};

float Particle::getMaxLifeTime(){
    return maxLifeTime;
}

void Particle::updatePosition(float dt){
    /*std::random_device rd;
    std::mt19937 gen(rd());//Объявление рандомного генератора другим генератором
    std::uniform_real_distribution<float> uid(-maxRange,maxRange);

    for (int i=0;i<count;i++){
        pos_n_lifetime[i][2] += dt;
        if (pos_n_lifetime[i][2]>maxLifeTime){
            pos_n_lifetime[i][2] -= maxLifeTime;
            pos_n_lifetime[i][0] = uid(gen);//protagonist.getCurrentX()+((float)Math.random()*2-1)*maxRange;
            pos_n_lifetime[i][1] = uid(gen);//protagonist.getCurrentY()+((float)Math.random()*2-1)*maxRange;
        }

    }*/

    for (int i=0;i<count;i++){
        pos_n_lifetime[i][2] += dt;
        if (pos_n_lifetime[i][2]>maxLifeTime){
            pos_n_lifetime[i][2] -= maxLifeTime;
            float x = (QRandomGenerator::global()->generateDouble()*2-1)*maxRange;
            float y = (QRandomGenerator::global()->generateDouble()*2-1)*maxRange;
            pos_n_lifetime[i][0] = x;//protagonist.getCurrentX()+((float)Math.random()*2-1)*maxRange;
            pos_n_lifetime[i][1] = y;//protagonist.getCurrentY()+((float)Math.random()*2-1)*maxRange;
        }

    }

}
