#include "SnakeHunter.h"
#include "random.hpp"

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

    //Random random = new Random();
    int currentRandom;
    //int rand=(int)(Math.random() * Nsegm - 1);
    for (int k = 0; k < NsegmEvolved_-1; k++) {
        currentRandom = effolkronium::random_static::get<int>(0, Nsegm - 2);//random.nextInt(Nsegm-1);
        if (!segments[currentRandom].isWeakPoint()) {
            segments[currentRandom].setWeakPoint();
            //segments[currentRandom].changeType(2);
        }
        else{
            for (int j = 1; j < Nsegm-1; j++) {//C 1, т.к. нулевой всегда weakpoint
                if (!segments[j].isWeakPoint()){
                    segments[j].setWeakPoint();
                    //segments[j].changeType(2);
                    break;
                }
            }
        }
    }
}
