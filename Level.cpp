#include "Level.h"

Level::Level(int levelNum)
{
    int color;

    int changeLevelFood_arraySize;
    int foods_arraySize, boss_arraySize, bossType;
    int snakeHunter_arraySize, snakeHunter_numSegments, snakeHunter_numSegmEvolved;
    int sharkHunter_arraySize, flockieBird_arraySize;

    changeLevelFood_arraySize=2;
    switch (levelNum){
        case (0):
            color=0xFF009EE7;
            //changeLevelFood_arraySize=1;//Переход только на уровень ниже
            break;
        case (1):
            color=0xFF008DD8;
            //foods_arraySize=15;
            break;
        case (2):
            color=0xFF007CC9;
            //foods_arraySize=15;
            //snakeHunter_arraySize=2;                snakeHunter_numSegments=3;                snakeHunter_numSegmEvolved=1;
            break;
        case (3):
            color=0xFF006DBB;
            //foods_arraySize=10;
            //snakeHunter_arraySize=1;                snakeHunter_numSegments=8;                snakeHunter_numSegmEvolved=2;
            break;
        case (4):
            color=0xFF0066AD;
            //foods_arraySize=10;
            //sharkHunter_arraySize=3;//3;//Акула
            break;
        case (5):
            color=0xFF00619E;
            flockieBird_arraySize=1;//                birdType1=0;                Nbirds1=15;
            break;
        case (6)://Boss
            color=0xFF005C8F;
            //foods_arraySize=10;
            //flockieBird_arraySize=2;
            break;
        case (7):
            color=0xFF005780;
            //foods_arraySize=30;
            //flockieBird_arraySize=3;
            //sharkHunter_arraySize=3;//3;//Акула
            //snakeHunter_arraySize=4;                snakeHunter_numSegments=8;                snakeHunter_numSegmEvolved=4;

            //changeLevelFood_arraySize=3;//Переход только на уровень выше. Обработать соответствующе
            //М.б. это ночной уровень, где всё чёрное, а вокруг гг свечение, м.б. жёлтое
            break;
    }

    //backgroundColor = {0, 0x9E/255., 0xE7/255., 1};
    for (int i=0; i<3; i++){
        backgroundColor.push_back(((color>>(2-i)*8)&0xFF) / 255.);
    }
    backgroundColor.push_back(1);//alpha

/*
    switch (changeLevelFood_arraySize){
        case (0):
            changeLevelFood_array = new ChangeLevelFood[0];
            break;
        case (1):
            changeLevelFood_array = new ChangeLevelFood[1];
            changeLevelFood_array[0]=new ChangeLevelFood(0);
            break;
        case (2):
            changeLevelFood_array = new ChangeLevelFood[2];
            changeLevelFood_array[0]=new ChangeLevelFood(0);
            changeLevelFood_array[1]=new ChangeLevelFood(1);
            break;
        case (3):
            changeLevelFood_arraySize=1;
            changeLevelFood_array = new ChangeLevelFood[1];
            changeLevelFood_array[0]=new ChangeLevelFood(1);
            break;
    }

    maxFoodArraySize = Math.min(foods_arraySize + (snakeHunter_arraySize) * snakeHunter_numSegments + sharkHunter_arraySize * 5 + boss_arraySize*12, 30);//30


    foods_array = new Food[maxFoodArraySize];
    for (int j=0;j<maxFoodArraySize;j++){
        foods_array[j] = new Food();
        if (j>=foods_arraySize){
            foods_array[j].setEaten();
        }
    }

    snakeHunter_array = new SnakeHunter[snakeHunter_arraySize];
    for (int j=0;j<snakeHunter_arraySize;j++){
        snakeHunter_array[j] = new SnakeHunter(snakeHunter_numSegments,snakeHunter_numSegmEvolved);
    }

    sharkHunter_array = new SharkHunter[sharkHunter_arraySize];
    for (int j=0;j<sharkHunter_arraySize;j++){
        sharkHunter_array[j] = new SharkHunter();
    }

    Random random = new Random();
    flockieBird_array = new FlockieBird[flockieBird_arraySize];
    for (int j=0;j<flockieBird_arraySize;j++){
        flockieBird_array[j] = new FlockieBird(10,random.nextInt(3));
    }

    boss_array = new Boss[boss_arraySize];
    for (int j=0;j<boss_arraySize;j++){
        boss_array[j] = new Boss(bossType);
    }*/
}

std::vector<float> Level::getColor(){
    return backgroundColor;
}
