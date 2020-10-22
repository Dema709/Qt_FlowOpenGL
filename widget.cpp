#include "widget.h"


#include <QDebug>
#include <QtGui>
#include <QtWidgets>
#include <QtOpenGL>
#if FPS_DEBUG_DETAILED
    #include <algorithm>//Для FPS_DEBUG_DETAILED
#endif

Widget::Widget(QWidget *parent)
    : QGLWidget(parent), particle(20)
{
    qDebug()<<"Widget constructor";
    VertexLoader vertexLoader(21);//Animation frames
    V = vertexLoader.getVertex();
    vertices = vertexLoader.getVerticles();

    //Mouse move events will occur only when a mouse button is pressed down,
    //unless mouse tracking has been enabled with QWidget::setMouseTracking().
    setMouseTracking(true);//Отслеживание мыши вне зависимости от нажатия
}

Widget::~Widget()
{
    qDebug()<<"Widget destructor";
}

    const GLchar* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec2 position;\n"
        "uniform highp mat4 u_Matrix;\n"
        "void main()\n"
        "{\n"
        "gl_Position = u_Matrix*vec4(position.x, position.y, 0.0, 1.0);\n"
        "}\0";
    const GLchar* fragmentShaderSource =
            "#version 330 core\n"
            "uniform vec4 u_Color;\n"
            "void main() {\n"
            "   gl_FragColor = u_Color;\n"
            "}\n";

