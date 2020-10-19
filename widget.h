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

    float invertFPS = 1./30;//Величина, обратная требуемому фпс

    int mouse_pos_x, mouse_pos_y;
    bool is_mouse_pressed = false;

};
#endif // WIDGET_H
