#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include <QMainWindow>
#include <QMediaRecorder>
#include <QAudioProbe>
#include <QAudioRecorder>
#include <QDir>
#include <QMediaRecorder>
#include <QStandardPaths>

#include "deepspeechwrapper.h"

class QAudioRecorder;
class QAudioProbe;
class QAudioBuffer;

class AudioLevel;

class AudioRecorder : public QObject
{
    Q_OBJECT

public:
    AudioRecorder(QObject* parent = nullptr);

    QStringList getAudioInputs() const;
    QStringList getAudioCodecs() const;
    QStringList getContainers() const;
    QList<int> getAudioSampleRates() const;

    void setAudioInput(QString const& audioInput);
    void setCodec(QString const& codec);
    void setContainer(QString const& container);
    void setSampleRate(int sampleRate);
    void setQuality(int quality);
    void setEncodingMode(bool constantQuality);

    void processBuffer(QAudioBuffer const&);
    void record();
    void stop(bool sendToDeepSpeech = true);
    void sendToDeepSpeech(QByteArray const& audio);

private:
    void updateStatus(QMediaRecorder::Status);
    void clearAudioLevels();

    QAudioEncoderSettings settings;
    QAudioRecorder* m_audioRecorder = nullptr;
    QAudioProbe* m_probe = nullptr;
    QVector<qreal> m_audioLevels;
    QString container;

    DeepSpeechWrapper* deepSpeechWrapper;

signals:
    void log(QString const&);
    void error(QString const&);
    void result(QString const&);
    void statusUpdated(QString const&);
    void durationChanged(int);
    void stopSignal();
};

#endif // AUDIORECORDER_H
