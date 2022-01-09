TEMPLATE = app

QT += multimedia widgets

HEADERS = \
    audiorecorder.h \
    audiolevel.h \
    awstranscribewrapper.h \
    deepspeechwrapper.h \
    googlespeechwrapper.h

SOURCES = \
    awstranscribewrapper.cpp \
    deepspeechwrapper.cpp \
    googlespeechwrapper.cpp \
    main.cpp \
    audiorecorder.cpp \
    audiolevel.cpp

FORMS += audiorecorder.ui

DEFINES += DEEPSPEECHDATA=\\\"$$PWD/third-party\\\"

LIBS += -L$$PWD/third-party/ -ldeepspeech
INCLUDEPATH += $$PWD/third-party/DeepSpeech/native_client

LIBS += -L$$PWD/third-party/lib \
    -laws-cpp-sdk-core \
    -laws-cpp-sdk-transcribestreaming
INCLUDEPATH += $$PWD/third-party/include

debug {
CONFIG += console
}
