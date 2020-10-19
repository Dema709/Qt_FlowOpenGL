#Flow OpenGl
Разработка игры (похожей на Flow от Sony) с использованием Qt и OpenGL

# Standalone .exe
Qt5Core.dll
Qt5Gui.dll
libgcc_s_seh-1.dll
libstdc++-6.dll
libwinpthread-1.dll
platforms/qwindows.dll

#.pro file
QT += opengl
LIBS += -lopengl32 или opengl32	//Цитата из интернета: Это базовая библиотека OpenGl, Qt ее только использует, а базовые функции содержатся в opengl32.lib.