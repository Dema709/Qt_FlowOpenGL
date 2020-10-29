#ifndef FLOCKIEBIRD_H
#define FLOCKIEBIRD_H

#include <vector>

class Food;
class Widget;

class FlockieBird
{
public:
    FlockieBird(int Nbirds_);
    void updateMapPosition(float dt);
    void draw(Widget& widget);

    bool areEaten();
    int getNbirds();
    bool isEaten(int N_);
    float getCurrentX(int N_);
    float getCurrentY(int N_);
    float getCurrentRadius(int N_);
    void setDamaged(int nBird);
private:
    float currentX, currentY, speed = 30;
    float aimX, aimY;
    std::vector<Food> foods_array;
    int Nbirds, NbirdsNow;
    bool areEaten_=false;//Съедены ли все?
    bool isPanic=false;//Паника для всех
    float panicCenterX, panicCenterY;

    float panicTimer = 0, panicMaxTime = 2;

    float maxRange=1000;

    void goToRandomLocation();
};

#endif // FLOCKIEBIRD_H
