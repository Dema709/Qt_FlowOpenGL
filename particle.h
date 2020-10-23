#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>
class Widget;

class Particle
{
public:
    Particle(int count, float maxLifeTime=10, float maxRange=500);
    int getCount();
    float getX(int);
    float getY(int);
    float getLifetime(int);
    float getMaxLifeTime();
    void updatePosition(float dt);
    void draw(Widget& widget);
private:
    int count;
    std::vector<std::vector<float>> pos_n_lifetime;//pos_x, pox_y, lifetime
    float maxLifeTime;//Секунд
    float maxRange;
};

#endif // PARTICLE_H
