#include "audiorecorder.h"
#include "texttospeech.h"
#include "audiorecordersettingswidget.h"
#include "texttospeechsettingswidget.h"

#include "audiowavewidget.h"

#include <QtWidgets>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    AudioRecorder recorder;
    TextToSpeech tts;

    AudioRecorderSettingsWidget recorderWidget;
    recorderWidget.show();
    TextToSpeechSettingsWidget ttsWidget;
    ttsWidget.show();

    QObject::connect(&tts, &TextToSpeech::stateChangedSignal, &ttsWidget, &TextToSpeechSettingsWidget::stateChangedSlot);
    QObject::connect(&tts, &TextToSpeech::availableLocalesChangedSignal, &ttsWidget, &TextToSpeechSettingsWidget::setLocales);
    QObject::connect(&tts, &TextToSpeech::availableVoicesChangedSignal, &ttsWidget, &TextToSpeechSettingsWidget::setVoices);

    QObject::connect(&ttsWidget, &TextToSpeechSettingsWidget::pitchChangedSignal, &tts, &TextToSpeech::setPitch);
    QObject::connect(&ttsWidget, &TextToSpeechSettingsWidget::rateChangedSignal, &tts, &TextToSpeech::setRate);
    QObject::connect(&ttsWidget, &TextToSpeechSettingsWidget::volumeChangedSignal, &tts, &TextToSpeech::setVolume);
    QObject::connect(&ttsWidget, &TextToSpeechSettingsWidget::engineChangedSignal, &tts, &TextToSpeech::setEngine);
    QObject::connect(&ttsWidget, &TextToSpeechSettingsWidget::localeChangedSignal, &tts, &TextToSpeech::setLocale);
    QObject::connect(&ttsWidget, &TextToSpeechSettingsWidget::voiceChangedSignal, &tts, &TextToSpeech::setVoice);

    recorderWidget.setAudioInputs(recorder.getAudioInputs());
    recorderWidget.setAudioCodecs(recorder.getAudioCodecs());
    recorderWidget.setContainers(recorder.getContainers());
    recorderWidget.setAudioSampleRates(recorder.getAudioSampleRates());
    QObject::connect(&recorder, &AudioRecorder::log, [=](QString const& log)
    {
        qDebug() << log;
    });
    QObject::connect(&recorder, &AudioRecorder::error, [=](QString const& error)
    {
        qDebug() << error;
    });
    QObject::connect(&recorder, &AudioRecorder::result, [&](QString const& result)
    {
        tts.speak(result);
    });
    QObject::connect(&recorder, &AudioRecorder::statusUpdated, &recorderWidget, &AudioRecorderSettingsWidget::updateStatus);
    QObject::connect(&recorder, &AudioRecorder::durationChanged, &recorderWidget, &AudioRecorderSettingsWidget::updateProgress);

    QObject::connect(&recorderWidget, &AudioRecorderSettingsWidget::audioInputChangedSignal, &recorder, &AudioRecorder::setAudioInput);
    QObject::connect(&recorderWidget, &AudioRecorderSettingsWidget::codecChangedSignal, &recorder, &AudioRecorder::setCodec);
    QObject::connect(&recorderWidget, &AudioRecorderSettingsWidget::containerChangedSignal, &recorder, &AudioRecorder::setContainer);
    QObject::connect(&recorderWidget, &AudioRecorderSettingsWidget::sampleRateSignal, &recorder, &AudioRecorder::setSampleRate);
    QObject::connect(&recorderWidget, &AudioRecorderSettingsWidget::qualityChangedSignal, &recorder, &AudioRecorder::setQuality);
    QObject::connect(&recorderWidget, &AudioRecorderSettingsWidget::encodingModeChangedSignal, &recorder, &AudioRecorder::setEncodingMode);
    QObject::connect(&recorderWidget, &AudioRecorderSettingsWidget::stopSignal, [&]()
    {
        recorder.stop();
    });

    AudioWaveWidget waveWidget;
    waveWidget.setFixedSize(200, 80);
    waveWidget.show();

    QPushButton recordButton("Record");
    recordButton.show();

    QObject::connect(&recordButton, &QPushButton::clicked, &recorder, &AudioRecorder::record);
    QObject::connect(&recordButton, &QPushButton::clicked, &waveWidget, &AudioWaveWidget::start);
    QObject::connect(&recorder, &AudioRecorder::stopSignal, &waveWidget, &AudioWaveWidget::stop);

    return app.exec();
}
