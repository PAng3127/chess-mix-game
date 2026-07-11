QT       += core widgets

TARGET = chess
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

# 源文件
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    widgets/settingsdialog.cpp \
    widgets/gamebutton.cpp \
    games/gravitychess/gravityboardscene.cpp \
    games/gravitychess/gravityboardview.cpp \
    games/gravitychess/gravitypiece.cpp

# 头文件
HEADERS += \
    mainwindow.h \
    widgets/settingsdialog.h \
    widgets/gamebutton.h \
    games/gravitychess/gravityboardscene.h \
    games/gravitychess/gravityboardview.h \
    games/gravitychess/gravitypiece.h

# 包含路径
INCLUDEPATH += games/gravitychess widgets
