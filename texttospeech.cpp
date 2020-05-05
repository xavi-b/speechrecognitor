#include "texttospeech.h"

TextToSpeech::TextToSpeech(QObject *parent)
    : QObject(parent)
{
    QLoggingCategory::setFilterRules(QStringLiteral("qt.speech.tts=true \n qt.speech.tts.*=true"));

    this->setEngine("default");
}

void TextToSpeech::speak(QString const& text)
{
    if(this->m_speech->state() == QTextToSpeech::Speaking)
        return;

    this->m_speech->say(text);
}
void TextToSpeech::stop()
{
    this->m_speech->stop();
}

void TextToSpeech::setRate(int rate)
{
    this->m_speech->setRate(rate / 10.0);
}

void TextToSpeech::setPitch(int pitch)
{
    this->m_speech->setPitch(pitch / 10.0);
}

void TextToSpeech::setVolume(int volume)
{
    this->m_speech->setVolume(volume / 100.0);
}

void TextToSpeech::setEngine(QString const& engineName)
{
    int rate = -1;
    int pitch = -1;
    int volume = -1;

    if(this->m_speech != nullptr)
    {
        rate = this->m_speech->rate();
        pitch = this->m_speech->pitch();
        volume = this->m_speech->volume();
    }

    delete this->m_speech;
    if (engineName == "default")
        this->m_speech = new QTextToSpeech(this);
    else
        this->m_speech = new QTextToSpeech(engineName, this);

    if(rate >= 0)
        this->m_speech->setRate(rate);
    if(pitch >= 0)
        this->m_speech->setPitch(pitch);
    if(volume >= 0)
        this->m_speech->setVolume(volume);

    connect(this->m_speech, &QTextToSpeech::stateChanged, this, &TextToSpeech::stateChangedSignal);

    emit availableLocalesChangedSignal(this->m_speech->availableLocales(), this->m_speech->locale());
}

void TextToSpeech::setLocale(QLocale const& locale)
{
    this->m_speech->setLocale(locale);

    emit availableVoicesChangedSignal(this->m_speech->availableVoices(), this->m_speech->voice());
}

void TextToSpeech::setVoice(const QVoice &voice)
{
    this->m_speech->setVoice(voice);
}
