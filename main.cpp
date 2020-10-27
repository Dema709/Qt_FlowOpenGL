#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*float test;//canvasSize = (canvasSize + dt * 0.8) % 1;
    for (int i=0; i<100; i++)
    {
        float dt = 0.5;
        test += dt * 0.8;
        test = fmodf(test, 1.);
        qDebug()<<"Main i:"<<i<<(i+1)*0.5*0.8<<"->"<<test;
    }*/

    //Мультисэмплинг (антиалиасинг) = здесь + glEnable(GL_MULTISAMPLE);
    QGLFormat fmt;
    fmt.setSampleBuffers(true);
    fmt.setSamples(8);
    QGLFormat::setDefaultFormat(fmt);

    Widget w;
    w.resize(500, 500);
    //w.setGeometry(900, 40, 800, 800);
    w.show();
    return a.exec();
}
