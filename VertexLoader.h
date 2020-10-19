#ifndef VERTEX_INITIALIZATION
#define VERTEX_INITIALIZATION

#include <vector>
#include <QtOpenGL>//GLfloat

struct VertexPair{
    size_t index;//Начальный индекс пары координат (точки)
    size_t count;//Количество точек
};

struct Vertex{
    //Начальный номер для вершин фигуры и их количество
    VertexPair ASIX;
    VertexPair SQUARE;
    VertexPair PARTICLE;//Фоновая частица
};

class VertexLoader
{
public:
    VertexLoader();//Инициализация, создаёт массив вершин и связанную структуру, хранящую индексы моделей
    std::vector<GLfloat> getVerticles();
    Vertex getVertex();

private:
    Vertex V;
    std::vector<GLfloat> vertices;
};

#endif // VERTEX_INITIALIZATION
