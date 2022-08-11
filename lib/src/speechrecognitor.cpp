#include "speechrecognitor.h"

#include <QRandomGenerator>
#include <QDir>
#include <QMediaRecorder>
#include <QStandardPaths>
#include <QProcess>
#include <QtConcurrent/QtConcurrent>

namespace XB
{

SpeechRecognitor::SpeechRecognitor(QObject* parent)
    : QObject(parent)
{
    m_audioRecorder = new QAudioRecorder(this);
    m_probe         = new QAudioProbe(this);
    connect(m_probe, &QAudioProbe::audioBufferProbed, this, &SpeechRecognitor::processBuffer);
    m_probe->setSource(m_audioRecorder);

    connect(m_audioRecorder, &QAudioRecorder::durationChanged, this, &SpeechRecognitor::updateProgress);
    connect(m_audioRecorder, &QAudioRecorder::statusChanged, this, &SpeechRecognitor::updateStatus);
    connect(m_audioRecorder, &QAudioRecorder::stateChanged, this, &SpeechRecognitor::stateChanged);
    connect(m_audioRecorder, QOverload<QMediaRecorder::Error>::of(&QAudioRecorder::error), this, QOverload<QMediaRecorder::Error>::of(&SpeechRecognitor::error));
}

void SpeechRecognitor::updateProgress(qint64 duration)
{
    qDebug() << "duration" << duration;
    qDebug() << "audio levels" << m_audioLevels;
    if (m_audioRecorder->error() != QMediaRecorder::NoError || duration < 2000)
        return;

    emit progressUpdated(duration);
}

void SpeechRecognitor::updateStatus(QMediaRecorder::Status status)
{
    switch (status)
    {
    case QMediaRecorder::RecordingStatus:
        break;
    case QMediaRecorder::PausedStatus:
        clearAudioLevels();
        break;
    case QMediaRecorder::UnloadedStatus:
    case QMediaRecorder::LoadedStatus:
        clearAudioLevels();
    default:
        break;
    }

    emit statusChanged(status);
}

// TODO minimum duration 2 sec + stop if silence
void SpeechRecognitor::record()
{
    switch (m_audioRecorder->state())
    {
    case QMediaRecorder::PausedState:
        break;
    case QMediaRecorder::RecordingState:
        stop();
    case QMediaRecorder::StoppedState:
        m_audioRecorder->setOutputLocation(QUrl::fromLocalFile(QDir(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::TempLocation)).filePath(QDateTime::currentDateTimeUtc().toString(Qt::ISODate) + ".wav")));
        m_audioRecorder->record();
        break;
    }
}

void SpeechRecognitor::stop()
{
    switch (m_audioRecorder->state())
    {
    case QMediaRecorder::RecordingState:
    case QMediaRecorder::PausedState:
        m_audioRecorder->stop();
        QtConcurrent::run(std::bind(&SpeechRecognitor::processRecord, this));
        break;
    case QMediaRecorder::StoppedState:
        break;
    }
}

void SpeechRecognitor::updateLanguages()
{
    QtConcurrent::run(std::bind(&SpeechRecognitor::processLanguages, this));
}

QList<qreal> SpeechRecognitor::audioLevels() const
{
    return m_audioLevels;
}

QString SpeechRecognitor::lang() const
{
    return m_lang;
}

void SpeechRecognitor::setLang(const QString& lang)
{
    if (m_lang == lang)
        return;

    QtConcurrent::run(std::bind(&SpeechRecognitor::processLang, this, lang));
}

QMediaRecorder::Error SpeechRecognitor::error() const
{
    return m_audioRecorder->error();
}

QString SpeechRecognitor::errorString() const
{
    return m_audioRecorder->errorString();
}

QMediaRecorder::Status SpeechRecognitor::status() const
{
    return m_audioRecorder->status();
}

QMediaRecorder::State SpeechRecognitor::state() const
{
    return m_audioRecorder->state();
}

QString SpeechRecognitor::resultLocation() const
{
    return m_audioRecorder->actualLocation().toLocalFile();
}

void SpeechRecognitor::processRecord()
{
    QFile fOut(QDir(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::TempLocation)).filePath("result.txt"));
    fOut.remove();
    QProcess::execute("vosk-transcriber", {"-l", m_lang, "-i", m_audioRecorder->actualLocation().toLocalFile(), "-o", fOut.fileName()});
    fOut.open(QIODevice::ReadOnly);

    emit resultString(fOut.readAll());
}

