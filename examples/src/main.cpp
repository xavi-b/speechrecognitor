#include "audiorecorder.h"

#include <QtWidgets>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    AudioRecorder recorder;
    recorder.show();

    return app.exec();
}
