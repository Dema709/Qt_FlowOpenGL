#include "VertexLoader.h"

#include <QDebug>//Временное включение для отладки

VertexLoader::VertexLoader(int ANIMATION_FRAMES_)// : ANIMATION_FRAMES(ANIMATION_FRAMES_)
{
    V.ANIMATION_FRAMES = ANIMATION_FRAMES_;
    int ANIMATION_FRAMES = ANIMATION_FRAMES_;//Чтобы не менять конструкторы-заполнители с вершин

    {
        //Оси
        std::vector<GLfloat> current_vertices = {
            // ось X
            -500, 0,
             500, 0,
            // ось Y
            0, -500,
            0,  500,
        };
        V.ASIX = {vertices.size()/2, current_vertices.size()/2,GL_LINES};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        //Квадрат
        std::vector<GLfloat> current_vertices = {
            -0.5f, -0.5f,
            -0.5f, 0.5f,
             0.5f, 0.5f,
             0.5f, -0.5f,
        };
        V.SQUARE = {vertices.size()/2, current_vertices.size()/2,GL_TRIANGLE_FAN};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        //Частица с радиусом 5, состоит их трёх окружностей
        std::vector<GLfloat> current_vertices;
        int VERTEX_COUNT = 16;//Число вершин одной окружности
        float radius = 5;
        for (int j = 0; j < 3; j++) {
            float percent, rad;
            for (int i = 0; i < VERTEX_COUNT; ++i) {
                current_vertices.push_back(0);
                current_vertices.push_back(0);

                percent = static_cast<float>(i) / VERTEX_COUNT;
                rad = percent * 2 * M_PI;
                current_vertices.push_back(radius * (j + 1) / 3. * cos(rad));
                current_vertices.push_back(radius * (j + 1) / 3. * sin(rad));

                percent = static_cast<float>(i+1) / (float) (VERTEX_COUNT);
                rad = percent * 2 * M_PI;
                current_vertices.push_back(radius * (j + 1) / 3. * cos(rad));
                current_vertices.push_back(radius * (j + 1) / 3. * sin(rad));
            }
        }
        V.PARTICLE = {vertices.size()/2, current_vertices.size()/2, GL_TRIANGLES};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        //Эллипс [(24 на 11 с толщиной 2)*0.7]
        std::vector<GLfloat> current_vertices;
        float radius1 = 24*0.7, radius2 = 11*0.7, ringwidth = 2*0.7/2;
        int VERTEX_COUNT = 16;//Число внешних (внутренних) вершин эллипса
        for (int i=0; i<VERTEX_COUNT*2+2; i++){
            float percent = static_cast<float>(i) / VERTEX_COUNT / 2.;
            float rad = percent * 2 * M_PI;
            if (i%2==0){
                //Чётные числа для внешего радиуса
                current_vertices.push_back((radius1 + ringwidth) * cos(rad));
                current_vertices.push_back((radius2 + ringwidth) * sin(rad));
            } else {
                current_vertices.push_back((radius1 - ringwidth) * cos(rad));
                current_vertices.push_back((radius2 - ringwidth) * sin(rad));
            }
        }

        V.ELLIPSE = {vertices.size()/2, current_vertices.size()/2, GL_TRIANGLE_STRIP};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        //Заполненный круг радиуса 1
        std::vector<GLfloat> current_vertices;
        int VERTEX_COUNT = 16;
        current_vertices.push_back(0);  current_vertices.push_back(0);
        for (int i=0; i<VERTEX_COUNT+1; i++){
            float percent = static_cast<float>(i) / VERTEX_COUNT;
            float rad = percent * 2 * M_PI;
            current_vertices.push_back(cos(rad));
            current_vertices.push_back(sin(rad));
        }
        V.ROUND = {vertices.size()/2, current_vertices.size()/2, GL_TRIANGLE_FAN};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        //LowPoly круг радиуса 1
        std::vector<GLfloat> current_vertices;
        int VERTEX_COUNT = 8;
        current_vertices.push_back(0);  current_vertices.push_back(0);
        for (int i=0; i<VERTEX_COUNT+1; i++){
            float percent = static_cast<float>(i) / VERTEX_COUNT;
            float rad = percent * 2 * M_PI;
            current_vertices.push_back(cos(rad));
            current_vertices.push_back(sin(rad));
        }
        V.LOWPOLY_ROUND = {vertices.size()/2, current_vertices.size()/2, GL_TRIANGLE_FAN};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        std::vector<GLfloat> current_vertices;
        int VERTEX_COUNT = 16+1;
        float x0, y0, x1, y1, x2=0, y2 = 0; float animationStatus=0;
                x0 = 0; y0 = 11-4.5f; x1 = 0; y1 = 18-4.5f;
                for (int j=0;j<ANIMATION_FRAMES;++j) {
                    animationStatus=(j/(float)(ANIMATION_FRAMES-1));
                    x2 = -15*animationStatus; y2 = 28-10*animationStatus;//-4.5f;
                    int outerVertexCount=VERTEX_COUNT;
                    for (int i=0;i<outerVertexCount;++i){
                        float percent=(i/(float)(outerVertexCount-1));
                        float outer_x, outer_y;
                        if (i%2==0)//Чётные числа для внешего радиуса
                        {
                            outer_x=(x0*(1-percent)*(1-percent)+2*percent*(1-percent)*x1+percent*percent*x2);
                            outer_y=(y0*(1-percent)*(1-percent)+2*percent*(1-percent)*y1+percent*percent*y2);
                        }
                        else{//Дополнительная кривая Безье для толщины
                            outer_x=((x0+3)*(1-percent)*(1-percent)+2*percent*(1-percent)*(x1+2)+percent*percent*(x2+2*animationStatus*animationStatus));
                            outer_y=(y0*(1-percent)*(1-percent)+2*percent*(1-percent)*(y1+2)+percent*percent*(y2+2*(1-animationStatus)*(1-animationStatus)));
                            //Множители вроде animationStatus в квадрате, чтобы окончания крылышек не были прямоугольными
                        }
                        current_vertices.push_back(outer_x);
                        current_vertices.push_back(outer_y);
                    }
                }

        V.BEZIER = {vertices.size()/2, current_vertices.size()/2/ANIMATION_FRAMES, GL_TRIANGLE_STRIP};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        //Пятиугольник с средним радиусом 1 и толщиной стенки 0.2f
        std::vector<GLfloat> current_vertices = {
            (float)(0.9*cos(M_PI*2/5*0)), (float)(0.9*sin(M_PI*2/5*0)),
            (float)(1.1*cos(M_PI*2/5*0)), (float)(1.1*sin(M_PI*2/5*0)),
            (float)(0.9*cos(M_PI*2/5*1)), (float)(0.9*sin(M_PI*2/5*1)),
            (float)(1.1*cos(M_PI*2/5*1)), (float)(1.1*sin(M_PI*2/5*1)),
            (float)(0.9*cos(M_PI*2/5*2)), (float)(0.9*sin(M_PI*2/5*2)),
            (float)(1.1*cos(M_PI*2/5*2)), (float)(1.1*sin(M_PI*2/5*2)),
            (float)(0.9*cos(M_PI*2/5*3)), (float)(0.9*sin(M_PI*2/5*3)),
            (float)(1.1*cos(M_PI*2/5*3)), (float)(1.1*sin(M_PI*2/5*3)),
            (float)(0.9*cos(M_PI*2/5*4)), (float)(0.9*sin(M_PI*2/5*4)),
            (float)(1.1*cos(M_PI*2/5*4)), (float)(1.1*sin(M_PI*2/5*4)),
            (float)(0.9*cos(M_PI*2/5*5)), (float)(0.9*sin(M_PI*2/5*5)),
            (float)(1.1*cos(M_PI*2/5*5)), (float)(1.1*sin(M_PI*2/5*5)),
        };
        V.PENTAGON = {vertices.size()/2, current_vertices.size()/2,GL_TRIANGLE_STRIP};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        //Треугольник в 60 градусов с выгнутой круглой стороной
        std::vector<GLfloat> current_vertices;
        int VERTEX_COUNT = 16+7;
        float radius = 1, ringwidth = 0.1/2;
        float dAngle = 15. / 2 * M_PI / 180;

        current_vertices.push_back((-2*radius+(radius+ringwidth)*cos(30*M_PI/180.-dAngle)));
        current_vertices.push_back(((radius+ringwidth)*sin(30*M_PI/180.-dAngle)));

        current_vertices.push_back((-2*radius+(radius+ringwidth)*cos(30*M_PI/180.)));
        current_vertices.push_back(((radius+ringwidth)*sin(30*M_PI/180.)));

        current_vertices.push_back(-ringwidth*4);
        current_vertices.push_back(0);

        current_vertices.push_back(0);
        current_vertices.push_back(0);

        current_vertices.push_back((-2*radius+(radius+ringwidth)*cos(-30*M_PI/180.+dAngle)));
        current_vertices.push_back(((radius+ringwidth)*sin(-30*M_PI/180.+dAngle)));

        current_vertices.push_back((-2*radius+(radius+ringwidth)*cos(-30*M_PI/180.)));
        current_vertices.push_back(((radius+ringwidth)*sin(-30*M_PI/180.)));

        current_vertices.push_back((-2*radius+(radius+ringwidth)*cos(-30*M_PI/180.)));
        current_vertices.push_back(((radius+ringwidth)*sin(-30*M_PI/180.)));

        int outerVertexCount=VERTEX_COUNT-7;
        float percent, rad, outer_x, outer_y;
        for (int i=0;i<outerVertexCount;++i){
            if (i==outerVertexCount-1) percent=((i-1)/(float)(outerVertexCount-2));
            else percent=(i/(float)(outerVertexCount-2));
            rad=(float)((percent-0.5f)*2*M_PI*60/360);
            //Log.wtf(LOG_TAG,"percent="+percent+" rad="+rad);
            if (i%2!=0){
                outer_x=(float)(-2*radius+(radius+ringwidth)*cos(rad));
                outer_y=(float)((radius+ringwidth)*sin(rad));
            }
            else{
                outer_x=(float)(-2*radius+(radius-ringwidth)*cos(rad));
                outer_y=(float)((radius-ringwidth)*sin(rad));
            }

            current_vertices.push_back(outer_x);
            current_vertices.push_back(outer_y);
        }

        V.ROUNDED_TRIANGLE_INCENTER = {vertices.size()/2, current_vertices.size()/2,GL_TRIANGLE_STRIP};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        //Треугольник в 60 градусов с вогнутой круглой стороной
        std::vector<GLfloat> current_vertices;
        int VERTEX_COUNT = 16+7;
        float radius = 1, ringwidth = 0.1f/2, dAngle = (float)(12./2.*M_PI/180.);

        current_vertices.push_back((radius-ringwidth)*cos(45*M_PI/180.-dAngle));
        current_vertices.push_back((radius-ringwidth)*sin(45*M_PI/180.-dAngle));

        current_vertices.push_back((radius-ringwidth)*cos(45*M_PI/180.));
        current_vertices.push_back((radius-ringwidth)*sin(45*M_PI/180.));

        current_vertices.push_back(ringwidth*3);
        current_vertices.push_back(0);

        current_vertices.push_back(0);
        current_vertices.push_back(0);

        current_vertices.push_back((radius-ringwidth)*cos(-45*M_PI/180.+dAngle));
        current_vertices.push_back((radius-ringwidth)*sin(-45*M_PI/180.+dAngle));

        current_vertices.push_back((radius-ringwidth)*cos(-45*M_PI/180.));
        current_vertices.push_back((radius-ringwidth)*sin(-45*M_PI/180.));

        current_vertices.push_back((radius-ringwidth)*cos(-45*M_PI/180.));
        current_vertices.push_back((radius-ringwidth)*sin(-45*M_PI/180.));

        int outerVertexCount=VERTEX_COUNT-7;
        float percent, rad, outer_x, outer_y;
        for (int i=0;i<outerVertexCount;++i){
            if (i==outerVertexCount-1) percent=((i-1)/(float)(outerVertexCount-2));
            else percent=(i/(float)(outerVertexCount-2));
            rad=(float)((percent-0.5f)*2.*M_PI*90./360.);
            //Log.wtf(LOG_TAG,"percent="+percent+" rad="+rad);
            if (i%2==0){
                outer_x=(float)((radius+ringwidth)*cos(rad));
                outer_y=(float)((radius+ringwidth)*sin(rad));
            }
            else{
                outer_x=(float)((radius-ringwidth)*cos(rad));
                outer_y=(float)((radius-ringwidth)*sin(rad));
            }
            //Log.wtf(LOG_TAG,"alpha "+i+" " +rad/Math.PI*180+" radius="+sqrt(outer_x*outer_x+outer_y*outer_y));
            current_vertices.push_back(outer_x);
            current_vertices.push_back(outer_y);
        }//Верхушка

        V.ROUNDED_TRIANGLE_OUTCENTER = {vertices.size()/2, current_vertices.size()/2,GL_TRIANGLE_STRIP};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        std::vector<GLfloat> current_vertices;
        int VERTEX_COUNT = 16*2+10;//Количество вершин для плюсика *2+10

        float radius = 1; float distToRadius=4;
        int outerVertexCount=(VERTEX_COUNT-10)/2;

        current_vertices.push_back(0);
        current_vertices.push_back(0);
        current_vertices.push_back(radius);
        current_vertices.push_back(radius);

        float percent, rad, outer_x, outer_y;

        for (int i=0;i<outerVertexCount/2+1;++i){
            percent=(i/(float)(outerVertexCount));
            rad=(float)(percent*2*M_PI);
            outer_x=(float)(radius*cos(rad));
            outer_y=distToRadius+(float)(radius*sin(rad));
            current_vertices.push_back(outer_x);
            current_vertices.push_back(outer_y);
        }//Верхушка
        current_vertices.push_back(-radius);current_vertices.push_back(radius);
        for (int i=0;i<outerVertexCount/2+1;++i){
            percent=(i/(float)(outerVertexCount));
            //Log.wtf(LOG_TAG,"angle "+percent*360);
            rad=(float)(percent*2*M_PI);
            outer_x=-distToRadius-(float)(radius*sin(rad));
            outer_y=(float)(radius*cos(rad));
            current_vertices.push_back(outer_x);
            current_vertices.push_back(outer_y);
        }//Левая часть
        current_vertices.push_back(-radius);current_vertices.push_back(-radius);
        for (int i=0;i<outerVertexCount/2+1;++i){
            percent=(i/(float)(outerVertexCount));
            //Log.wtf(LOG_TAG,"angle "+percent*360);
            rad=(float)(percent*2*M_PI);
            outer_x=-(float)(radius*cos(rad));
            outer_y=-distToRadius-(float)(radius*sin(rad));
            current_vertices.push_back(outer_x);
            current_vertices.push_back(outer_y);
        }//Низ
        current_vertices.push_back(radius);current_vertices.push_back(-radius);
        for (int i=0;i<outerVertexCount/2+1;++i){
            percent=(i/(float)(outerVertexCount));
            //Log.wtf(LOG_TAG,"angle "+percent*360);
            rad=(float)(percent*2*M_PI);
            outer_x=distToRadius+(float)(radius*sin(rad));
            outer_y=-(float)(radius*cos(rad));
            current_vertices.push_back(outer_x);
            current_vertices.push_back(outer_y);
        }//Правая часть
        current_vertices.push_back(radius);current_vertices.push_back(radius);

        V.PLUS = {vertices.size()/2, current_vertices.size()/2, GL_TRIANGLE_FAN};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        //Треугольник
        std::vector<GLfloat> current_vertices = {
            (float)((1-0.16)*cos(M_PI*2/3*0)),(float)((1-0.16)*sin(M_PI*2/3*0)),
            (float)((1+0.16)*cos(M_PI*2/3*0)),(float)((1+0.16)*sin(M_PI*2/3*0)),
            (float)((1-0.16)*cos(M_PI*2/3*1)),(float)((1-0.16)*sin(M_PI*2/3*1)),
            (float)((1+0.16)*cos(M_PI*2/3*1)),(float)((1+0.16)*sin(M_PI*2/3*1)),
            (float)((1-0.16)*cos(M_PI*2/3*2)),(float)((1-0.16)*sin(M_PI*2/3*2)),
            (float)((1+0.16)*cos(M_PI*2/3*2)),(float)((1+0.16)*sin(M_PI*2/3*2)),
            (float)((1-0.16)*cos(M_PI*2/3*3)),(float)((1-0.16)*sin(M_PI*2/3*3)),
            (float)((1+0.16)*cos(M_PI*2/3*3)),(float)((1+0.16)*sin(M_PI*2/3*3)),
        };
        V.TRIANGLE = {vertices.size()/2, current_vertices.size()/2,GL_TRIANGLE_STRIP};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        //Кольцо с средним радиусом 1 и толщиной 0,1886792452830189
        std::vector<GLfloat> current_vertices;
        int VERTEX_COUNT = 16*2+2;
        float radius = 1, ringwidth = 0.1886792452830189f/2;
        float percent, rad, outer_x, outer_y;
        for (int i=0;i<VERTEX_COUNT;++i){
            percent=(i/(float)(VERTEX_COUNT-2));
            rad=(float)(percent*2*M_PI);
            if (i%2==0)//Чётные числа для внешего радиуса
            {
                outer_x=(float)((radius+ringwidth)*cos(rad));
                outer_y=(float)((radius+ringwidth)*sin(rad));
            }
            else{
                outer_x=(float)((radius-ringwidth)*cos(rad));
                outer_y=(float)((radius-ringwidth)*sin(rad));
            }
            current_vertices.push_back(outer_x);
            current_vertices.push_back(outer_y);
        }

        V.RING1 = {vertices.size()/2, current_vertices.size()/2,GL_TRIANGLE_STRIP};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        //Кольцо с средним радиусом 1 и толщиной 0,3773584905660377
        std::vector<GLfloat> current_vertices;
        int VERTEX_COUNT = 16*2+2;
        float radius = 1, ringwidth = 0.3773584905660377/2;
        float percent, rad, outer_x, outer_y;
        for (int i=0;i<VERTEX_COUNT;++i){
            percent=(i/(float)(VERTEX_COUNT-2));
            rad=(float)(percent*2*M_PI);
            if (i%2==0)//Чётные числа для внешего радиуса
            {
                outer_x=(float)((radius+ringwidth)*cos(rad));
                outer_y=(float)((radius+ringwidth)*sin(rad));
            }
            else{
                outer_x=(float)((radius-ringwidth)*cos(rad));
                outer_y=(float)((radius-ringwidth)*sin(rad));
            }
            current_vertices.push_back(outer_x);
            current_vertices.push_back(outer_y);
        }

        V.RING2 = {vertices.size()/2, current_vertices.size()/2,GL_TRIANGLE_STRIP};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        //Кольцо с средним радиусом 1 и толщиной 0,1
        std::vector<GLfloat> current_vertices;
        int VERTEX_COUNT = 16*2+2;
        float radius = 1, ringwidth = 0.1/2;
        float percent, rad, outer_x, outer_y;
        for (int i=0;i<VERTEX_COUNT;++i){
            percent=(i/(float)(VERTEX_COUNT-2));
            rad=(float)(percent*2*M_PI);
            if (i%2==0)//Чётные числа для внешего радиуса
            {
                outer_x=(float)((radius+ringwidth)*cos(rad));
                outer_y=(float)((radius+ringwidth)*sin(rad));
            }
            else{
                outer_x=(float)((radius-ringwidth)*cos(rad));
                outer_y=(float)((radius-ringwidth)*sin(rad));
            }
            current_vertices.push_back(outer_x);
            current_vertices.push_back(outer_y);
        }

        V.RING3 = {vertices.size()/2, current_vertices.size()/2,GL_TRIANGLE_STRIP};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        //Полукольцо с средним радиусом 1 и толщиной 0,1886792452830189. Верхняя часть
        std::vector<GLfloat> current_vertices;
        int VERTEX_COUNT = 16;
        float radius = 1, ringwidth = 0.1886792452830189/2;

        current_vertices.push_back(sqrt(pow(radius-ringwidth,2)-pow(2*ringwidth,2))-0.0175f);
        current_vertices.push_back(ringwidth*2);

        current_vertices.push_back(-sqrt(pow(radius-ringwidth,2)-pow(2*ringwidth,2))+0.0175f);
        current_vertices.push_back(ringwidth*2);

        current_vertices.push_back(radius-ringwidth);
        current_vertices.push_back(0);

        current_vertices.push_back(-(radius-ringwidth));
        current_vertices.push_back(0);

        current_vertices.push_back(-radius);
        current_vertices.push_back(0);

        float percent, rad, outer_x, outer_y;
        for (int i=0;i<VERTEX_COUNT+2;i++){
            if (i%2==0)//Чётные числа для внешего радиуса
            {
                percent=(0.5f*i/(float)(VERTEX_COUNT));
                rad=(float)(percent*2*M_PI);
                outer_x=(float)((radius+ringwidth)*cos(rad));
                outer_y=(float)((radius+ringwidth)*sin(rad));
            }
            else{
                outer_x=(float)((radius-ringwidth)*cos(rad));
                outer_y=(float)((radius-ringwidth)*sin(rad));
            }
            current_vertices.push_back(outer_x);
            current_vertices.push_back(outer_y);
        }

        V.HALFRING = {vertices.size()/2, current_vertices.size()/2,GL_TRIANGLE_STRIP};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        //Рот у ГГ - через кривые безье
        std::vector<GLfloat> current_vertices;
        int VERTEX_COUNT = 16;//VERTEX_MOUTH_COUNT = 16+2+1;

        float deltaX=9;
        float animationStatus, percent, x0, y0, x1, y1, x2, y2;
        float outer_x, outer_y;
        for (int j=0;j<ANIMATION_FRAMES;++j) {
            animationStatus=(j/(float)(ANIMATION_FRAMES-1));
            current_vertices.push_back(4+deltaX);
            current_vertices.push_back(0);

            x0 = 0; y0 = 0;
            x1 = 0; y1 = 18*1.4f;//1.4f - масштабный множитель
            x2 = (15+15*animationStatus)*1.4f; y2 = (28-25*animationStatus)*1.4f;
            for (int i=0;i<VERTEX_COUNT+1;++i){//16+1
                percent=(i/(float)VERTEX_COUNT);//16
                if (i%2==0)//Чётные числа для внешего радиуса
                {
                    outer_x=(x0*(1-percent)*(1-percent)+2*percent*(1-percent)*x1+percent*percent*x2);
                    outer_y=(y0*(1-percent)*(1-percent)+2*percent*(1-percent)*y1+percent*percent*y2);
                }
                else{//Дополнительная кривая Безье для толщины
                    outer_x=((x0+3*1.4f)*(1-percent)*(1-percent)+2*percent*(1-percent)*(x1+2+2*animationStatus)+percent*percent*(x2/*+2*animationStatus*animationStatus*/));
                    outer_y=(y0*(1-percent)*(1-percent)+2*percent*(1-percent)*(y1-2)+percent*percent*(y2-3*1.4f/*2*(1-animationStatus)*(1-animationStatus)*/));
                    //Множители вроде animationStatus в квадрате, чтобы окончания крылышек не были прямоугольными
                }
                current_vertices.push_back(outer_x+deltaX);
                current_vertices.push_back(outer_y);
            }
            current_vertices.push_back(x2-3.5f*animationStatus+deltaX);
            current_vertices.push_back(y2-3*1.4f+1.5f*animationStatus);
        }

        V.MOUTH = {vertices.size()/2, current_vertices.size()/2/ANIMATION_FRAMES,GL_TRIANGLE_STRIP};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        //Тело акулки //4 части * 3 точки у треугольника
        std::vector<GLfloat> current_vertices;
        int VERTEX_COUNT = 16;

        std::vector<float> vertexVector(2);
        float deltaX=3;//Толщина линий
        float outerVertexCount = VERTEX_COUNT;//Для быстрого перевода с java, убрать!
        float animationStatus, percent, x0, y0, x1, y1, x2, y2;
        for (int j=0;j<ANIMATION_FRAMES;++j) {
            animationStatus = (j / (float) (ANIMATION_FRAMES - 1));

            x0 = -36; y0 = -499;
            x1 = 560; y1 = -329;//1.4f - масштабный множитель
            x2 = 960; y2 = 0;
            for (int i=0;i<outerVertexCount;++i){
                percent=(i/(float)(outerVertexCount-1));
                //Log.wtf(LOG_TAG,""+percent);
                sharkBodyBezier1(vertexVector, i%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                current_vertices.push_back(vertexVector[0]);current_vertices.push_back(vertexVector[1]);
                if (i==0) {//первый треугольник
                    //percent=(i/(float)(outerVertexCount-1));
                    sharkBodyBezier1(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);current_vertices.push_back(vertexVector[1]);
                }
                else{
                    percent=((i-1)/(float)(outerVertexCount-1));
                    sharkBodyBezier1(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);current_vertices.push_back(vertexVector[1]);
                }
                if (i==outerVertexCount-1){//Последний треугольник
                    percent=(i/(float)(outerVertexCount-1));
                    sharkBodyBezier1(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);current_vertices.push_back(vertexVector[1]);
                }
                else{
                    percent=((i+1)/(float)(outerVertexCount-1));
                    sharkBodyBezier1(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);current_vertices.push_back(vertexVector[1]);
                }
            }

            //Здесь -vertexVector[1]
            for (int i=0;i<outerVertexCount;++i){
                percent=(i/(float)(outerVertexCount-1));
                //Log.wtf(LOG_TAG,""+percent);
                sharkBodyBezier1(vertexVector,i%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                current_vertices.push_back(vertexVector[0]);current_vertices.push_back(-vertexVector[1]);
                if (i==0) {//первый треугольник
                    //percent=(i/(float)(outerVertexCount-1));
                    sharkBodyBezier1(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);current_vertices.push_back(-vertexVector[1]);
                }
                else{
                    percent=((i-1)/(float)(outerVertexCount-1));
                    sharkBodyBezier1(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);current_vertices.push_back(-vertexVector[1]);
                }
                if (i==outerVertexCount-1){//Последний треугольник
                    percent=(i/(float)(outerVertexCount-1));
                    sharkBodyBezier1(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);current_vertices.push_back(-vertexVector[1]);
                }
                else{
                    percent=((i+1)/(float)(outerVertexCount-1));
                    sharkBodyBezier1(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);current_vertices.push_back(-vertexVector[1]);
                }
            }

            x0 = -36; y0 = -499;
            x1 = -302; y1 = -164;
            x2 = -1113 - 410 * animationStatus; y2 = 0;
            for (int i=0;i<outerVertexCount;++i){
                percent=(i/(float)(outerVertexCount-1));
                //Log.wtf(LOG_TAG,""+percent);
                sharkBodyBezier2(vertexVector,i%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                current_vertices.push_back(vertexVector[0]);current_vertices.push_back(vertexVector[1]);
                if (i==0) {//первый треугольник
                    //percent=(i/(float)(outerVertexCount-1));
                    sharkBodyBezier2(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);current_vertices.push_back(vertexVector[1]);
                }
                else{
                    percent=((i-1)/(float)(outerVertexCount-1));
                    sharkBodyBezier2(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);current_vertices.push_back(vertexVector[1]);
                }
                if (i==outerVertexCount-1){//Последний треугольник
                    percent=(i/(float)(outerVertexCount-1));
                    sharkBodyBezier2(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);current_vertices.push_back(vertexVector[1]);
                }
                else{
                    percent=((i+1)/(float)(outerVertexCount-1));
                    sharkBodyBezier2(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);current_vertices.push_back(vertexVector[1]);
                }
            }
            for (int i=0;i<outerVertexCount;++i){
                percent=(i/(float)(outerVertexCount-1));
                //Log.wtf(LOG_TAG,""+percent);
                sharkBodyBezier2(vertexVector,i%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                current_vertices.push_back(vertexVector[0]);current_vertices.push_back(-vertexVector[1]);
                if (i==0) {//первый треугольник
                    //percent=(i/(float)(outerVertexCount-1));
                    sharkBodyBezier2(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);current_vertices.push_back(-vertexVector[1]);
                }
                else{
                    percent=((i-1)/(float)(outerVertexCount-1));
                    sharkBodyBezier2(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);current_vertices.push_back(-vertexVector[1]);
                }
                if (i==outerVertexCount-1){//Последний треугольник
                    percent=(i/(float)(outerVertexCount-1));
                    sharkBodyBezier2(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);current_vertices.push_back(-vertexVector[1]);
                }
                else{
                    percent=((i+1)/(float)(outerVertexCount-1));
                    sharkBodyBezier2(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);current_vertices.push_back(-vertexVector[1]);
                }
            }

        }



        V.SHARKBODY = {vertices.size()/2, current_vertices.size()/2/ANIMATION_FRAMES,GL_TRIANGLES};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        //Три "буквы Л"
        std::vector<GLfloat> current_vertices;
        //int VERTEX_COUNT = 12*3;//VERTEX_SHARKMOUTH_COUNT = 12*3;

        float radius = 103*0.94f;
        float dAngle = 4. / 180. * M_PI;
        float timeyWimeyX, timeyWimeyY;
        float x0, x1, x2, y0, y1, y2;
        for (int i=0;i<3;i++){
            switch (i){
                case 0:
                    x0=-92;y0=-40;x1=-28;y1=7;x2=-92;y2=40;
                    break;
                case 1:
                    x0=92;y0=-40;x1=23;y1=-15;x2=40;y2=-92;
                    break;
                case 2:
                    x0=21.7f;y0=97.9f;x1=16;y1=30;x2=79.7f;y2=60;
                    break;
                default:
                qDebug()<<"ERROR::VertexLoader"<<"Ошибка в цикле для акульего рта";
                    //Log.wtf(LOG_TAG,"Ошибка в цикле для акульего рта");
                    break;
            }
            timeyWimeyX=(x0+x2)*0.5f-x1;
            timeyWimeyY=(y0+y2)*0.5f-y1;
            float deltaX=(float)(sqrt(pow(timeyWimeyX,2)+pow(timeyWimeyY,2)))/10.;//Нормируем к единице и умножаем на некое число (ну, делим, на самом деле) - расст. до точек
            timeyWimeyX=timeyWimeyX/deltaX;
            timeyWimeyY=timeyWimeyY/deltaX;

            current_vertices.push_back(radius*cos(atan2(y0,x0)+dAngle));
            current_vertices.push_back(radius*sin(atan2(y0,x0)+dAngle));

            current_vertices.push_back(radius*cos(atan2(y0,x0)-dAngle));
            current_vertices.push_back(radius*sin(atan2(y0,x0)-dAngle));

            current_vertices.push_back(x1-timeyWimeyX);
            current_vertices.push_back(y1-timeyWimeyY);

            //
            current_vertices.push_back(radius*cos(atan2(y0,x0)-dAngle));
            current_vertices.push_back(radius*sin(atan2(y0,x0)-dAngle));

            current_vertices.push_back(x1-timeyWimeyX);
            current_vertices.push_back(y1-timeyWimeyY);

            current_vertices.push_back(x1+timeyWimeyX);
            current_vertices.push_back(y1+timeyWimeyY);

            //
            current_vertices.push_back(x1-timeyWimeyX);
            current_vertices.push_back(y1-timeyWimeyY);

            current_vertices.push_back(x1+timeyWimeyX);
            current_vertices.push_back(y1+timeyWimeyY);

            current_vertices.push_back(radius*cos(atan2(y2,x2)+dAngle));
            current_vertices.push_back(radius*sin(atan2(y2,x2)+dAngle));

            //
            current_vertices.push_back(x1-timeyWimeyX);
            current_vertices.push_back(y1-timeyWimeyY);

            current_vertices.push_back(radius*cos(atan2(y2,x2)+dAngle));
            current_vertices.push_back(radius*sin(atan2(y2,x2)+dAngle));

            current_vertices.push_back(radius*cos(atan2(y2,x2)-dAngle));
            current_vertices.push_back(radius*sin(atan2(y2,x2)-dAngle));
        }

        V.SHARKMOUTH = {vertices.size()/2, current_vertices.size()/2,GL_TRIANGLES};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        std::vector<GLfloat> current_vertices;
        int VERTEX_COUNT = 16;

        float deltaX=2;//Толщина линий
        std::vector<float> vertexVector(2);
        float x0, y0, x1, y1, x2, y2;
        float percent;
        int outerVertexCount = VERTEX_COUNT;        //Log.wtf(LOG_TAG,""+outerVertexCount);
        for (int j=0;j<ANIMATION_FRAMES;++j) {
            float animationStatus = (j / (float) (ANIMATION_FRAMES - 1));

            x0 = 50; y0 = -297;
            x1 = 202; y1 = -175;//1.4f - масштабный множитель
            x2 = 348; y2 = 0;
            for (int i=0;i<outerVertexCount;++i){
                percent=(i/(float)(outerVertexCount-1));
                //Log.wtf(LOG_TAG,""+percent);
                boss1BodyBezier1(vertexVector,i%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(vertexVector[1]);
                if (i==0) {//первый треугольник
                    //percent=(i/(float)(outerVertexCount-1));
                    boss1BodyBezier1(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(vertexVector[1]);
                }
                else{
                    percent=((i-1)/(float)(outerVertexCount-1));
                    boss1BodyBezier1(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(vertexVector[1]);
                }
                if (i==outerVertexCount-1){//Последний треугольник
                    percent=(i/(float)(outerVertexCount-1));
                    boss1BodyBezier1(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(vertexVector[1]);
                }
                else{
                    percent=((i+1)/(float)(outerVertexCount-1));
                    boss1BodyBezier1(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(vertexVector[1]);
                }
            }
            for (int i=0;i<outerVertexCount;++i){
                percent=(i/(float)(outerVertexCount-1));
                //Log.wtf(LOG_TAG,""+percent);
                boss1BodyBezier1(vertexVector,i%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(-vertexVector[1]);
                if (i==0) {//первый треугольник
                    //percent=(i/(float)(outerVertexCount-1));
                    boss1BodyBezier1(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(-vertexVector[1]);
                }
                else{
                    percent=((i-1)/(float)(outerVertexCount-1));
                    boss1BodyBezier1(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(-vertexVector[1]);
                }
                if (i==outerVertexCount-1){//Последний треугольник
                    percent=(i/(float)(outerVertexCount-1));
                    boss1BodyBezier1(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(-vertexVector[1]);
                }
                else{
                    percent=((i+1)/(float)(outerVertexCount-1));
                    boss1BodyBezier1(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(-vertexVector[1]);
                }
            }

            x0 = 50; y0 = -297;
            x1 = -60; y1 = -60;
            x2 = -266 - 119 * animationStatus; y2 = 0;
            for (int i=0;i<outerVertexCount;++i){
                percent=(i/(float)(outerVertexCount-1));
                //Log.wtf(LOG_TAG,""+percent);
                boss1BodyBezier2(vertexVector,i%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(vertexVector[1]);
                if (i==0) {//первый треугольник
                    //percent=(i/(float)(outerVertexCount-1));
                    boss1BodyBezier2(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(vertexVector[1]);
                }
                else{
                    percent=((i-1)/(float)(outerVertexCount-1));
                    boss1BodyBezier2(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(vertexVector[1]);
                }
                if (i==outerVertexCount-1){//Последний треугольник
                    percent=(i/(float)(outerVertexCount-1));
                    boss1BodyBezier2(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(vertexVector[1]);
                }
                else{
                    percent=((i+1)/(float)(outerVertexCount-1));
                    boss1BodyBezier2(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(vertexVector[1]);
                }
            }
            for (int i=0;i<outerVertexCount;++i){
                percent=(i/(float)(outerVertexCount-1));
                //Log.wtf(LOG_TAG,""+percent);
                boss1BodyBezier2(vertexVector,i%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(-vertexVector[1]);
                if (i==0) {//первый треугольник
                    //percent=(i/(float)(outerVertexCount-1));
                    boss1BodyBezier2(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(-vertexVector[1]);
                }
                else{
                    percent=((i-1)/(float)(outerVertexCount-1));
                    boss1BodyBezier2(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(-vertexVector[1]);
                }
                if (i==outerVertexCount-1){//Последний треугольник
                    percent=(i/(float)(outerVertexCount-1));
                    boss1BodyBezier2(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(-vertexVector[1]);
                }
                else{
                    percent=((i+1)/(float)(outerVertexCount-1));
                    boss1BodyBezier2(vertexVector,(i+1)%2,percent,animationStatus,x0,y0,x1,y1,x2,y2,deltaX);
                    current_vertices.push_back(vertexVector[0]);    current_vertices.push_back(-vertexVector[1]);
                }
            }

        }





        V.BOSS1 = {vertices.size()/2, current_vertices.size()/2/ANIMATION_FRAMES, GL_TRIANGLES};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    if (vertices.size()%2){
        qDebug()<<"ERROR::VertexLoader. Vertices init size:"<<vertices.size();
        assert(false);
    }
    qDebug()<<"vertices:"<<vertices.size()*sizeof(float)/1024<<"kB";
}

std::vector<GLfloat> VertexLoader::getVerticles(){
    return std::move(vertices);
}

Vertex VertexLoader::getVertex(){
    return std::move(V);
}

void VertexLoader::sharkBodyBezier1(std::vector<float>& whatIshouldReturn, int side, float percent, float animationStatus, float x0, float y0, float x1, float y1, float x2, float y2, float deltaX){
    //std::vector<float> whatIshouldReturn(2);
    if (side%2==0)
    {
        whatIshouldReturn[0]= (x0*(1-percent)*(1-percent)+2*percent*(1-percent)*x1+percent*percent*x2);
        whatIshouldReturn[1]=(y0*(1-percent)*(1-percent)+2*percent*(1-percent)*y1+percent*percent*y2);
    }
    else{//Дополнительная кривая Безье для толщины
        whatIshouldReturn[0]= ((x0+3*deltaX)*(1-percent)*(1-percent)+2*percent*(1-percent)*(x1-4*deltaX)+percent*percent*(x2-12*deltaX));
        whatIshouldReturn[1]=((y0+10*deltaX)*(1-percent)*(1-percent)+2*percent*(1-percent)*(y1+8*deltaX)+percent*percent*(y2));
    }
    //return whatIshouldReturn;
}//Убрать!

void VertexLoader::sharkBodyBezier2(std::vector<float>& whatIshouldReturn, int side, float percent, float animationStatus, float x0, float y0, float x1, float y1, float x2, float y2, float deltaX){
    //std::vector<float> whatIshouldReturn(2);
    if (side%2==0)//Чётные числа для внешего радиуса
    {
        whatIshouldReturn[0]=(x0*(1-percent)*(1-percent)+2*percent*(1-percent)*x1+percent*percent*x2);
        whatIshouldReturn[1]=(y0*(1-percent)*(1-percent)+2*percent*(1-percent)*y1+percent*percent*y2);
    }
    else{//Дополнительная кривая Безье для толщины
        whatIshouldReturn[0]=((x0+3*deltaX)*(1-percent)*(1-percent)+2*percent*(1-percent)*(x1+12*deltaX)+percent*percent*(x2+20*deltaX*(2+animationStatus)));
        whatIshouldReturn[1]=((y0+10*deltaX)*(1-percent)*(1-percent)+2*percent*(1-percent)*(y1+12*deltaX)+percent*percent*(y2));
    }
    //return whatIshouldReturn;
}//Убрать!

void VertexLoader::boss1BodyBezier1(std::vector<float>& whatIshouldReturn, int side, float percent, float animationStatus, float x0, float y0, float x1, float y1, float x2, float y2, float deltaX){
    if (side%2==0)
    {
        whatIshouldReturn[0]= (x0*(1-percent)*(1-percent)+2*percent*(1-percent)*x1+percent*percent*x2);
        whatIshouldReturn[1]=(y0*(1-percent)*(1-percent)+2*percent*(1-percent)*y1+percent*percent*y2);
    }
    else{//Дополнительная кривая Безье для толщины
        whatIshouldReturn[0]= ((x0+3*deltaX)*(1-percent)*(1-percent)+2*percent*(1-percent)*(x1-4*deltaX)+percent*percent*(x2-10*deltaX));
        whatIshouldReturn[1]=((y0+10*deltaX)*(1-percent)*(1-percent)+2*percent*(1-percent)*(y1+8*deltaX)+percent*percent*(y2));
    }
}//Передняя часть акулки

void VertexLoader::boss1BodyBezier2(std::vector<float>& whatIshouldReturn, int side, float percent, float animationStatus, float x0, float y0, float x1, float y1, float x2, float y2, float deltaX){
    if (side%2==0)//Чётные числа для внешего радиуса
    {
        whatIshouldReturn[0]=(x0*(1-percent)*(1-percent)+2*percent*(1-percent)*x1+percent*percent*x2);
        whatIshouldReturn[1]=(y0*(1-percent)*(1-percent)+2*percent*(1-percent)*y1+percent*percent*y2);
    }
    else{//Дополнительная кривая Безье для толщины
        whatIshouldReturn[0]=((x0+3*deltaX)*(1-percent)*(1-percent)+2*percent*(1-percent)*(x1)+percent*percent*(x2+13*deltaX*(1.5f+animationStatus*0.5f)));
        whatIshouldReturn[1]=((y0+10*deltaX)*(1-percent)*(1-percent)+2*percent*(1-percent)*(y1+4*deltaX)+percent*percent*(y2));
    }
}//Хвост акулки
