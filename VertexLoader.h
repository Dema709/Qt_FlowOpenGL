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
    int ANIMATION_FRAMES;//Количество кадров для анимации, должно выходить из требуемого FPS

    //Начальный номер для вершин фигуры и их количество
    VertexPair ASIX;
    VertexPair SQUARE;
    VertexPair PARTICLE;//Фоновая частица
    VertexPair ELLIPSE;
    VertexPair ROUND;
    VertexPair LOWPOLY_ROUND;
    VertexPair BEZIER;//Лапки у type0 еды - через кривые безье
    VertexPair PENTAGON;//Пятиугольник с средним радиусом 1 и толщиной стенки 0.2f
    VertexPair ROUNDED_TRIANGLE_INCENTER;//Треугольник в 60 градусов с выгнутой круглой стороной
    VertexPair ROUNDED_TRIANGLE_OUTCENTER;//Треугольник в 60 градусов с вогнутой круглой стороной
    VertexPair PLUS;//Плюсик
    VertexPair TRIANGLE;//Треугольник
    VertexPair RING1;//Кольцо с средним радиусом 1 и толщиной 0,1886792452830189
    VertexPair RING2;//Кольцо с средним радиусом 1 и толщиной 0,3773584905660377
    VertexPair RING3;//Кольцо с средним радиусом 1 и толщиной 0,1f
};

class VertexLoader
{
public:
    VertexLoader(int ANIMATION_FRAMES_);//Инициализация, создаёт массив вершин и связанную структуру, хранящую индексы моделей
    std::vector<GLfloat> getVerticles();
    Vertex getVertex();

private:
    Vertex V;
    std::vector<GLfloat> vertices;
};

#endif // VERTEX_INITIALIZATION