void SpeechRecognitor::processLanguages()
{
    QProcess process;
    process.start("vosk-transcriber", {"--list-languages"});
    process.waitForStarted(500);
    process.waitForFinished(500);

    if (process.exitCode() == 0)
        emit languagesChanged(QString(process.readAllStandardOutput()).split('\n'));
    else
        emit languagesChanged(QStringList());
}

void SpeechRecognitor::processLang(QString const& lang)
{
    QString command = QString("from vosk import Model; model = Model(lang=\"%1\")").arg(lang);

    qDebug().noquote() << "processLang" << lang << command;

    if (QProcess::execute("python", {"-u", "-c", command}) == 0)
    {
        qDebug() << "Lang changed to" << lang;

        m_lang = lang;

        emit langChanged(m_lang);
    }
}

void SpeechRecognitor::clearAudioLevels()
{
    for (int i = 0; i < m_audioLevels.size(); ++i)
        m_audioLevels[i] = 0;

    emit audioLevelsChanged(m_audioLevels);
}

// This function returns the maximum possible sample value for a given audio format
qreal getPeakValue(const QAudioFormat& format)
{
    // Note: Only the most common sample formats are supported
    if (!format.isValid())
        return qreal(0);

    if (format.codec() != "audio/pcm")
        return qreal(0);

    switch (format.sampleType())
    {
    case QAudioFormat::Unknown:
        break;
    case QAudioFormat::Float:
        if (format.sampleSize() != 32) // other sample formats are not supported
            return qreal(0);
        return qreal(1.00003);
    case QAudioFormat::SignedInt:
        if (format.sampleSize() == 32)
            return qreal(INT_MAX);
        if (format.sampleSize() == 16)
            return qreal(SHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(CHAR_MAX);
        break;
    case QAudioFormat::UnSignedInt:
        if (format.sampleSize() == 32)
            return qreal(UINT_MAX);
        if (format.sampleSize() == 16)
            return qreal(USHRT_MAX);
        if (format.sampleSize() == 8)
            return qreal(UCHAR_MAX);
        break;
    }

    return qreal(0);
}

template <class T>
QVector<qreal> getBufferLevels(const T* buffer, int frames, int channels)
{
    QVector<qreal> max_values;
    max_values.fill(0, channels);

    for (int i = 0; i < frames; ++i)
    {
        for (int j = 0; j < channels; ++j)
        {
            qreal value = qAbs(qreal(buffer[i * channels + j]));
            if (value > max_values.at(j))
                max_values.replace(j, value);
        }
    }

    return max_values;
}

// returns the audio level for each channel
QVector<qreal> getBufferLevels(const QAudioBuffer& buffer)
{
    QVector<qreal> values;

    if (!buffer.format().isValid() || buffer.format().byteOrder() != QAudioFormat::LittleEndian)
        return values;

    if (buffer.format().codec() != "audio/pcm")
        return values;

    int channelCount = buffer.format().channelCount();
    values.fill(0, channelCount);
    qreal peak_value = getPeakValue(buffer.format());
    if (qFuzzyCompare(peak_value, qreal(0)))
        return values;

    switch (buffer.format().sampleType())
    {
    case QAudioFormat::Unknown:
    case QAudioFormat::UnSignedInt:
        if (buffer.format().sampleSize() == 32)
            values = getBufferLevels(buffer.constData<quint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = getBufferLevels(buffer.constData<quint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = getBufferLevels(buffer.constData<quint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] = qAbs(values.at(i) - peak_value / 2) / (peak_value / 2);
        break;
    case QAudioFormat::Float:
        if (buffer.format().sampleSize() == 32)
        {
            values = getBufferLevels(buffer.constData<float>(), buffer.frameCount(), channelCount);
            for (int i = 0; i < values.size(); ++i)
                values[i] /= peak_value;
        }
        break;
    case QAudioFormat::SignedInt:
        if (buffer.format().sampleSize() == 32)
            values = getBufferLevels(buffer.constData<qint32>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 16)
            values = getBufferLevels(buffer.constData<qint16>(), buffer.frameCount(), channelCount);
        if (buffer.format().sampleSize() == 8)
            values = getBufferLevels(buffer.constData<qint8>(), buffer.frameCount(), channelCount);
        for (int i = 0; i < values.size(); ++i)
            values[i] /= peak_value;
        break;
    }

    return values;
}

void SpeechRecognitor::processBuffer(const QAudioBuffer& buffer)
{
    m_audioLevels = QList<qreal>::fromVector(getBufferLevels(buffer));
}

} // namespace XB
