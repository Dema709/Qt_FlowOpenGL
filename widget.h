#ifndef WIDGET_H
#define WIDGET_H

#include <QGLWidget>
#include <vector>
#include <QOpenGLShaderProgram>
#include <QGLFunctions>
#include "VertexLoader.h"

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
private:
    std::vector<GLfloat> vertices;
    Vertex V;

    GLint uColorLocation;
    GLint uMatrixLocation;
    GLuint shaderProgram;
};
#endif // WIDGET_H
