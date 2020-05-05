#ifndef AUDIORECORDERSETTINGSWIDGET_H
#define AUDIORECORDERSETTINGSWIDGET_H

#include <QVariant>
#include <QApplication>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpacerItem>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QMediaRecorder>

class AudioRecorderSettingsWidget : public QWidget
{
    Q_OBJECT
private:
    QGridLayout* mainLayout;
    QGridLayout* gridLayout_2;
    QLabel* label;
    QLabel* label_2;
    QComboBox* audioDeviceBox;
    QComboBox* containerBox;
    QComboBox* audioCodecBox;
    QLabel* label_4;
    QLabel* label_3;
    QComboBox* sampleRateBox;
    QVBoxLayout* verticalLayout;
    QGroupBox* groupBox;
    QGridLayout* gridLayout;
    QRadioButton* constantQualityRadioButton;
    QSlider* qualitySlider;
    QRadioButton* constantBitrateRadioButton;
    QSpacerItem* horizontalSpacer;
    QComboBox* bitrateBox;
    QPushButton* stopButton;
    QLabel* levelLabel;
    QVBoxLayout* levelsLayout;
    QStatusBar* statusbar;

public:
    AudioRecorderSettingsWidget(QWidget* parent = nullptr);
    void retranslateUi();

    void setAudioInputs(QStringList const& audioInputs);
    void setAudioCodecs(QStringList const& codecs);
    void setContainers(QStringList const& containers);
    void setAudioSampleRates(QList<int> const& sampleRates);
    void updateProgress(qint64 duration);
    void updateStatus(QString const& status);

signals:
    void audioInputChangedSignal(QString const&);
    void codecChangedSignal(QString const&);
    void containerChangedSignal(QString const&);
    void sampleRateSignal(int);
    void qualityChangedSignal(int);
    void encodingModeChangedSignal(bool);
    void stopSignal();

};

#endif // AUDIORECORDERSETTINGSWIDGET_H
