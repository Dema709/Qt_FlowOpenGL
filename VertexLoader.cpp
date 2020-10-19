#include "VertexLoader.h"

#include <QDebug>//Временное включение для отладки

VertexLoader::VertexLoader()
{
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
        //Лапки у type0 еды - через кривые безье
        std::vector<GLfloat> current_vertices;
        //VERTEX_BEZIER_COUNT = 16+1;
        /*
        float x0, y0, x1, y1, x2=0, y2 = 0; float animationStatus=0;
        x0 = 0; y0 = 11-4.5f; x1 = 0; y1 = 18-4.5f;
        for (int j=0;j<ANIMATION_FRAMES;++j) {
            animationStatus=(j/(float)(ANIMATION_FRAMES-1));
            x2 = -15*animationStatus; y2 = 28-10*animationStatus;//-4.5f;
            outerVertexCount=VERTEX_BEZIER_COUNT;
            for (int i=0;i<outerVertexCount;++i){
                percent=(i/(float)(outerVertexCount-1));
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
                vertices[idx++]=outer_x;
                vertices[idx++]=outer_y;
                if (POSITION_COUNT==3) vertices[idx++]=0;
            }
        }
        */
        //Вызов будет происходить для переопределённого DRAW и вычисления первой точки на основе текущего кадра анимации
        V.BEZIER = {vertices.size()/2, current_vertices.size()/2/ANIMATION_FRAMES, GL_TRIANGLE_STRIP};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }


}

std::vector<GLfloat> VertexLoader::getVerticles(){
    return std::move(vertices);
}

Vertex VertexLoader::getVertex(){
    return std::move(V);
}
