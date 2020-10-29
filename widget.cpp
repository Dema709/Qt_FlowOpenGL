#include "widget.h"


#include <QDebug>
#include <QtGui>
#include <QtWidgets>
#include <QtOpenGL>
#if FPS_DEBUG_DETAILED
    #include <algorithm>//Для FPS_DEBUG_DETAILED
#endif

#include "particle.h"///////////////////////////?

Widget::Widget(QWidget *parent)
    : QGLWidget(parent), particle(20*10,10,1000)
{
    qDebug()<<"Widget constructor";
    VertexLoader vertexLoader(21);//Animation frames
    V = vertexLoader.getVertex();
    vertices = vertexLoader.getVerticles();

    //Mouse move events will occur only when a mouse button is pressed down,
    //unless mouse tracking has been enabled with QWidget::setMouseTracking().
    setMouseTracking(true);//Отслеживание мыши вне зависимости от нажатия

    int foodCount = 0*900;food.reserve(foodCount);for (int i=0; i<foodCount; i++){food.push_back(Food());}
    int testFoodCount = 0*9; test_food.reserve(testFoodCount); for (int i=0; i<testFoodCount; i++){test_food.push_back(Food(i));}

    for (int i=0; i<levelNum; i++){
        levelArray.push_back(Level(i));
    }
    currentColor = levelArray[currentLevel].getColor();
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

    glClearColor(currentColor[0], currentColor[1], currentColor[2], currentColor[3]);//Цвет фона
    glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);//Прозрачность

    //Мультисэмплинг (антиалиасинг) = здесь + перед созданием QGLWidget в main.cpp
    glEnable(GL_MULTISAMPLE);

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

        //Настройка матрицы обзора (View + Projection Matrix)
        float pos_x=0, pos_y=0;
        mMatrix.setToIdentity();//Сброс матрицы
        mMatrix.ortho(-half_widht,half_widht,-half_height,half_height,0,1);
        mMatrix.lookAt({pos_x,pos_y,1},{pos_x,pos_y,0},{0,1,0});

        uMatrixLocation = glGetUniformLocation(shaderProgram, "u_Matrix");
        glUniformMatrix4fv(uMatrixLocation, 1, false, mMatrix.constData());

        //Начальные положения мышки
        mouse_pos_x = width()/2; mouse_pos_y = height()/2;        //qDebug()<<mouse_pos_x<<mouse_pos_y;
        //Начальные положения размера окна
        half_widht = mouse_pos_x; half_height = mouse_pos_y;
        screen_widht = mouse_pos_x*2; screen_height = mouse_pos_y*2;

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

    std::lock_guard<std::mutex> globalLockGuard(global_mutex);

    //Настройка матрицы обзора (View + Projection Matrix)
    float pos_x=0, pos_y=0;//Временно!
    pos_x = camera.getCurrentX(); pos_y = camera.getCurrentY();
    mMatrix.setToIdentity();//Сброс матрицы
    {
        std::lock_guard<std::mutex> screenLockGuard(screen_size_mutex);
        mMatrix.ortho(-half_widht,half_widht,-half_height,half_height,0,1);
    }
    mMatrix.lookAt({pos_x,pos_y,1},{pos_x,pos_y,0},{0,1,0});
    //Без загрузки сразу в память

    #if FPS_DEBUG
        calculateFPS();
    #endif

    if (changeBackgroundColorTime>=1){//Цвет уже сменился
        currentColor = levelArray[currentLevel].getColor();
        /*for (int i=0;i<3;i++){
            curColor[i]=(float)((levelArray[currentLevel].getColor()>>(2-i)*8)&0xFF)/255f;
        }*/
    }
    else {
        if (shouldIChangeLevel!=0){//!changeBackgroundColorAccepted) {//Уровень только что сменился
            oldColor = currentColor;
            newColor = levelArray[currentLevel].getColor();
            /*for (int i=0;i<3;i++){
                oldColor[i] = curColor[i];
                newColor[i]=(float)((levelArray[currentLevel].getColor()>>(2-i)*8)&0xFF)/255f;
            }*/
        } else {
            for (int i=0; i<currentColor.size(); i++){
                currentColor[i] = newColor[i]*changeBackgroundColorTime + oldColor[i]*(1-changeBackgroundColorTime);
                //curColor[i] = newColor[i]*changeBackgroundColorTime+oldColor[i]*(1-changeBackgroundColorTime);
            }
        }
    }

    glClearColor(currentColor[0], currentColor[1], currentColor[2], currentColor[3]);//Цвет фона
    //glClearColor(0.9,0.9,1, 0);//Цвет фона
    glClear(GL_COLOR_BUFFER_BIT);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    #define DRAW(figure_name) glDrawArrays(V.figure_name.mode, V.figure_name.index, V.figure_name.count)
    #define DRAW_A(figure_name, animation_frame) glDrawArrays(V.figure_name.mode, V.figure_name.index+V.figure_name.count*animation_frame, V.figure_name.count)

    if (!true){
    drawAxes();
    glUniform4f(uColorLocation, 0, 0, 1, 0.5);
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
    drawBezier2(500, 200, 2, 0, 0);
    drawBezier2(550, 200, 2, 0, 0.5f);
    drawBezier2(600, 200, 2, 0, 1);
    drawBezier3(500, 100, 2, 0, 0);
    drawBezier3(550, 100, 2, 0, 0.5f);
    drawBezier3(600, 100, 2, 0, 1);
    drawBezier4(500, 0, 2, 0, 0);
    drawBezier4(550, 0, 2, 0, 0.5f);
    drawBezier4(600, 0, 2, 0, 1);

    drawHalfRing(-300, -200, 25);
    drawHalfRings(-200, -200, 25, 45, 0);
    drawHalfRings(-100, -200, 25, 45, 30);

    drawMouth(-300, -300, -45, 0);
    drawMouth(-200, -300, -45, 0.5f);
    drawMouth(-100, -300, -45, 1);

    drawSharkBody(-450, 250, 45, 0, 0.1f);
    drawSharkBody(-580, 250, 45, 1, 0.1f);
    }

    //drawAxes();//Временно:

    protagonist.draw(*this);
    particle.draw(*this);
    levelArray[currentLevel].draw(*this);
    /*for (auto & f : food){
        f.draw(*this);
    }
    for (auto & f : test_food){
        f.draw(*this);
    }*/
};
void Widget::resizeGL(int width, int height){
    //qDebug()<<"resizeGL with (width ="<<width<<"; height ="<<height<<")";

    {
        std::lock_guard<std::mutex> screenLockGuard(screen_size_mutex);
        if (width>height){
            half_height = view_base;
            half_widht = view_base * width / height;
        } else {
            half_widht = view_base;
            half_height = view_base * height / width;
        }
        screen_widht = width;
        screen_height = height;
        //qDebug()<<"resizeGL. half_widht:"<<half_widht<<"; half_height:"<<half_height;

        camera.setScreen(half_widht, half_height);
    }

    glViewport(0, 0, (GLint)width, (GLint)height);
};

