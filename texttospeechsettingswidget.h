#ifndef TEXTTOSPEECHSETTINGSWIDGET_H
#define TEXTTOSPEECHSETTINGSWIDGET_H

#include <QVariant>
#include <QApplication>
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSlider>
#include <QSpacerItem>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QTextToSpeech>

class TextToSpeechSettingsWidget : public QWidget
{
    Q_OBJECT
private:
    QVBoxLayout* verticalLayout;
    QGridLayout* gridLayout;
    QLabel* label_5;
    QLabel* label_3;
    QLabel* label_4;
    QSlider* pitch;
    QLabel* label_6;
    QSlider* volume;
    QComboBox* language;
    QComboBox* voiceComboBox;
    QLabel* label;
    QLabel* label_2;
    QSlider* rate;
    QComboBox* engineComboBox;
    QHBoxLayout* horizontalLayout;
    QPushButton* stopButton;
    QSpacerItem* verticalSpacer;
    QStatusBar* statusbar;

    QVector<QVoice> m_voices;

public:
    TextToSpeechSettingsWidget(QWidget* parent = nullptr);

    void retranslateUi();
    void setLocales(QVector<QLocale> const& locales, QLocale const& currentLocale);
    void setVoices(const QVector<QVoice> &voices, const QVoice &currentVoice);
    void stateChangedSlot(QTextToSpeech::State state);

signals:
    void pitchChangedSignal(int);
    void rateChangedSignal(int);
    void volumeChangedSignal(int);
    void engineChangedSignal(QString const& engine);
    void localeChangedSignal(QLocale const& locale);
    void voiceChangedSignal(QVoice const& voiceComboBox);
};

#endif // TEXTTOSPEECHSETTINGSWIDGET_H
