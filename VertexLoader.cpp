#include "VertexLoader.h"

//#include <QDebug>//Временное включение для отладки

VertexLoader::VertexLoader()
{
    {
        //Оси
        std::vector<GLfloat> current_vertices = {
            // ось X
            -1, 0,
             1, 0,
            // ось Y
            0, -1,
            0,  1,
        };
        V.ASIX = {vertices.size()/2, current_vertices.size()/2};
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
        V.SQUARE = {vertices.size()/2, current_vertices.size()/2};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    {
        //Частица
        std::vector<GLfloat> current_vertices;
        V.PARTICLE = {vertices.size()/2, current_vertices.size()/2};
        vertices.insert(vertices.end(),current_vertices.begin(),current_vertices.end());
    }

    //qDebug()<<vertices.size();
}

std::vector<GLfloat> VertexLoader::getVerticles(){
    return std::move(vertices);
}

Vertex VertexLoader::getVertex(){
    return std::move(V);
}
