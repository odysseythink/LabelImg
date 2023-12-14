QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    api.pb.cc \
    common.cpp \
    go_interface.cpp \
    libs/canvas.cpp \
    libs/label_dialog.cpp \
    libs/color_dialog.cpp \
    libs/labelfile.cpp \
    libs/labels_widget.cpp \
    libs/shape.cpp \
    libs/toolbar.cpp \
    main.cpp \
    main_win.cpp \
    settings.cpp \
    libs/zoom_widget.cpp

HEADERS += \
    api.pb.h \
    common.h \
    go_interface.h \
    libs/canvas.h \
    libs/label_dialog.h \
    libs/color_dialog.h \
    libs/labelfile.h \
    libs/labels_widget.h \
    libs/shape.h \
    libs/toolbar.h \
    main_win.h \
    settings.h \
    libs/zoom_widget.h

FORMS += \
    libs/label_dialog.ui \
    libs/labels_widget.ui \
    main_win.ui

TRANSLATIONS += \
    LabelImg_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc


unix: LIBS += "../go_label_img/liblabelImg.a" -ldl -lresolv
else: LIBS += "../go_label_img/liblabelImg.a"

LIBS += -static -lprotobuf
