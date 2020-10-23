#ifndef WIDGET_H
#define WIDGET_H

#include <QGLWidget>
#include <vector>
#include <QOpenGLShaderProgram>
#include <QGLFunctions>
#include "VertexLoader.h"
#include "particle.h"//Временно, пока не уберётся в уровень

#include <mutex>

#define FPS_DEBUG false
#define FPS_DEBUG_DETAILED false //Работает только при (FPS_DEBUG == true)

class Widget : public QGLWidget, protected QGLFunctions
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
private slots:
    void slotUpdatePosition();//Обновление физики (местоположения) и последующая отрисовка
private:
    std::vector<GLfloat> vertices;
    Vertex V;

    GLint uColorLocation;
    GLint uMatrixLocation;
    GLuint shaderProgram;

    QMatrix4x4 mMatrix;

    Particle particle;

    int invertFPS = 1000/50;//Величина, обратная требуемому фпс; Период подсчёта кадров в миллисекундах
    //FPS выше 60 смысла ставить нет (?)
    QElapsedTimer dt_timer;

    int mouse_pos_x, mouse_pos_y;
    bool is_mouse_pressed = false;

    #if FPS_DEBUG
        void calculateFPS();
        int frames_to_count = 50, cur_frame = 0;
        QElapsedTimer FPS_timer;
        #if FPS_DEBUG_DETAILED
            QElapsedTimer detailed_FPS_timer;//Время окончания инициализации для определения реального delta_t
            QVector<qint64> delay_vector_for_fps;
        #endif
    #endif

    int view_base = 360;//Минимальная половина зона обзора по одной из осей
    int half_widht, half_height;//Половинные размеры зоны обзора для подсчёта матрицы
    std::mutex screen_size_mutex;
    //Для half_widht, half_height
    std::mutex mouse_control_mutex;
    //Для mouse_pos_x, mouse_pos_y, is_mouse_pressed
    std::mutex global_mutex;
    //Для разделения процесса отрисовки и подсчёта движения

public:

    void drawAxes();
    void drawSquare(float centerX, float centerY, float sizeScale, float orientation = 0);
    void drawPentagon(float centerX, float centerY, float sizeScale, float orientation);
    void drawRound(float centerX, float centerY, float radius);
    void drawLowpolyRound(float centerX, float centerY, float radius);
    void drawRoundedTriangleInCenter(float centerX, float centerY, float sizeScale, float orientation);
    void drawRoundedTriangleOutCenter(float centerX, float centerY, float sizeScale, float orientation);
    void drawPlus(float centerX, float centerY, float sizeScale, float orientation);
    void drawEllipse(float centerX, float centerY, float sizeScale, float orientation);
    void drawTriangle(float centerX, float centerY, float radius, float orientation);
    void drawRing(float centerX, float centerY, float radius);
    void drawRing2(float centerX, float centerY, float radius);
    void drawRing3(float centerX, float centerY, float radius);
    void drawRingTransfered(float centerX, float centerY, float radius, float orientation, float offsetX, float offsetY);
    void drawRing2Transfered(float centerX, float centerY, float radius, float orientation, float offsetX, float offsetY);
    void drawRing3Transfered(float centerX, float centerY, float radius, float orientation, float offsetX, float offsetY);
    void drawBezier(float centerX, float centerY, float sizeScale, float orientation, float multiplSnake);//Для еды типа 0 (по бокам)
    void drawBezier2(float centerX, float centerY, float sizeScale, float orientation, float multiplSnake);//Для еды типа 3 (сзади два)
    void drawBezier3(float centerX, float centerY, float sizeScale, float orientation, float multiplSnake);//Для еды типа 4 (сзади один)
    void drawBezier4(float centerX, float centerY, float sizeScale, float orientation, float multiplSnake);//Для еды типа 6 (сзади два, чуть поодаль друг от друга)
    void drawHalfRing(float centerX, float centerY, float radius);//Нарисовать полукруг (верхняя часть)
    void drawHalfRings(float centerX, float centerY, float radius, float orientation, float rotationForEating);//Нарисовать два полукруга как открытый рот
    void drawMouth(float centerX, float centerY, float orientation, float multiplSnake);//Рот ГГ

    void drawSharkBody(float centerX, float centerY, float orientation, float multiplSnake, float scaleForLittleOrBigFish);
    void drawPatricle(float centerX, float centerY, float alpha);

};
#endif // WIDGET_H
