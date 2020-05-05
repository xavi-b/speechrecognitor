#include "texttospeechsettingswidget.h"

TextToSpeechSettingsWidget::TextToSpeechSettingsWidget(QWidget *parent) : QWidget(parent)
{
    verticalLayout = new QVBoxLayout();

    gridLayout = new QGridLayout();
    label_5 = new QLabel();
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
    label_5->setSizePolicy(sizePolicy1);
    label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_5, 4, 0, 1, 1);

    label_3 = new QLabel();
    sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
    label_3->setSizePolicy(sizePolicy1);
    label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_3, 3, 0, 1, 1);

    label_4 = new QLabel();
    sizePolicy1.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
    label_4->setSizePolicy(sizePolicy1);
    label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_4, 5, 0, 1, 1);

    pitch = new QSlider();
    pitch->setMinimum(-10);
    pitch->setMaximum(10);
    pitch->setSingleStep(1);
    pitch->setOrientation(Qt::Horizontal);

    gridLayout->addWidget(pitch, 3, 2, 1, 1);

    label_6 = new QLabel();
    label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_6, 6, 0, 1, 1);

    volume = new QSlider();
    volume->setMaximum(100);
    volume->setSingleStep(5);
    volume->setPageStep(20);
    volume->setValue(70);
    volume->setOrientation(Qt::Horizontal);

    gridLayout->addWidget(volume, 1, 2, 1, 1);

    language = new QComboBox();
    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(language->sizePolicy().hasHeightForWidth());
    language->setSizePolicy(sizePolicy2);

    gridLayout->addWidget(language, 5, 2, 1, 1);

    voiceComboBox = new QComboBox();

    gridLayout->addWidget(voiceComboBox, 6, 2, 1, 1);

    label = new QLabel();
    sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
    label->setSizePolicy(sizePolicy1);
    label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label, 2, 0, 1, 1);

    label_2 = new QLabel();
    sizePolicy1.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
    label_2->setSizePolicy(sizePolicy1);
    label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_2, 1, 0, 1, 1);

    rate = new QSlider();
    rate->setMinimum(-10);
    rate->setMaximum(10);
    rate->setOrientation(Qt::Horizontal);

    gridLayout->addWidget(rate, 2, 2, 1, 1);

    engineComboBox = new QComboBox();
    sizePolicy2.setHeightForWidth(engineComboBox->sizePolicy().hasHeightForWidth());
    engineComboBox->setSizePolicy(sizePolicy2);

    gridLayout->addWidget(engineComboBox, 4, 2, 1, 1);

    verticalLayout->addLayout(gridLayout);

    horizontalLayout = new QHBoxLayout();

    stopButton = new QPushButton();

    horizontalLayout->addWidget(stopButton);

    verticalLayout->addLayout(horizontalLayout);

    statusbar = new QStatusBar();

    verticalLayout->addWidget(statusbar);

    this->setLayout(this->verticalLayout);

#ifndef QT_NO_SHORTCUT
    label_4->setBuddy(language);
#endif // QT_NO_SHORTCUT

    retranslateUi();

    this->engineComboBox->addItem("Default", QString("default"));

    auto const& engines = QTextToSpeech::availableEngines();
    for (QString const& engine : engines)
        this->engineComboBox->addItem(engine, engine);

    connect(this->engineComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, [=](int index)
    {
        emit engineChangedSignal(this->engineComboBox->currentData().toString());
    });

    engineComboBox->setCurrentIndex(0);

    connect(pitch, &QSlider::valueChanged, this, &TextToSpeechSettingsWidget::pitchChangedSignal);
    connect(rate, &QSlider::valueChanged, this, &TextToSpeechSettingsWidget::rateChangedSignal);
    connect(volume, &QSlider::valueChanged, this, &TextToSpeechSettingsWidget::volumeChangedSignal);
}

void TextToSpeechSettingsWidget::retranslateUi()
{
    label_5->setText(tr("Engine"));
    label_3->setText(tr("Pitch:"));
    label_4->setText(tr("&Language:"));
    label_6->setText(tr("Voice name:"));
    label->setText(tr("Rate:"));
    label_2->setText(tr("Volume:"));
    stopButton->setText(tr("Stop"));
}

void TextToSpeechSettingsWidget::setLocales(QVector<QLocale> const& locales, QLocale const& currentLocale)
{
    disconnect(language, qOverload<int>(&QComboBox::currentIndexChanged), this, nullptr);
    language->clear();

    int index = 0;
    for (const QLocale &locale : locales)
    {
        QString name(QString("%1 (%2)")
                     .arg(QLocale::languageToString(locale.language()))
                     .arg(QLocale::countryToString(locale.country())));

        QVariant localeVariant(locale);

        language->addItem(name, localeVariant);

        if (locale.name() == currentLocale.name())
            index = language->count() - 1;
    }

    language->setCurrentIndex(index);

    connect(language, qOverload<int>(&QComboBox::currentIndexChanged), this, [=](int index)
    {
        emit localeChangedSignal(this->language->currentData().toLocale());
    });

    emit localeChangedSignal(this->language->currentData().toLocale());
}

void TextToSpeechSettingsWidget::setVoices(QVector<QVoice> const& voices, QVoice const& currentVoice)
{
    disconnect(voiceComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, nullptr);
    voiceComboBox->clear();

    this->m_voices = voices;

    int index = 0;
    for (const QVoice &voice : voices)
    {
        voiceComboBox->addItem(QString("%1 - %2 - %3").arg(voice.name())
                          .arg(QVoice::genderName(voice.gender()))
                          .arg(QVoice::ageName(voice.age())));

        if (voice.name() == currentVoice.name())
            index = voiceComboBox->count() - 1;
    }

    voiceComboBox->setCurrentIndex(index);

    connect(voiceComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, [=](int index)
    {
        emit voiceChangedSignal(this->m_voices.at(index));
    });

    emit voiceChangedSignal(this->m_voices.at(index));
}

void TextToSpeechSettingsWidget::stateChangedSlot(QTextToSpeech::State state)
{
    if (state == QTextToSpeech::Speaking)
        statusbar->showMessage("Speech started...");
    else if (state == QTextToSpeech::Ready)
        statusbar->showMessage("Speech stopped...", 2000);
    else if (state == QTextToSpeech::Paused)
        statusbar->showMessage("Speech paused...");
    else
        statusbar->showMessage("Speech error!");

    stopButton->setEnabled(state == QTextToSpeech::Speaking || state == QTextToSpeech::Paused);
}
