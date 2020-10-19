#include "widget.h"
#include <QDebug>

#include <QtGui>

//#include <QOpenGLFunctions>
#include <QtWidgets>
#include <QtOpenGL>

Widget::Widget(QWidget *parent)
    : QGLWidget(parent)
{
    qDebug()<<"Widget constructor";
    VertexLoader vertexLoader;
    V = vertexLoader.getVertex();
    vertices = vertexLoader.getVerticles();
}

Widget::~Widget()
{
    qDebug()<<"Widget destructor";
}

    /*
    //C хабра, без ошибок, но только один цвет
    const GLchar* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 position;\n"
        "void main()\n"
        "{\n"
        "gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
        "}\0";


    //Работает с 2 точками на плоскости
    const GLchar* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec2 position;\n"
        "void main()\n"
        "{\n"
        "gl_Position = vec4(position.x, position.y, 0.0, 1.0);\n"
        "}\0";
    */

    //Тест
    const GLchar* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec2 position;\n"
        "uniform highp mat4 u_Matrix;\n"
        "void main()\n"
        "{\n"
        "gl_Position ="
                                      " u_Matrix*"
            "vec4(position.x, position.y, 0.0, 1.0);\n"
        "}\0";

    /*
    //C хабра, без ошибок, но только один цвет
    const GLchar* fragmentShaderSource = "#version 330 core\n"
        "out vec4 color;\n"
        "void main()\n"
        "{\n"
        "color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n\0";

    //Компилится с ошибками, но работает как надо
     const GLchar* fragmentShaderSource =
             "varying lowp vec4 col;\n"
             "void main() {\n"
             "   gl_FragColor = col;\n"
             "}\n";

    //Работает без ошибок, но проблема с растягиванием
    const GLchar* fragmentShaderSource =
            "#version 330 core\n"
            "uniform vec4 in_Color;\n"
            "void main()\n"
            "{\n"
            "    gl_FragColor = in_Color;\n"
            "}\n";
*/
    //Тест. Без ошибок
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

    vertices = {
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
    };

    //#if HABR
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
        QMatrix4x4 mMatrix;
        glUniformMatrix4fv(uMatrixLocation, 1, false, mMatrix.constData());//Матрица по умолчанию

        qDebug()<<"initializeGL end";
};
void Widget::paintGL(){
    qDebug()<<"paintGL";

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


    glClearColor(0.8,0.8,1, 0);//Цвет фона
    glClear(GL_COLOR_BUFFER_BIT);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();*/
 /*
    glScalef(nSca, nSca, nSca);
    glTranslatef(0.0f, zTra, 0.0f);//Меняет положение камеры!
    glRotatef(xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    glRotatef(zRot, 0.0f, 0.0f, 1.0f);*/

    glLineWidth(3.0f);
    glUniform4f(uColorLocation, 1, 0, 0, 1);//Красная ось x
    glDrawArrays(GL_LINES, 3, 2);

    glUniform4f(uColorLocation, 0, 1, 0, 1);//Зелёная ось y
    glDrawArrays(GL_LINES, 5, 2);

    glUniform4f(uColorLocation, 0, 0, 1, 0.5);//Треугольник
    glDrawArrays(GL_TRIANGLES, 0, 3);


};
void Widget::resizeGL(int width, int height){
    qDebug()<<"resizeGL with (width ="<<width<<"; height ="<<height<<")";
/*
        QMatrix4x4 mMatrix;
        mMatrix.setToIdentity();
        mMatrix.ortho(-1, 1, -1, 1, -1, 40);*/
/*
        qreal top, bottom, right, left, aratio;
        //left = -1; right = 1; bottom = -1; top = 1;

        aratio = height / width;
        if (width > height)
            {
                top = 3.0f;
                bottom = -top;
                right = top * aratio;
                left = -right;
            }
            else
        {
            right = 3.0f;
            left = -right;
            top = right / aratio;
            bottom = -top;
        }
        qDebug()<<left<<right<<bottom<<top<<aratio;
        mMatrix.ortho(left, right, bottom, top, -1, 40);*/

        /*
        // Calculate aspect ratio
        qreal aspect = qreal(width) / qreal(height ? height : 1);
        // Set near plane to -1.0, far plane to 7.0, field of view 45 degrees
        const qreal zNear = -1.0, zFar = 7.0, fov = 45.0;
        // Reset projection
        mMatrix.setToIdentity();
        // Set perspective projection
        mMatrix.perspective(fov, aspect, zNear, zFar);
        */

        //mMatrix.lookAt({0,0,3},{0,0,0},{0,10,0});

        //mMatrix.ortho(QRect{1,-1,1,1});
        //glUniformMatrix4fv(uMatrixLocation, 1, false, mMatrix.constData());
    /*
    QMatrix4x4 mMatrix;
    mMatrix.translate(0,0);
    //mMatrix.perspective(0,static_cast<double>(width)/height,-5,5);
    //mMatrix.lookAt({0,0,3},{0,0,0},{0,1,0});

    glUniformMatrix4fv(uMatrixLocation, 1, false, mMatrix.constData());
*/

    /*glMatrixMode(GL_PROJECTION);        //Без этого не растягивается
    glLoadIdentity();                   //Без этого не работает
    GLfloat ratio=(GLfloat)height/(GLfloat)width;

    if (width>=height)
       glOrtho(-1.0/ratio, 1.0/ratio, -1.0, 1.0, -10.0, 1.0);
    else
       glOrtho(-1.0, 1.0, -1.0*ratio, 1.0*ratio, -10.0, 1.0);
*/
    QMatrix4x4 mMatrix;
    //mMatrix.scale(0.5);
    mMatrix.translate(0,0.5);

    glUniformMatrix4fv(uMatrixLocation, 1, false, mMatrix.constData());

    glViewport(0, 0, (GLint)width, (GLint)height);
};


/*
https://stackoverflow.com/questions/42359575/implementing-a-fragment-shader-that-behaves-similar-to-glcolor4f

GLint uColorLocation;//"in_Color"
uColorLocation = glGetUniformLocation(shaderProgram, "in_Color");
glUniform4f(uColorLocation, 0, 0, 0, 0.5);

При этом шейдер:
uniform vec4 in_Color;
void main () {
  gl_FragColor = in_Color;
}
*/


/*
java
uColorLocation = glGetUniformLocation(programId, "u_Color");
glUniform4f(uColorLocation, 0.0f, 0.0f, 1.0f, 0.5f);

precision mediump float;
uniform vec4 u_Color;
//Фрагментный шейдер (?)
void main(){
    gl_FragColor = u_Color;
}
*/
