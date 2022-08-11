#include "audiorecorder.h"
#include "audiolevel.h"

#include <QVBoxLayout>

AudioRecorder::AudioRecorder()
{
    m_speechrecognitor = new XB::SpeechRecognitor(this);
    connect(m_speechrecognitor, &XB::SpeechRecognitor::progressUpdated, this, &AudioRecorder::updateProgress);
    connect(m_speechrecognitor, &XB::SpeechRecognitor::audioLevelsChanged, this, &AudioRecorder::updateAudioLevels);
    connect(m_speechrecognitor, QOverload<QMediaRecorder::Error>::of(&XB::SpeechRecognitor::error), this, &AudioRecorder::displayErrorMessage);
    connect(m_speechrecognitor, &XB::SpeechRecognitor::statusChanged, this, &AudioRecorder::updateStatus);
    connect(m_speechrecognitor, &XB::SpeechRecognitor::stateChanged, this, &AudioRecorder::onStateChanged);

    connect(m_speechrecognitor, &XB::SpeechRecognitor::resultString, this, [=](QString const& result) {
        qDebug() << "Result:" << result;
    });
    connect(m_speechrecognitor, &XB::SpeechRecognitor::languagesChanged, this, [=](QStringList const& languages) {
        qDebug() << "Languages:" << languages;
    });

    QVBoxLayout* layout = new QVBoxLayout;

    QPushButton* languagesButton = new QPushButton("Update languages");
    connect(languagesButton, &QPushButton::clicked, m_speechrecognitor, &XB::SpeechRecognitor::updateLanguages);
    layout->addWidget(languagesButton);

    m_recordButton = new QPushButton("Record");
    connect(m_recordButton, &QPushButton::clicked, this, &AudioRecorder::toggleRecord);
    layout->addWidget(m_recordButton);

    m_levelsLayout = new QVBoxLayout;
    layout->addLayout(m_levelsLayout);

    m_statusBar = new QStatusBar;
    layout->addWidget(m_statusBar);

    setLayout(layout);
}

void AudioRecorder::updateProgress(qint64 duration)
{
    m_statusBar->showMessage(tr("Recorded %1 sec").arg(duration / 1000));
}

void AudioRecorder::updateStatus(QMediaRecorder::Status status)
{
    QString statusMessage;

    switch (status)
    {
    case QMediaRecorder::RecordingStatus:
        statusMessage = tr("Recording to %1").arg(m_speechrecognitor->resultLocation());
        break;
    case QMediaRecorder::PausedStatus:
        statusMessage = tr("Paused");
        break;
    case QMediaRecorder::UnloadedStatus:
    case QMediaRecorder::LoadedStatus:
        statusMessage = tr("Stopped");
    default:
        break;
    }

    if (m_speechrecognitor->error() == QMediaRecorder::NoError)
        m_statusBar->showMessage(statusMessage);
}

void AudioRecorder::onStateChanged(QMediaRecorder::State state)
{
    switch (state)
    {
    case QMediaRecorder::RecordingState:
        m_recordButton->setText(tr("Stop"));
        break;
    case QMediaRecorder::PausedState:
    case QMediaRecorder::StoppedState:
        m_recordButton->setText(tr("Record"));
        break;
    }
}

void AudioRecorder::toggleRecord()
{
    if (m_speechrecognitor->state() == QMediaRecorder::StoppedState)
        m_speechrecognitor->record();
    else
        m_speechrecognitor->stop();
}

void AudioRecorder::displayErrorMessage()
{
    m_statusBar->showMessage(m_speechrecognitor->errorString());
}

void AudioRecorder::updateAudioLevels(QList<qreal> const& levels)
{
    if (m_audioLevels.count() != levels.count())
    {
        qDeleteAll(m_audioLevels);
        m_audioLevels.clear();
        for (int i = 0; i < levels.count(); ++i)
        {
            AudioLevel* level = new AudioLevel(this);
            m_audioLevels.append(level);
            m_levelsLayout->addWidget(level);
        }
    }
}
