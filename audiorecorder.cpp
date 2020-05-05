#include "audiorecorder.h"
#include "audiolevel.h"

// This function returns the maximum possible sample value for a given audio format
static qreal getPeakValue(const QAudioFormat& format)
{
    // Note: Only the most common sample formats are supported
    if (!format.isValid())
        return qreal(0);

    if (format.codec() != "audio/pcm")
        return qreal(0);

    switch (format.sampleType()) {
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
static QVector<qreal> getBufferLevels(const T *buffer, int frames, int channels)
{
    QVector<qreal> max_values;
    max_values.fill(0, channels);

    for (int i = 0; i < frames; ++i) {
        for (int j = 0; j < channels; ++j) {
            qreal value = qAbs(qreal(buffer[i * channels + j]));
            if (value > max_values.at(j))
                max_values.replace(j, value);
        }
    }

    return max_values;
}

// returns the audio level for each channel
static QVector<qreal> getBufferLevels(const QAudioBuffer& buffer)
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

    switch (buffer.format().sampleType()) {
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
        if (buffer.format().sampleSize() == 32) {
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

AudioRecorder::AudioRecorder(QObject* parent)
    : QObject(parent)
{

    m_audioRecorder = new QAudioRecorder(this);
    m_probe = new QAudioProbe(this);
    connect(m_probe, &QAudioProbe::audioBufferProbed,
            this, &AudioRecorder::processBuffer);
    m_probe->setSource(m_audioRecorder);

    QDir dir(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::TempLocation));

    m_audioRecorder->setOutputLocation(QUrl::fromLocalFile(dir.filePath("output.wav")));

    connect(m_audioRecorder, &QAudioRecorder::durationChanged, this, &AudioRecorder::durationChanged);
    connect(m_audioRecorder, &QAudioRecorder::statusChanged, this, &AudioRecorder::updateStatus);
    connect(m_audioRecorder, QOverload<QMediaRecorder::Error>::of(&QAudioRecorder::error), this, [=]()
    {
        emit error(m_audioRecorder->errorString());
    });

    deepSpeechWrapper = new DeepSpeechWrapper(this);
    connect(deepSpeechWrapper, &DeepSpeechWrapper::log, this, &AudioRecorder::log);
    connect(deepSpeechWrapper, &DeepSpeechWrapper::error, this, &AudioRecorder::error);
    connect(deepSpeechWrapper, &DeepSpeechWrapper::result, this, &AudioRecorder::result);
}

QStringList AudioRecorder::getAudioInputs() const
{
    return this->m_audioRecorder->audioInputs();
}

QStringList AudioRecorder::getAudioCodecs() const
{
    return this->m_audioRecorder->supportedAudioCodecs();
}

QStringList AudioRecorder::getContainers() const
{
    return this->m_audioRecorder->supportedContainers();
}

QList<int> AudioRecorder::getAudioSampleRates() const
{
    return this->m_audioRecorder->supportedAudioSampleRates();
}

void AudioRecorder::setAudioInput(QString const& audioInput)
{
    m_audioRecorder->setAudioInput(audioInput);
}

void AudioRecorder::setCodec(QString const& codec)
{
    settings.setCodec(codec);
}

void AudioRecorder::setContainer(QString const& container)
{
    this->container = container;
}

void AudioRecorder::setSampleRate(int sampleRate)
{
    settings.setSampleRate(sampleRate);
}

void AudioRecorder::setQuality(int quality)
{
    settings.setQuality(QMultimedia::EncodingQuality(quality));
}

void AudioRecorder::setEncodingMode(bool constantQuality)
{
    settings.setEncodingMode(constantQuality ?
                             QMultimedia::ConstantQualityEncoding :
                             QMultimedia::ConstantBitRateEncoding);
}

void AudioRecorder::updateStatus(QMediaRecorder::Status status)
{
    QString statusMessage;

    switch (status)
    {
    case QMediaRecorder::RecordingStatus:
        statusMessage = tr("Recording to %1").arg(m_audioRecorder->actualLocation().toString());
        break;
    case QMediaRecorder::PausedStatus:
        clearAudioLevels();
        statusMessage = tr("Paused");
        break;
    case QMediaRecorder::UnloadedStatus:
    case QMediaRecorder::LoadedStatus:
        clearAudioLevels();
        statusMessage = tr("Stopped");
    default:
        break;
    }

    if (m_audioRecorder->error() == QMediaRecorder::NoError)
        emit statusUpdated(statusMessage);
}

void AudioRecorder::record()
{
    if (this->m_audioRecorder->state() == QMediaRecorder::StoppedState)
    {
        //if(this->ui->deepSpeechRadioBtn->isChecked())
        {
            this->settings.setSampleRate(this->deepSpeechWrapper->getModelSampleRate());
            this->settings.setCodec("audio/x-raw");
            this->container = "audio/x-wav";
        }

        this->settings.setChannelCount(1);
        this->settings.setBitRate(16);

        this->m_audioRecorder->setEncodingSettings(this->settings, QVideoEncoderSettings(), this->container);

        this->m_audioRecorder->record();
    }
}

void AudioRecorder::stop(bool sendToDeepSpeech)
{
    m_audioRecorder->stop();

    emit stopSignal();

    if(!sendToDeepSpeech)
        return;

    QFile file(this->m_audioRecorder->outputLocation().toLocalFile());
    if(!file.open(QIODevice::ReadOnly))
        return;

    this->sendToDeepSpeech(file.readAll());
}

void AudioRecorder::clearAudioLevels()
{
    for (int i = 0; i < m_audioLevels.size(); ++i)
        m_audioLevels[i] = 0;
}

void AudioRecorder::processBuffer(QAudioBuffer const& buffer)
{
    static int silenceSize = 0;

    this->m_audioLevels = getBufferLevels(buffer);

    //TODO threshold setting widget
    if(this->m_audioLevels[0] < 0.001)
        ++silenceSize;
    else
        silenceSize = 0;

    //TODO delay setting widget
    if(silenceSize > 30)
    {
        silenceSize = 0;
        qDebug() << "silence";
        stop();
    }
}

void AudioRecorder::sendToDeepSpeech(QByteArray const& audio)
{
    deepSpeechWrapper->process(audio);
}
