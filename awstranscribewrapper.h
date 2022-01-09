#ifndef AWSTRANSCRIBEWRAPPER_H
#define AWSTRANSCRIBEWRAPPER_H

#include <QObject>
#include <QAudioEncoderSettings>
#include <aws/core/Aws.h>
#include <aws/core/utils/threading/Semaphore.h>
#include <aws/transcribestreaming/TranscribeStreamingServiceClient.h>
#include <aws/transcribestreaming/model/StartStreamTranscriptionHandler.h>
#include <aws/transcribestreaming/model/StartStreamTranscriptionRequest.h>

class AWSTranscribeWrapper : public QObject
{
public:
    AWSTranscribeWrapper(QObject* parent = nullptr);
    ~AWSTranscribeWrapper();

    void setup();
    void process(const QByteArray& audio, const QAudioEncoderSettings& settings);

    int getSampleRate() const;

private:
    int SampleRate = 16000; // 16 Khz

    Aws::SDKOptions options;

    Aws::TranscribeStreamingService::TranscribeStreamingServiceClient*      client = nullptr;
    Aws::TranscribeStreamingService::Model::StartStreamTranscriptionHandler handler;
    Aws::TranscribeStreamingService::Model::StartStreamTranscriptionRequest request;
};

#endif // AWSTRANSCRIBEWRAPPER_H
