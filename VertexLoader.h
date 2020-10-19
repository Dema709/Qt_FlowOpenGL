#ifndef VERTEX_INITIALIZATION
#define VERTEX_INITIALIZATION

#include <vector>
#include <QtOpenGL>//GLfloat

struct VertexPair{
    size_t index;//Начальный индекс пары координат (точки)
    size_t count;//Количество точек
    GLenum mode;//Режим отрисовки
        //GL_LINES - линия между точками
        //GL_TRIANGLES - треугольники по три точки
        //GL_TRIANGLE_FAN - треугольники с общей первой точкой (и предыдущей)
        //GL_TRIANGLE_STRIP - последовательные треугольники
};

struct Vertex{
    //Начальный номер для вершин фигуры и их количество
    VertexPair ASIX;
    VertexPair SQUARE;
    VertexPair PARTICLE;//Фоновая частица
    VertexPair ELLIPSE;
    VertexPair ROUND;
    VertexPair LOWPOLY_ROUND;
    VertexPair BEZIER;//Лапки у type0 еды - через кривые безье
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
    int ANIMATION_FRAMES;//Количество кадров для анимации, должно выходить из требуемого FPS
};

#endif // VERTEX_INITIALIZATION
