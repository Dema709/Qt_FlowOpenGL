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
    VertexLoader vertexLoader;
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

    /*vertices = {
        //Исходные данные для треугольника
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.0f,  0.5f,
        //Ось x
        -1, 0,
         1, 0,
        //Ось y
        0, -1,
        0, 1,
    };*/

        //См. https://habr.com/ru/post/311808/
        initializeGLFunctions();//Для включения функций, так как виджет унаследован от protected QGLFunctions
        //GLfloat vertices[] = {-0.5f, -0.5f, 0.0f,    0.5f, -0.5f, 0.0f,          0.0f,  0.5f, 0.0f        };
        GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        /*//Добавочка для проверки sizeof(vertices), связана с каким-то приколом подсчёта занимаемого места
        unsigned long long real_size1, vec_size2;
        {
            GLfloat v1[] =              {-0.5f, -0.5f, 0.0f,    0.5f, -0.5f, 0.0f,          0.0f,  0.5f};
            std::vector<GLfloat> v2 =   {-0.5f, -0.5f, 0.0f,    0.5f, -0.5f, 0.0f,          0.0f,  0.5f};
            real_size1 = sizeof(v1); vec_size2 = sizeof(v2);
            qDebug()<<sizeof(v1)<<sizeof(v2);
        }*/

        //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &(vertices[0]), GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), &(vertices[0]), GL_STATIC_DRAW);
        //sizeof(vertices) В оригинале без умножения! Но для вектора как-то неправильно считается

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


        //Добавочка с https://stackoverflow.com/questions/42359575/implementing-a-fragment-shader-that-behaves-similar-to-glcolor4f
        uColorLocation = glGetUniformLocation(shaderProgram, "u_Color");
        glUniform4f(uColorLocation, 1, 0, 0, 1);//Красный цвет по умолчанию

        //И из android приложения на счёт матрицы
        uMatrixLocation = glGetUniformLocation(shaderProgram, "u_Matrix");
        //QMatrix4x4 mMatrix;
        glUniformMatrix4fv(uMatrixLocation, 1, false, mMatrix.constData());//Матрица по умолчанию

        //Начальные положения мышки
        mouse_pos_x = width()/2; mouse_pos_y=height()/2;
        //qDebug()<<mouse_pos_x<<mouse_pos_y;

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
        timer->start(invertFPS); // И запустим таймер

        qDebug()<<"initializeGL end";
};
void Widget::paintGL(){
    //qDebug()<<"paintGL";
    #if FPS_DEBUG
        calculateFPS();
    #endif

    /*QMatrix4x4 mMatrix;
    mMatrix.perspective(0,1,-5,5);
    glUniformMatrix4fv(uMatrixLocation, 1, false, mMatrix.constData());*/
    /*
        float eyeX = 0;
        float eyeY = 0;
        float eyeZ = 3;

        // точка направления камеры
        float centerX = 0;
        float centerY = 0;
        float centerZ = 0;

        // up-вектор
        float upX = 0;
        float upY = 1;
        float upZ = 0;
    */
    //mMatrix.lookAt({0,0,3},{0,0,0},{0,1,0});
    //matrix.perspective(0,1,-5,5);
    //mMatrix.ortho(-1,1,-1,1,-5,5);
    //mMatrix.scale(1.);//Больше - ближе
    //mat4 zzzzzz = gperspective(90.0f, 1.0f, 0.1f, 100.0f);

    /*
    Then instead of using.
    shaders->setUniformValue(projIndex, QMatrix4x4(glm::value_ptr(proj)) );
    you must use :
    shaders->setUniformValue(projIndex, QMatrix4x4(glm::value_ptr(proj)).transposed());
    */


    glClearColor(0.9,0.9,1, 0);//Цвет фона
    glClear(GL_COLOR_BUFFER_BIT);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    #define DRAW(figure_name) glDrawArrays(V.figure_name.mode,V.figure_name.index,V.figure_name.count)

    if (true){//Тестовое отображение всех фигур
        glLineWidth(3.0f);
        glUniform4f(uColorLocation, 1, 0, 0, 1);//Красная ось x
        glDrawArrays(V.ASIX.mode, V.ASIX.index, V.ASIX.count/2);

        glUniform4f(uColorLocation, 0, 1, 0, 1);//Зелёная ось y
        glDrawArrays(V.ASIX.mode, V.ASIX.index + V.ASIX.count/2, V.ASIX.count/2);

        int figure_n = 0, max_figures_in_row = 10;//Текущий номер фигуры
        float start_x = -450, start_y = -450;//Начальное смещение по осям
        float dx = 100, dy = 100;
        {
            QMatrix4x4 tempMatrix(mMatrix);
            tempMatrix.translate(start_x+dx*(figure_n%max_figures_in_row), start_y+dy*(figure_n/max_figures_in_row));
            tempMatrix.scale(50);    figure_n++;
            glUniform4f(uColorLocation, 0, 0, 1, 0.7);
            glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
            DRAW(SQUARE);//Квадрат
        }

        {
            QMatrix4x4 tempMatrix(mMatrix);
            tempMatrix.translate(start_x+dx*(figure_n%max_figures_in_row), start_y+dy*(figure_n/max_figures_in_row));
            tempMatrix.scale(6);    figure_n++;
            glUniform4f(uColorLocation, 0, 0, 1, 0.2);
            glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
            DRAW(PARTICLE);//Частица
        }

        {
            QMatrix4x4 tempMatrix(mMatrix);
            tempMatrix.translate(start_x+dx*(figure_n%max_figures_in_row), start_y+dy*(figure_n/max_figures_in_row));
            tempMatrix.scale(3);    tempMatrix.rotate(45,0,0,1);   figure_n++;
            glUniform4f(uColorLocation, 0, 0, 1, 0.7);
            glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
            DRAW(ELLIPSE);//Эллипс
        }

        {
            QMatrix4x4 tempMatrix(mMatrix);
            tempMatrix.translate(start_x+dx*(figure_n%max_figures_in_row), start_y+dy*(figure_n/max_figures_in_row));
            tempMatrix.scale(40);    figure_n++;
            glUniform4f(uColorLocation, 0, 0, 1, 0.7);
            glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
            DRAW(ROUND);//Круг
        }

        {
            QMatrix4x4 tempMatrix(mMatrix);
            tempMatrix.translate(start_x+dx*(figure_n%max_figures_in_row), start_y+dy*(figure_n/max_figures_in_row));
            tempMatrix.scale(40);    figure_n++;
            glUniform4f(uColorLocation, 0, 0, 1, 0.7);
            glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
            DRAW(LOWPOLY_ROUND);//Низкополигональный круг
        }

        glUniformMatrix4fv(uMatrixLocation, 1, false, mMatrix.constData());
    }



    //


    //DRAW(PARTICLE);
    /*{
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(1,0);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(SQUARE);
    glUniformMatrix4fv(uMatrixLocation, 1, false, mMatrix.constData());
    }*/
/*
    for (int i=0; i<particle.getCount(); i++){
        QMatrix4x4 tempMatrix(mMatrix);
        float maxLifetime = particle.getMaxLifeTime();
        glUniform4f(uColorLocation, 0, 0, 1, (0.5 - abs(particle.getLifetime(i) - maxLifetime / 2.) / maxLifetime) * 0.26);//Цвет
        tempMatrix.translate(-particle.getX(i), -particle.getY(i));
        glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
        DRAW(PARTICLE);
        glUniformMatrix4fv(uMatrixLocation, 1, false, mMatrix.constData());
    }*/

    {/*
        openGLRenderer.drawEllipse(currentX, currentY,1+0.2f*multiplSize, orientation * 180 / (float) Math.PI);
        openGLRenderer.drawLowpolyRound(currentX, currentY,3.5f);
        openGLRenderer.drawLowpolyRoundTransfered(currentX, currentY,3.5f,orientation * 180 / (float) Math.PI,8.4f,0);
        openGLRenderer.drawLowpolyRoundTransfered(currentX, currentY,3.5f,orientation * 180 / (float) Math.PI,-8.4f,0);
        openGLRenderer.drawBezier(currentX, currentY,0.7f*1.5f,orientation * 180 / (float) Math.PI, multiplSnake);
        */

    }
};
void Widget::resizeGL(int width, int height){
    qDebug()<<"resizeGL with (width ="<<width<<"; height ="<<height<<")";

    mMatrix.setToIdentity();//Сброс матрицы
    //mMatrix.scale(0.5/2.5);
    mMatrix.scale(1/500.);//-500 ... 500
    //mMatrix.translate(0,0.5);

    if (width<height){
        mMatrix.scale(static_cast<float>(height)/width, 1);
    } else {
        mMatrix.scale(1, static_cast<float>(width)/height);
    }

    glUniformMatrix4fv(uMatrixLocation, 1, false, mMatrix.constData());

    glViewport(0, 0, (GLint)width, (GLint)height);
};

void Widget::slotUpdatePosition()
{
    qint64 dt = dt_timer.elapsed();
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
