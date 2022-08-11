#ifndef SPEECHRECOGNITOR_H
#define SPEECHRECOGNITOR_H

#include <QMediaRecorder>
#include <QUrl>
#include <QAudioProbe>
#include <QAudioRecorder>

namespace XB
{

class SpeechRecognitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<qreal> audioLevels READ audioLevels NOTIFY audioLevelsChanged)
    Q_PROPERTY(QString lang READ lang WRITE setLang NOTIFY langChanged)
public:
    SpeechRecognitor(QObject* parent = nullptr);

    void         record();
    void         stop();
    void         cancel();
    void         updateLanguages();
    QList<qreal> audioLevels() const;

    QString lang() const;
    void    setLang(QString const& lang);

    QMediaRecorder::Error  error() const;
    QString                errorString() const;
    QMediaRecorder::Status status() const;
    QMediaRecorder::State  state() const;

    QString resultLocation() const;

public slots:
    void processBuffer(const QAudioBuffer&);

signals:
    void resultString(QString const&);
    void error(QMediaRecorder::Error);
    void audioLevelsChanged(QList<qreal> const& audioLevels);
    void languagesChanged(QStringList const& languages);
    void progressUpdated(qint64 duration);
    void statusChanged(QMediaRecorder::Status status);
    void stateChanged(QMediaRecorder::State state);
    void langChanged(QString const&);

private slots:
    void processRecord();
    void processLanguages();
    void processLang(QString const& lang);

    void updateStatus(QMediaRecorder::Status);
    void updateProgress(qint64 pos);

private:
    void clearAudioLevels();

    QString         m_lang          = "en";
    QAudioRecorder* m_audioRecorder = nullptr;
    QAudioProbe*    m_probe         = nullptr;
    QList<qreal>    m_audioLevels;
    qint64          m_silentStartTime = 0;
};

} // namespace XB

#endif // SPEECHRECOGNITOR_H
