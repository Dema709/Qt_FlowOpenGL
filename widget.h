#ifndef WIDGET_H
#define WIDGET_H

#include <QGLWidget>
#include <vector>
#include <QOpenGLShaderProgram>
#include <QGLFunctions>
#include "VertexLoader.h"
#include "particle.h"//Временно, пока не уберётся в уровень

#define HABR true

class Widget : public QGLWidget
        #if HABR
                                , protected QGLFunctions
        #endif
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

    int invertFPS = 1000./30;//Величина, обратная требуемому фпс; Период подсчёта кадров в миллисекундах

    int mouse_pos_x, mouse_pos_y;
    bool is_mouse_pressed = false;

    QElapsedTimer time_start;//Время окончания инициализации для определения реального delta_t
    int frames_to_count = 50, cur_frame = 0;
    QElapsedTimer fps_start;
    QVector<qint64> fps_ms_saved_T;

};
#endif // WIDGET_H
