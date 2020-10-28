#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //fmodf() Остаток

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