void Widget::initializeGL(){
    qDebug()<<"initializeGL";

    glClearColor(0,0x6D/255., 0xBB/255., 0);//Цвет фона
    glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);//Прозрачность

        //См. https://habr.com/ru/post/311808/
        initializeGLFunctions();//Для включения функций, так как виджет унаследован от protected QGLFunctions
        GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), &(vertices[0]), GL_STATIC_DRAW);
        //В оригинале без умножения, sizeof(vertices) для массива. Но для вектора так неправильно считается

        GLuint vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        {
            GLint success;
            GLchar infoLog[512];
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
                qDebug() << "ERROR::SHADER::1::COMPILATION_FAILED\n" << infoLog;
            }
        }
        GLuint fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        {
            GLint success;
            GLchar infoLog[512];
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
                qDebug() << "ERROR::SHADER::2::COMPILATION_FAILED\n" << infoLog;
            }
        }
        //GLuint shaderProgram;
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        {
            GLint success;
            GLchar infoLog[512];
            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
                qDebug() << "ERROR::SHADER::3::COMPILATION_FAILED\n" << infoLog;
            }
        }
        glUseProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        uColorLocation = glGetUniformLocation(shaderProgram, "u_Color");
        glUniform4f(uColorLocation, 1, 0, 0, 1);//Красный цвет по умолчанию

        uMatrixLocation = glGetUniformLocation(shaderProgram, "u_Matrix");
        glUniformMatrix4fv(uMatrixLocation, 1, false, mMatrix.constData());//Матрица по умолчанию

        //Начальные положения мышки
        mouse_pos_x = width()/2; mouse_pos_y=height()/2;        //qDebug()<<mouse_pos_x<<mouse_pos_y;

        QTimer *timer = new QTimer(this);
        timer->setTimerType(Qt::PreciseTimer);//Precise timers try to keep millisecond accuracy
        connect(timer, SIGNAL(timeout()), this, SLOT(slotUpdatePosition()));

        #if FPS_DEBUG
            FPS_timer.start();
            #if FPS_DEBUG_DETAILED
                delay_vector_for_fps.fill(0, frames_to_count);
                detailed_FPS_timer.start();
            #endif
        #endif

        dt_timer.start();//Таймер для определения dt для физики
        timer->start(invertFPS); //Таймер для вызова функции расчёта физики (и последующей отрисовки)

        qDebug()<<"initializeGL end";
};
void Widget::paintGL(){
    //qDebug()<<"paintGL";

    #if FPS_DEBUG
        calculateFPS();
    #endif

    glClearColor(0.9,0.9,1, 0);//Цвет фона
    glClear(GL_COLOR_BUFFER_BIT);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    #define DRAW(figure_name) glDrawArrays(V.figure_name.mode, V.figure_name.index, V.figure_name.count)
    #define DRAW_A(figure_name, animation_frame) glDrawArrays(V.figure_name.mode, V.figure_name.index+V.figure_name.count*animation_frame, V.figure_name.count)

    drawAxes();
    glUniform4f(uColorLocation, 0, 0, 1, 0.7);
    drawSquare(-300, 300, 50);
    drawSquare(-200, 300, 50, 45);
    drawPentagon(-100, 300, 25, 0);
    drawRound(-300, 200, 25);
    drawLowpolyRound(-200, 200, 25);
    drawRoundedTriangleInCenter(-100, 200, 25, 0);
    drawRoundedTriangleOutCenter(-100, 200, 25, 0);
    drawPlus(-300, 100, 25 / 5., 45);
    drawEllipse(-200, 100, 3, -45);
    drawTriangle(-100, 100, 25, -90);
    drawRing2(-300, -100, 25);
    drawRing(-200, -100, 25);
    drawRing3(-100, -100, 25);

    drawBezier(500, 300, 2, 0, 0);
    drawBezier(550, 300, 2, 0, 0.5f);
    drawBezier(600, 300, 2, 0, 1);
//    drawBezier2(500, 200, 2, 0, 0);
//    drawBezier2(550, 200, 2, 0, 0.5f);
//    drawBezier2(600, 200, 2, 0, 1);
//    drawBezier3(500, 100, 2, 0, 0);
//    drawBezier3(550, 100, 2, 0, 0.5f);
//    drawBezier3(600, 100, 2, 0, 1);
//    drawBezier4(500, 0, 2, 0, 0);
//    drawBezier4(550, 0, 2, 0, 0.5f);
//    drawBezier4(600, 0, 2, 0, 1);
};
void Widget::resizeGL(int width, int height){
    qDebug()<<"resizeGL with (width ="<<width<<"; height ="<<height<<")";

    mMatrix.setToIdentity();//Сброс матрицы

    float ortho_half_widht, ortho_half_height, base = 360;
    if (width>height){
        ortho_half_height = base;
        ortho_half_widht = base * width / height;
    } else {
        ortho_half_widht = base;
        ortho_half_height = base * height / width;
    }
    mMatrix.ortho(-ortho_half_widht,ortho_half_widht,-ortho_half_height,ortho_half_height,0,1);
    float pos_x=0, pos_y=0;
    mMatrix.lookAt({pos_x,pos_y,1},{pos_x,pos_y,0},{0,1,0});

    /*
                    resizeGL ->>>>>>>
                    float ortho_half_widht, ortho_half_height, base = 360;
                    if (width>height){
                        ortho_half_height = base;
                        ortho_half_widht = base * width / height;
                    } else {
                        ortho_half_widht = base;
                        ortho_half_height = base * height / width;
                    }


                    float pos_x, pos_y;
                    pos_x = 60; pos_y = 0;


                    mMatrix.ortho(-ortho_half_widht,ortho_half_widht,-ortho_half_height,ortho_half_height,0,1);
                    mMatrix.lookAt({pos_x,pos_y,1},{pos_x,pos_y,0},{0,1,0});
    */
    //mMatrix.scale(0.5/2.5);
    //mMatrix.scale(1/500.);//-500 ... 500
    //mMatrix.translate(0,0.5);
/*
    if (width<height){
        mMatrix.scale(static_cast<float>(height)/width, 1);
    } else {
        mMatrix.scale(1, static_cast<float>(width)/height);
    }*/

    glUniformMatrix4fv(uMatrixLocation, 1, false, mMatrix.constData());

    glViewport(0, 0, (GLint)width, (GLint)height);
};

void Widget::slotUpdatePosition()
{
    float dt = dt_timer.elapsed() / 1000.;
    dt_timer.start();

    particle.updatePosition(dt);
    updateGL();
}

void Widget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
   //qDebug()<<mouseEvent->pos().x()<<mouseEvent->pos().y();
   mouse_pos_x = mouseEvent->pos().x();
   mouse_pos_y = mouseEvent->pos().y();

   //updateGL();
}

void Widget::mousePressEvent(QMouseEvent*)
{
   qDebug()<<"press";
   is_mouse_pressed = true;
}

void Widget::mouseReleaseEvent(QMouseEvent*)
{
   qDebug()<<"release";
   is_mouse_pressed = false;
}

#if FPS_DEBUG
void Widget::calculateFPS(){
        if (cur_frame >= frames_to_count){
            cur_frame = 0;
            qDebug()<<"FPS1:"<<frames_to_count * 1000. / FPS_timer.elapsed();
            FPS_timer.start();
            #if FPS_DEBUG_DETAILED
                qDebug()<<"FPS2:"<<frames_to_count * 1000. /
                          std::accumulate(delay_vector_for_fps.begin(),delay_vector_for_fps.end(),0);
                QString debugS, tempS;
                for (auto t : delay_vector_for_fps){tempS.setNum(t);debugS+=tempS+" ";};
                qDebug()<<"Delay array:"<<debugS;
            #endif//#if FPS_DEBUG_DETAILED
        }

        #if FPS_DEBUG_DETAILED
            delay_vector_for_fps[cur_frame]=detailed_FPS_timer.elapsed();
            detailed_FPS_timer.start();
        #endif//#if FPS_DEBUG_DETAILED

        cur_frame++;
}
#endif//#if FPS_DEBUG

