#include "audiorecordersettingswidget.h"

AudioRecorderSettingsWidget::AudioRecorderSettingsWidget(QWidget *parent) : QWidget(parent)
{
    mainLayout = new QGridLayout();
    gridLayout_2 = new QGridLayout();
    label = new QLabel();

    gridLayout_2->addWidget(label, 0, 0, 1, 1);

    label_2 = new QLabel();

    gridLayout_2->addWidget(label_2, 1, 0, 1, 1);

    audioDeviceBox = new QComboBox();

    gridLayout_2->addWidget(audioDeviceBox, 0, 1, 1, 1);

    containerBox = new QComboBox();

    gridLayout_2->addWidget(containerBox, 2, 1, 1, 1);

    audioCodecBox = new QComboBox();

    gridLayout_2->addWidget(audioCodecBox, 1, 1, 1, 1);

    label_4 = new QLabel();

    gridLayout_2->addWidget(label_4, 3, 0, 1, 1);

    label_3 = new QLabel();

    gridLayout_2->addWidget(label_3, 2, 0, 1, 1);

    sampleRateBox = new QComboBox();

    gridLayout_2->addWidget(sampleRateBox, 3, 1, 1, 1);

    verticalLayout = new QVBoxLayout();
    groupBox = new QGroupBox();
    gridLayout = new QGridLayout(groupBox);
    constantQualityRadioButton = new QRadioButton(groupBox);
    constantQualityRadioButton->setChecked(true);

    gridLayout->addWidget(constantQualityRadioButton, 0, 0, 1, 2);

    qualitySlider = new QSlider(groupBox);
    qualitySlider->setOrientation(Qt::Horizontal);

    gridLayout->addWidget(qualitySlider, 1, 1, 1, 1);

    constantBitrateRadioButton = new QRadioButton(groupBox);

    gridLayout->addWidget(constantBitrateRadioButton, 2, 0, 1, 2);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

    gridLayout->addItem(horizontalSpacer, 3, 0, 1, 1);

    bitrateBox = new QComboBox(groupBox);
    bitrateBox->setEnabled(false);

    gridLayout->addWidget(bitrateBox, 3, 1, 1, 1);


    verticalLayout->addWidget(groupBox);

    stopButton = new QPushButton();

    verticalLayout->addWidget(stopButton);

    levelLabel = new QLabel();

    verticalLayout->addWidget(levelLabel);

    levelsLayout = new QVBoxLayout();

    verticalLayout->addLayout(levelsLayout);

    gridLayout_2->addLayout(verticalLayout, 4, 0, 1, 2);

    mainLayout->addLayout(gridLayout_2, 0, 0, 1, 3);

    statusbar = new QStatusBar;
    mainLayout->addWidget(statusbar);

    this->setLayout(this->mainLayout);

    retranslateUi();

    //quality
    qualitySlider->setRange(0, int(QMultimedia::VeryHighQuality));
    qualitySlider->setValue(int(QMultimedia::NormalQuality));

    //bitrates:
    bitrateBox->addItem(tr("Default"), QVariant(0));
    bitrateBox->addItem(QStringLiteral("32000"), QVariant(32000));
    bitrateBox->addItem(QStringLiteral("64000"), QVariant(64000));
    bitrateBox->addItem(QStringLiteral("96000"), QVariant(96000));
    bitrateBox->addItem(QStringLiteral("128000"), QVariant(128000));

    QObject::connect(this->constantQualityRadioButton, SIGNAL(toggled(bool)), this->qualitySlider, SLOT(setEnabled(bool)));
    QObject::connect(this->constantBitrateRadioButton, SIGNAL(toggled(bool)), this->bitrateBox, SLOT(setEnabled(bool)));

    QObject::connect(this->stopButton, &QPushButton::clicked, this, &AudioRecorderSettingsWidget::stopSignal);

    connect(audioDeviceBox, qOverload<int>(&QComboBox::currentIndexChanged), this, [=](int index)
    {
        emit audioInputChangedSignal(audioDeviceBox->currentData().toString());
    });
    connect(audioCodecBox, qOverload<int>(&QComboBox::currentIndexChanged), this, [=](int index)
    {
        emit codecChangedSignal(audioCodecBox->currentData().toString());
    });
    connect(containerBox, qOverload<int>(&QComboBox::currentIndexChanged), this, [=](int index)
    {
        emit containerChangedSignal(containerBox->currentData().toString());
    });
    connect(sampleRateBox, qOverload<int>(&QComboBox::currentIndexChanged), this, [=](int index)
    {
        emit sampleRateSignal(sampleRateBox->currentData().toInt());
    });
    connect(qualitySlider, &QSlider::valueChanged, this, [=](int value)
    {
        emit qualityChangedSignal(qualitySlider->value());
    });
    connect(constantQualityRadioButton, &QRadioButton::toggled, this, [=]()
    {
        emit encodingModeChangedSignal(constantQualityRadioButton->isChecked() ?
                                           QMultimedia::ConstantQualityEncoding :
                                           QMultimedia::ConstantBitRateEncoding);
    });
}

void AudioRecorderSettingsWidget::retranslateUi()
{
    label->setText(tr("Input Device:"));
    label_2->setText(tr("Audio Codec:"));
    label_4->setText(tr("Sample rate:"));
    label_3->setText(tr("File Container:"));
    groupBox->setTitle(tr("Encoding Mode:"));
    constantQualityRadioButton->setText(tr("Constant Quality:"));
    constantBitrateRadioButton->setText(tr("Constant Bitrate:"));
    stopButton->setText(tr("Stop"));
    levelLabel->setText(tr("Audio Level:"));
}

void AudioRecorderSettingsWidget::setAudioInputs(QStringList const& audioInputs)
{
    this->audioDeviceBox->addItem(tr("Default"), QVariant(QString()));
    for (auto &device: audioInputs)
        this->audioDeviceBox->addItem(device, QVariant(device));
}

void AudioRecorderSettingsWidget::setAudioCodecs(QStringList const& codecs)
{
    this->audioCodecBox->addItem(tr("Default"), QVariant(QString()));
    for (auto &codecName: codecs)
        this->audioCodecBox->addItem(codecName, QVariant(codecName));
}

void AudioRecorderSettingsWidget::setContainers(QStringList const& containers)
{
    this->containerBox->addItem(tr("Default"), QVariant(QString()));
    for (auto &containerName: containers)
        this->containerBox->addItem(containerName, QVariant(containerName));
}

void AudioRecorderSettingsWidget::setAudioSampleRates(QList<int> const& samplesRates)
{
    this->sampleRateBox->addItem(tr("Default"), QVariant(0));
    for (int sampleRate: samplesRates)
        this->sampleRateBox->addItem(QString::number(sampleRate), QVariant(sampleRate));
}

void AudioRecorderSettingsWidget::updateProgress(qint64 duration)
{
    if (duration < 2000)
        return;

    this->statusbar->showMessage(tr("Recorded %1 sec").arg(duration / 1000));
}

void AudioRecorderSettingsWidget::updateStatus(QString const& status)
{
    this->statusbar->showMessage(status);
}
