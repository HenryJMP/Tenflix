QT += core gui widgets multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Tenflix
TEMPLATE = app

SOURCES += \
    main.cpp \
    Pelis.cpp \
    Serie.cpp \
    Cap.cpp \
    Videos.cpp

HEADERS += \
    Pelis.hpp \
    Serie.hpp \
    Cap.hpp \
    Videos.hpp

CONFIG += link_pkgconfig
PKGCONFIG += jsoncpp

QMAKE_CXXFLAGS += -std=c++11