void Widget::slotUpdatePosition()
{
    //Все подсчёты перед отображением

    //Получение прошеднего времени с расчёта прошлого кадра
    float dt = dt_timer.elapsed() / 1000.;  //dt = invertFPS / 1000.;
    dt_timer.start();

    //Потокобезопасные переменные для дальнейших расчётов
    float mouse_pos_x_, mouse_pos_y_;
    bool is_mouse_pressed_;
    {
        std::lock_guard<std::mutex> mouseLockGuard(mouse_control_mutex);
        mouse_pos_x_ = mouse_pos_x;
        mouse_pos_y_ = mouse_pos_y;
        is_mouse_pressed_ = is_mouse_pressed;
    }
    float half_height_, half_widht_, screen_height_, screen_widht_;
    {
        std::lock_guard<std::mutex> screenLockGuard(screen_size_mutex);
        half_height_ = half_height;
        half_widht_ = half_widht;
        screen_height_ = screen_height;
        screen_widht_ = screen_widht;
    }

    //Расчёт физики и перемещения
    {
        std::lock_guard<std::mutex> globalLockGuard(global_mutex);
        camera.updateMovement(dt, protagonist);

        //TouchX/Y - координаты на карте. Преобразуются из координат касания экрана
        float target_x = camera.getCurrentX() + (mouse_pos_x_ * 2 - screen_widht_ ) * half_widht_  / screen_widht_;
        float target_y = camera.getCurrentY() - (mouse_pos_y_ * 2 - screen_height_) * half_height_ / screen_height_;
        protagonist.updateMapPosition(dt, is_mouse_pressed_, target_x, target_y);

        particle.updatePosition(dt, protagonist);

        //Изменение уровня и фонового цвета
        shouldIChangeLevel = levelArray[currentLevel].updateFoodMapPosition(dt, protagonist, camera.getCurrentX(), camera.getCurrentY(), half_widht_, half_height_);
        if (shouldIChangeLevel != 0) {
            changeBackgroundColorTime = 0;
            currentLevel = currentLevel + shouldIChangeLevel;
            if (currentLevel < 0) currentLevel=0;//Для предотвращения вылета на -1 несуществующий уровень
            //qDebug()<<"Widget::slotUpdatePosition()"<<"Changing level"<<currentLevel;
        } else {
            changeBackgroundColorTime = changeBackgroundColorTime + dt;
        }

        //for (auto & f : food){f.updateMapPosition(dt);}
        //protagonist.updateEat(food);
        //for (auto & f : test_food){f.updateMapPositionTest(dt);}
    }

    updateGL();
}

