#ifndef WIDGET_H
#define WIDGET_H

#include <QGLWidget>
#include <vector>
#include <QOpenGLShaderProgram>
#include <QGLFunctions>
#include "VertexLoader.h"
#include "particle.h"//Временно, пока не уберётся в уровень
#include "Camera.h"
#include "Protagonist.h"
#include "Food.h"
#include "Level.h"
#include "SnakeHunter.h"
#include "ChangeLevelFood.h"

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

    int invertFPS = 1000/50;//Величина, обратная требуемому фпс; Период подсчёта кадров в миллисекундах
    //FPS выше 60 смысла ставить нет (?)
    QElapsedTimer dt_timer;

    float mouse_pos_x, mouse_pos_y;
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
    float half_widht, half_height;//Половинные размеры зоны обзора для подсчёта матрицы (соотношение сторон)
    float screen_widht, screen_height;//Размеры окна
    std::mutex screen_size_mutex;
    //Для half_widht, half_height, screen_widht, screen_height
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
    void drawBezier5(float centerX, float centerY, float sizeScale, float orientation, float multiplSnake, float changeLevelFoodRadius);//Для смены уровня (почти как для еды типа 4 (сзади один))
    void drawHalfRing(float centerX, float centerY, float radius);//Нарисовать полукруг (верхняя часть)
    void drawHalfRings(float centerX, float centerY, float radius, float orientation, float rotationForEating);//Нарисовать два полукруга как открытый рот
    void drawMouth(float centerX, float centerY, float orientation, float multiplSnake);//Рот ГГ
    void drawSharkBody(float centerX, float centerY, float orientation, float multiplSnake, float scaleForLittleOrBigFish);
    void drawPatricle(float centerX, float centerY, float alpha);
    void drawSquareTransfered(float centerX, float centerY, float sizeScale, float orientation, float offsetX, float offsetY);
    void drawLowpolyRoundTransfered(float centerX, float centerY, float radius, float orientation, float offsetX, float offsetY);
    void drawRoundedTriangleInCenterTransfered(float centerX, float centerY, float sizeScale, float orientation);
    void drawRoundedTriangleOutCenterTransfered(float centerX, float centerY, float sizeScale, float orientation);
    void drawSquaresTransferedWithScale(float centerX, float centerY, float orientation, float deltaAngle, float scale);
    void setColor(float r, float g, float b, float a);
private:
    ChakaPon::Camera camera;
    Particle particle;
    Protagonist protagonist;
    QVector3D rotating_about = {0,0,1};//Вектор, относительно которого происходит вращение
    std::vector<Food> food, test_food;

    std::vector<Level> levelArray;
    int shouldIChangeLevel=0;
    int levelNum=8;//Количество уровней = Последний уровень + 1
    int currentLevel=0;

    std::vector<float> currentColor, oldColor, newColor;
    float changeBackgroundColorTime=10;
};
#endif // WIDGET_H
