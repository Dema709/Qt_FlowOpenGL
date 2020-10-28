#ifndef SEGMENT_H
#define SEGMENT_H

class Widget;

class Segment
{
public:
    Segment();
    Segment(float headCurrentX, float headCurrentY, float headOrientation, int type_);//Сегмент змейки
    void updateMapPosition(float headCurrentX, float headCurrentY, float dt, float headCurrentSpeed);
    void drawWithScale(Widget& widget, float segNum, float NsegmMax);
    void drawDivision(Widget& widget, float dt);
    float getOrientationInDegrees();
    void setFirst();
    void setWeakPoint();
    float getCurrentX();
    float getCurrentY();
    float getOrientation();
    bool isWeakPoint();
    bool isWeakPointDamaged();
    void restoreWeakPoint();
    bool getSaturation();
    void setSaturation(bool saturation_);
    int getType();
    void changeType(int type_);
    bool isSegmentWeakPointAndUndamaged();
    float getCurrentRadius();
    void setWeakPointDamaged();
private:
    int type;
    float distance;
    float currentX,currentY;
    float orientation;
    bool saturation;
    float canvasSnake=0;//Махать лапками
    float currentRadius=20;
    bool isWeakPoint_ = false, isWeakPointDamaged_ = false;
    float howMuchIsTheFish=1;
    bool isFirst=false;

    void setDistance();
};

#endif // SEGMENT_H