void Widget::drawAxes(){
    glUniformMatrix4fv(uMatrixLocation, 1, false, mMatrix.constData());//Как bindMatrix();
    glLineWidth(3);

    glUniform4f(uColorLocation, 1, 0, 0, 1);//Красная ось x
    glDrawArrays(V.ASIX.mode, V.ASIX.index, V.ASIX.count/2);

    glUniform4f(uColorLocation, 0, 1, 0, 1);//Зелёная ось y
    glDrawArrays(V.ASIX.mode, V.ASIX.index + V.ASIX.count/2, V.ASIX.count/2);
}

void Widget::drawSquare(float centerX, float centerY, float sizeScale, float orientation){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.rotate(orientation,0,0,1);
    tempMatrix.scale(sizeScale);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());//Как bindMatrix();
    DRAW(SQUARE);
}//Нарисовать квадрат со стороной sizeScale

void Widget::drawPentagon(float centerX, float centerY, float sizeScale, float orientation){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.rotate(orientation,0,0,1);
    tempMatrix.scale(sizeScale);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(PENTAGON);
}

void Widget::drawRound(float centerX, float centerY, float radius){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(radius);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(ROUND);
}//Нарисовать заполненный круг вершинами

void Widget::drawLowpolyRound(float centerX, float centerY, float radius){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(radius);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(LOWPOLY_ROUND);
}//Нарисовать заполненный круг вершинами

void Widget::drawRoundedTriangleInCenter(float centerX, float centerY, float sizeScale, float orientation){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale);
    tempMatrix.rotate(orientation,0,0,1);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(ROUNDED_TRIANGLE_INCENTER);
}

void Widget::drawRoundedTriangleOutCenter(float centerX, float centerY, float sizeScale, float orientation){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale);
    tempMatrix.rotate(orientation,0,0,1);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(ROUNDED_TRIANGLE_OUTCENTER);
}

void Widget::drawPlus(float centerX, float centerY, float sizeScale, float orientation){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale);
    tempMatrix.rotate(orientation,0,0,1);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(PLUS);
}

void Widget::drawEllipse(float centerX, float centerY, float sizeScale, float orientation) {
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale);
    tempMatrix.rotate(orientation,0,0,1);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(ELLIPSE);
}

void Widget::drawTriangle(float centerX, float centerY, float radius, float orientation){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(radius);
    tempMatrix.rotate(orientation,0,0,1);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(TRIANGLE);
}

void Widget::drawRing(float centerX, float centerY, float radius){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(radius);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(RING1);
}

void Widget::drawRing2(float centerX, float centerY, float radius){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(radius);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(RING2);
}

void Widget::drawRing3(float centerX, float centerY, float radius){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(radius);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(RING3);
}

void Widget::drawRingTransfered(float centerX, float centerY, float radius, float orientation, float offsetX, float offsetY){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(radius);
    tempMatrix.rotate(orientation,0,0,1);
    tempMatrix.translate(offsetX/radius,offsetY/radius);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(RING1);
}

void Widget::drawRing2Transfered(float centerX, float centerY, float radius, float orientation, float offsetX, float offsetY){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(radius);
    tempMatrix.rotate(orientation,0,0,1);
    tempMatrix.translate(offsetX/radius,offsetY/radius);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(RING2);
}

void Widget::drawRing3Transfered(float centerX, float centerY, float radius, float orientation, float offsetX, float offsetY){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(radius);
    tempMatrix.rotate(orientation,0,0,1);
    tempMatrix.translate(offsetX/radius,offsetY/radius);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(RING3);
}

void Widget::drawBezier(float centerX, float centerY, float sizeScale, float orientation, float multiplSnake) {
    int frame = multiplSnake * (V.ANIMATION_FRAMES - 1);//Кадр анимации

    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale);
    tempMatrix.rotate(orientation,0,0,1);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW_A(BEZIER,frame);

    tempMatrix = mMatrix;
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale, -sizeScale);
    tempMatrix.rotate(-orientation,0,0,1);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW_A(BEZIER,frame);
}
