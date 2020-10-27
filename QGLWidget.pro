QT       += core gui
QT += opengl
LIBS += -lopengl32

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Camera.cpp \
    ChangeLevelFood.cpp \
    Food.cpp \
    Level.cpp \
    Protagonist.cpp \
    Segment.cpp \
    VertexLoader.cpp \
    main.cpp \
    particle.cpp \
    widget.cpp

HEADERS += \
    Camera.h \
    ChangeLevelFood.h \
    Food.h \
    Level.h \
    Protagonist.h \
    Segment.h \
    VertexLoader.h \
    particle.h \
    random.hpp \
    widget.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
