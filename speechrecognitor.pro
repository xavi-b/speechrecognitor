TEMPLATE = app

QT += multimedia widgets

HEADERS = \
    audiorecorder.h \
    audiolevel.h \
    deepspeechwrapper.h \
    googlespeechwrapper.h

SOURCES = \
    deepspeechwrapper.cpp \
    googlespeechwrapper.cpp \
    main.cpp \
    audiorecorder.cpp \
    audiolevel.cpp

FORMS += audiorecorder.ui

LIBS += -ldeepspeech
