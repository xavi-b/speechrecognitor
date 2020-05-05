#ifndef TESTTOSPEECH_H
#define TESTTOSPEECH_H

#include <QObject>
#include <QTextToSpeech>
#include <QLoggingCategory>

class TextToSpeech : public QObject
{
    Q_OBJECT
public:
    TextToSpeech(QObject* parent = nullptr);

public slots:
    void speak(QString const& text);
    void stop();

    void setRate(int);
    void setPitch(int);
    void setVolume(int volume);
    void setEngine(QString const& engine);
    void setLocale(QLocale const& locale);
    void setVoice(QVoice const& voice);

private:
    QTextToSpeech* m_speech = nullptr;

signals:
    void stateChangedSignal(QTextToSpeech::State state);
    void availableLocalesChangedSignal(QVector<QLocale> const& locales, QLocale const& currentLocale);
    void availableVoicesChangedSignal(QVector<QVoice> const& voices, QVoice const& currentVoice);

};

#endif // TESTTOSPEECH_H