void Widget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
    std::lock_guard<std::mutex> mouseLockGuard(mouse_control_mutex);
    mouse_pos_x = mouseEvent->pos().x();
    mouse_pos_y = mouseEvent->pos().y();
}

void Widget::mousePressEvent(QMouseEvent*)
{
    std::lock_guard<std::mutex> mouseLockGuard(mouse_control_mutex);
    is_mouse_pressed = true;

    //if (currentLevel + 1 < levelArray.size()) {currentLevel++; currentColor = levelArray[currentLevel].getColor();}//Мини-тест смены цветов фона
}

void Widget::mouseReleaseEvent(QMouseEvent*)
{
    std::lock_guard<std::mutex> mouseLockGuard(mouse_control_mutex);
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
    tempMatrix.rotate(orientation,rotating_about);
    tempMatrix.scale(sizeScale);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());//Как bindMatrix();
    DRAW(SQUARE);
}//Нарисовать квадрат со стороной sizeScale

void Widget::drawPentagon(float centerX, float centerY, float sizeScale, float orientation){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.rotate(orientation,rotating_about);
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
    tempMatrix.rotate(orientation,rotating_about);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(ROUNDED_TRIANGLE_INCENTER);
}

void Widget::drawRoundedTriangleOutCenter(float centerX, float centerY, float sizeScale, float orientation){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale);
    tempMatrix.rotate(orientation,rotating_about);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(ROUNDED_TRIANGLE_OUTCENTER);
}

void Widget::drawPlus(float centerX, float centerY, float sizeScale, float orientation){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale);
    tempMatrix.rotate(orientation,rotating_about);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(PLUS);
}

void Widget::drawEllipse(float centerX, float centerY, float sizeScale, float orientation) {
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale);
    tempMatrix.rotate(orientation,rotating_about);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(ELLIPSE);
}

void Widget::drawTriangle(float centerX, float centerY, float radius, float orientation){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(radius);
    tempMatrix.rotate(orientation,rotating_about);
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
    tempMatrix.rotate(orientation,rotating_about);
    tempMatrix.translate(offsetX/radius,offsetY/radius);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(RING1);
}

void Widget::drawRing2Transfered(float centerX, float centerY, float radius, float orientation, float offsetX, float offsetY){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(radius);
    tempMatrix.rotate(orientation,rotating_about);
    tempMatrix.translate(offsetX/radius,offsetY/radius);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(RING2);
}

void Widget::drawRing3Transfered(float centerX, float centerY, float radius, float orientation, float offsetX, float offsetY){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(radius);
    tempMatrix.rotate(orientation,rotating_about);
    tempMatrix.translate(offsetX/radius,offsetY/radius);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(RING3);
}

void Widget::drawBezier(float centerX, float centerY, float sizeScale, float orientation, float multiplSnake) {
    int frame = multiplSnake * (V.ANIMATION_FRAMES - 1);//Кадр анимации

    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale);
    tempMatrix.rotate(orientation,rotating_about);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW_A(BEZIER,frame);

    tempMatrix = mMatrix;
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale, -sizeScale);
    tempMatrix.rotate(-orientation,rotating_about);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW_A(BEZIER,frame);
}//Для еды типа 0 (по бокам)

void Widget::drawBezier2(float centerX, float centerY, float sizeScale, float orientation, float multiplSnake){
    int frame = multiplSnake * (V.ANIMATION_FRAMES - 1);//Кадр анимации

    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale);
    tempMatrix.rotate(orientation+90,rotating_about);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW_A(BEZIER,frame);

    tempMatrix = mMatrix;
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale, -sizeScale);
    tempMatrix.rotate(-orientation+90,rotating_about);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW_A(BEZIER,frame);
}//Для еды типа 3 (сзади два)

