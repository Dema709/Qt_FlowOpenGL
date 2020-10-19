#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>
#include <QRandomGenerator>
#include <random>

class Particle
{
public:
    Particle(int);
    int getCount();
    float getX(int);
    float getY(int);
    float getLifetime(int);
    float getMaxLifeTime();
    void updatePosition(float);
private:
    int count;
    std::vector<std::vector<float>> pos_n_lifetime;//pos_x, pox_y, lifetime
    float maxLifeTime = 10;
    float maxRange = 100;
    //QRandomGenerator randomGenerator;
    //std::mt19937 gen;
    //std::uniform_real_distribution<float> uid;
};

#endif // PARTICLE_H
