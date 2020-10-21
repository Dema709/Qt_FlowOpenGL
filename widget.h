#ifndef WIDGET_H
#define WIDGET_H

#include <QGLWidget>
#include <vector>
#include <QOpenGLShaderProgram>
#include <QGLFunctions>
#include "VertexLoader.h"
#include "particle.h"//Временно, пока не уберётся в уровень

#define FPS_DEBUG false
#define FPS_DEBUG_DETAILED true //Работает только при (FPS_DEBUG == true)

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

    void drawAxes();
    void drawSquare(float centerX, float centerY, float sizeScale, float orientation = 0);
    void drawPentagon(float centerX, float centerY, float sizeScale, float orientation);
    void drawRound(float centerX, float centerY, float radius);
    void drawLowpolyRound(float centerX, float centerY, float radius);
    void drawRoundedTriangleInCenter(float centerX, float centerY, float sizeScale, float orientation);
};
#endif // WIDGET_H
