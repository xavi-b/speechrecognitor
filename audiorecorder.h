#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include <QMainWindow>
#include <QMediaRecorder>
#include <QStatusBar>
#include <QComboBox>
#include "deepspeechwrapper.h"
#include "googlespeechwrapper.h"

namespace Ui
{
class AudioRecorder;
}
class QAudioRecorder;
class QAudioProbe;
class QAudioBuffer;

class AudioLevel;

class AudioRecorder : public QMainWindow
{
    Q_OBJECT

public:
    AudioRecorder();

public slots:
    void processBuffer(const QAudioBuffer&);

    void sendToDeepSpeech(QByteArray const& audio);
    void sendToGoogle(QByteArray const& audio);

private slots:
    void setOutputLocation();
    void togglePause();
    void toggleRecord();

    void updateStatus(QMediaRecorder::Status);
    void onStateChanged(QMediaRecorder::State);
    void updateProgress(qint64 pos);
    void displayErrorMessage();

private:
    void clearAudioLevels();

    Ui::AudioRecorder* ui = nullptr;

    QAudioRecorder*    m_audioRecorder = nullptr;
    QAudioProbe*       m_probe         = nullptr;
    QList<AudioLevel*> m_audioLevels;
    bool               m_outputLocationSet = false;

    DeepSpeechWrapper*   deepSpeechWrapper;
    GoogleSpeechWrapper* googleSpeechWrapper;
};

#endif // AUDIORECORDER_H