void Widget::drawBezier3(float centerX, float centerY, float sizeScale, float orientation, float multiplSnake) {
    int frame;//Кадр анимации
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    if (multiplSnake>=0.5){
        tempMatrix.scale(sizeScale);
        tempMatrix.rotate(orientation+90,rotating_about);
        frame = (multiplSnake-0.5f)*2*(V.ANIMATION_FRAMES - 1);
    } else {
        tempMatrix.scale(sizeScale, -sizeScale);
        tempMatrix.rotate(-orientation+90,rotating_about);
        frame = (1-multiplSnake*2)*(V.ANIMATION_FRAMES - 1);
    }
    tempMatrix.translate(-1,1);//Состыковка
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW_A(BEZIER,frame);
}//Для еды типа 4 (сзади один)

void Widget::drawBezier4(float centerX, float centerY, float sizeScale, float orientation, float multiplSnake){
    int frame = multiplSnake * (V.ANIMATION_FRAMES - 1);//Кадр анимации

    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale);
    tempMatrix.rotate(orientation+90,rotating_about);
    tempMatrix.translate(-5,0);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW_A(BEZIER,frame);

    tempMatrix = mMatrix;
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale, -sizeScale);
    tempMatrix.rotate(-orientation+90,rotating_about);
    tempMatrix.translate(-5,0);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW_A(BEZIER,frame);
}//Для еды типа 6 (сзади два, чуть поодаль друг от друга)

void Widget::drawHalfRing(float centerX, float centerY, float radius){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(radius);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(HALFRING);
}//Нарисовать полукруг (верхняя часть)
void Widget::drawHalfRings(float centerX, float centerY, float radius, float orientation, float rotationForEating){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.rotate(orientation,rotating_about);
    tempMatrix.scale(radius);
    tempMatrix.translate(-1.0943396226415094+(float)cos(rotationForEating/180*M_PI),(float)sin(rotationForEating/180*M_PI));
    tempMatrix.rotate(rotationForEating,rotating_about);
    tempMatrix.translate(0.0943396226415094f,0);//Сдвиг в исходной системе из-за рамки
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(HALFRING);

    tempMatrix = mMatrix;
    tempMatrix.translate(centerX,centerY);
    tempMatrix.rotate(orientation,rotating_about);
    tempMatrix.scale(radius);
    tempMatrix.translate(-1.0943396226415094+(float)cos(rotationForEating/180*M_PI),(float)sin(-rotationForEating/180*M_PI));
    tempMatrix.rotate(-rotationForEating,rotating_about);
    tempMatrix.scale(1,-1);//Зеркалим нижнюю челюсть
    tempMatrix.translate(0.0943396226415094f,0);//Сдвиг в исходной системе из-за рамки
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(HALFRING);
}//Нарисовать два полукруга как открытый рот

void Widget::drawMouth(float centerX, float centerY, float orientation, float multiplSnake){
    int frame = multiplSnake * (V.ANIMATION_FRAMES - 1);//Кадр анимации

    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.rotate(orientation,rotating_about);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW_A(MOUTH, frame);

    tempMatrix = mMatrix;
    tempMatrix.translate(centerX,centerY);
    tempMatrix.rotate(orientation,rotating_about);
    tempMatrix.scale(1,-1);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW_A(MOUTH, frame);
}//Рот ГГ

void Widget::drawSharkBody(float centerX, float centerY, float orientation, float multiplSnake, float scaleForLittleOrBigFish){
    int frame = multiplSnake * (V.ANIMATION_FRAMES - 1);//Кадр анимации

    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.rotate(orientation,rotating_about);
    tempMatrix.scale(scaleForLittleOrBigFish);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW_A(SHARKBODY, frame);
}

void Widget::drawPatricle(float centerX, float centerY, float alpha){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    glUniform4f(uColorLocation, 1, 1, 1, alpha);
    DRAW(PARTICLE);
}

void Widget::setColor(float r, float g, float b, float alpha){
    glUniform4f(uColorLocation, r, g, b, alpha);
}

void Widget::drawSquareTransfered(float centerX, float centerY, float sizeScale, float orientation, float offsetX, float offsetY){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale);
    tempMatrix.rotate(orientation,rotating_about);
    tempMatrix.translate(offsetX/sizeScale,offsetY/sizeScale);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(SQUARE);
}

