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
        //Частица
        std::vector<GLfloat> current_vertices;
        int VERTEX_PARTICLE_COUNT = 16*9;
        for (int j = 0; j < 3; j++) {
            float radius = 2./3.*2.5;//Стандартный масштаб
            int outerVertexCount = VERTEX_PARTICLE_COUNT/9 + 1;
            for (int i = 0; i < outerVertexCount - 1; ++i) {
                current_vertices.push_back(0);
                current_vertices.push_back(0);
                float percent = (i / (float) (outerVertexCount - 1));
                float rad = (float) (percent * 2*M_PI);
                float outer_x = (float) (radius * (j + 1) * cos(rad));
                float outer_y = (float) (radius * (j + 1) * sin(rad));
                current_vertices.push_back(outer_x);
                current_vertices.push_back(outer_y);

                percent = ((i + 1) / (float) (outerVertexCount - 1));
                rad = (float) (percent * 2*M_PI);
                outer_x = (float) (radius * (j + 1) * cos(rad));
                outer_y = (float) (radius * (j + 1) * sin(rad));
                current_vertices.push_back(outer_x);
                current_vertices.push_back(outer_y);
            }
        }
/*
        qDebug()<<"VERTEX_PARTICLE";
        for (int i=0; i<current_vertices.size()/2;i++){
            qDebug()<<current_vertices[i*2]<<current_vertices[i*2+1];
        }
        qDebug()<<"VERTEX_PARTICLE";
*/


        V.PARTICLE = {vertices.size()/2, current_vertices.size()/2, GL_TRIANGLES};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    //qDebug()<<"Initial size:"<<vertices.size();
}

std::vector<GLfloat> VertexLoader::getVerticles(){
    return std::move(vertices);
}

Vertex VertexLoader::getVertex(){
    return std::move(V);
}
