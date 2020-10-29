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
    VertexPair HALFRING;//Полукольцо с средним радиусом 1 и толщиной 0,1886792452830189. Верхняя часть
    VertexPair MOUTH;//Рот у ГГ - через кривые безье
    VertexPair SHARKBODY;//Тело акулки //4 части * 3 точки у треугольника
    VertexPair SHARKMOUTH;//Три "буквы Л"
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

    //Убрать!
    void sharkBodyBezier1(std::vector<float>& whatIshouldReturn, int side, float percent, float animationStatus, float x0, float y0, float x1, float y1, float x2, float y2, float deltaX);
    void sharkBodyBezier2(std::vector<float>& whatIshouldReturn, int side, float percent, float animationStatus, float x0, float y0, float x1, float y1, float x2, float y2, float deltaX);
};

#endif // VERTEX_INITIALIZATION