void Widget::drawLowpolyRoundTransfered(float centerX, float centerY, float radius, float orientation, float offsetX, float offsetY){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(radius);
    tempMatrix.rotate(orientation,rotating_about);
    tempMatrix.translate(offsetX/radius,offsetY/radius);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(LOWPOLY_ROUND);
}

void Widget::drawRoundedTriangleInCenterTransfered(float centerX, float centerY, float sizeScale, float orientation){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale);
    tempMatrix.rotate(orientation,rotating_about);
    tempMatrix.translate(2, 0);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(ROUNDED_TRIANGLE_INCENTER);
}

void Widget::drawRoundedTriangleOutCenterTransfered(float centerX, float centerY, float sizeScale, float orientation){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale);
    tempMatrix.rotate(orientation,rotating_about);
    tempMatrix.translate(0.538f, 0);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(ROUNDED_TRIANGLE_OUTCENTER);
}

void Widget::drawSquaresTransferedWithScale(float centerX, float centerY, float orientation, float deltaAngle, float scale){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.rotate(orientation,rotating_about);
    tempMatrix.translate(2, 16*0.7f);

    tempMatrix.rotate(deltaAngle,rotating_about);
    tempMatrix.translate(scale, 0);
    tempMatrix.scale(scale*2,3);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(SQUARE);
//Matrix.setIdentityM(mModelMatrix, 0);
//Matrix.translateM(mModelMatrix,0,centerX,centerY,0);
//Matrix.rotateM(mModelMatrix,0,orientation,0,0,1);
//Matrix.translateM(mModelMatrix,0,2,16*0.7f,0);

//Matrix.rotateM(mModelMatrix,0,deltaAngle,0,0,1);
//Matrix.translateM(mModelMatrix,0,scale,0,0);//50=100/2
//Matrix.scaleM(mModelMatrix,0,scale*2,3,1);//100
//bindMatrix();
//glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

    tempMatrix = mMatrix;
    tempMatrix.translate(centerX,centerY);
    tempMatrix.rotate(orientation,rotating_about);
    tempMatrix.translate(2, -16*0.7f);

    tempMatrix.rotate(-deltaAngle,rotating_about);
    tempMatrix.translate(scale, 0);
    tempMatrix.scale(scale*2,3);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(SQUARE);
//Matrix.setIdentityM(mModelMatrix, 0);
//Matrix.translateM(mModelMatrix,0,centerX,centerY,0);
//Matrix.rotateM(mModelMatrix,0,orientation,0,0,1);
//Matrix.translateM(mModelMatrix,0,2,-16*0.7f,0);

//Matrix.rotateM(mModelMatrix,0,-deltaAngle,0,0,1);
//Matrix.translateM(mModelMatrix,0,scale,0,0);//50=100/2
//Matrix.scaleM(mModelMatrix,0,scale*2,3,1);//100
//bindMatrix();
//glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
}

void Widget::drawBezier5(float centerX, float centerY, float sizeScale, float orientation, float multiplSnake, float changeLevelFoodRadius) {
    int frame;

    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    if (multiplSnake>=0.5){
        tempMatrix.scale(sizeScale);
        tempMatrix.rotate(orientation+90,rotating_about);
        frame = (multiplSnake-0.5f) * 2 * (V.ANIMATION_FRAMES - 1);//Кадр анимации
    } else {
        tempMatrix.scale(sizeScale, -sizeScale);
        tempMatrix.rotate(-orientation+90,rotating_about);
        frame = (1-multiplSnake*2) * (V.ANIMATION_FRAMES - 1);//Кадр анимации
    }

    tempMatrix.translate(-1, changeLevelFoodRadius+1);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW_A(BEZIER, frame);
}

void Widget::drawSharkmouthTransfered(float centerX, float centerY, float sizeScale, float orientation, float offsetX, float offsetY, float rotationForEating){
    QMatrix4x4 tempMatrix(mMatrix);
    tempMatrix.translate(centerX,centerY);
    tempMatrix.scale(sizeScale);
    tempMatrix.rotate(orientation,rotating_about);
    tempMatrix.translate(offsetX/sizeScale, offsetY/sizeScale);
    tempMatrix.rotate(rotationForEating,rotating_about);
    glUniformMatrix4fv(uMatrixLocation, 1, false, tempMatrix.constData());
    DRAW(SHARKMOUTH);
}
