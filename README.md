# Flow OpenGl
Разработка игры (похожей на Flow от Sony) с использованием Qt и OpenGL. Изначально написана на Java под Android (https://github.com/Dema709/Android_FlowOpenGL14API), поэтому иногда в коде встречаются старые комментарии и объявление чисел в виде 0.1f.

# Сторонние источники (библиотеки)
https://github.com/effolkronium/random

# Standalone .exe
Qt5Core.dll
Qt5Gui.dll
Qt5OpenGL.dll
Qt5Widgets.dll
libgcc_s_seh-1.dll
libstdc++-6.dll
libwinpthread-1.dll
platforms/qwindows.dll

#.pro file
QT += opengl
LIBS += -lopengl32 или opengl32	//Цитата из интернета: Это базовая библиотека OpenGl, Qt ее только использует, а базовые функции содержатся в opengl32.lib.