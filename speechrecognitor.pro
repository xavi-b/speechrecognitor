TEMPLATE = app

QT += core multimedia widgets opengl texttospeech

HEADERS = \
    audiorecorder.h \
    audiolevel.h \
    audiorecordersettingswidget.h \
    audiowavewidget.h \
    deepspeechwrapper.h \
    texttospeech.h \
    texttospeechsettingswidget.h

SOURCES = \
    audiorecordersettingswidget.cpp \
    audiowavewidget.cpp \
    deepspeechwrapper.cpp \
    main.cpp \
    audiorecorder.cpp \
    audiolevel.cpp \
    texttospeech.cpp \
    texttospeechsettingswidget.cpp

FORMS +=

LIBS += -L$$PWD/tensorflow/bazel-bin/native_client -ldeepspeech
INCLUDEPATH += $$PWD/DeepSpeech/native_client
DEPENDPATH += $$PWD/DeepSpeech/native_client
