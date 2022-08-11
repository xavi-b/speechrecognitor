#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QStatusBar>
#include <QPushButton>
#include "speechrecognitor.h"

class AudioLevel;

class AudioRecorder : public QWidget
{
    Q_OBJECT

public:
    AudioRecorder();

private slots:
    void toggleRecord();

    void updateStatus(QMediaRecorder::Status);
    void onStateChanged(QMediaRecorder::State);
    void updateProgress(qint64 pos);
    void displayErrorMessage();

private:
    void updateAudioLevels(const QList<qreal>& levels);

    XB::SpeechRecognitor* m_speechrecognitor = nullptr;

    QVBoxLayout* m_levelsLayout = nullptr;
    QStatusBar*  m_statusBar    = nullptr;
    QPushButton* m_recordButton = nullptr;

    QList<AudioLevel*> m_audioLevels;
};

#endif // AUDIORECORDER_H
