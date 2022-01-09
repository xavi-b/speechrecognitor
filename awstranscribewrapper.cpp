#include "awstranscribewrapper.h"
#include <QDebug>

using namespace Aws;
using namespace Aws::TranscribeStreamingService;
using namespace Aws::TranscribeStreamingService::Model;

AWSTranscribeWrapper::AWSTranscribeWrapper(QObject* parent)
    : QObject(parent)
{
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    Aws::InitAPI(options);
}

AWSTranscribeWrapper::~AWSTranscribeWrapper()
{
    if (client != nullptr)
        delete client;

    Aws::ShutdownAPI(options);
}

void AWSTranscribeWrapper::setup()
{
    Aws::Client::ClientConfiguration config;
    client = new TranscribeStreamingServiceClient(config);
    handler.SetTranscriptEventCallback([](const TranscriptEvent& ev) {
        qDebug() << "SetTranscriptEventCallback";
        for (auto&& r : ev.GetTranscript().GetResults())
        {
            if (r.GetIsPartial())
            {
                qDebug() << "[partial]";
            }
            else
            {
                qDebug() << "[Final]";
            }

            for (auto&& alt : r.GetAlternatives())
            {
                qDebug() << alt.GetTranscript().c_str();
            }
        }
    });
    handler.SetOnErrorCallback([](const Aws::Client::AWSError<TranscribeStreamingServiceErrors>& error) {
        qDebug() << "Error" << error.GetMessage().c_str();
    });
}

void AWSTranscribeWrapper::process(QByteArray const& audio, QAudioEncoderSettings const& settings)
{
    qDebug() << "AWSTranscribeWrapper process" << audio.size();

    request.SetMediaSampleRateHertz(SampleRate);
    request.SetLanguageCode(LanguageCode::en_US);
    request.SetMediaEncoding(MediaEncoding::pcm);
    request.SetEventStreamHandler(handler);

    auto OnStreamReady = [=](AudioStream& stream) {
        qDebug() << "START OnStreamReady" << audio.size();
        Aws::Vector<unsigned char> bits{audio.data(), audio.data() + audio.size()};
        AudioEvent                 event(std::move(bits));
        stream.WriteAudioEvent(event);
        stream.flush();
        stream.Close();
        qDebug() << "END OnStreamReady";
    };

    auto OnResponseCallback = [](const TranscribeStreamingServiceClient*,
                                 const Model::StartStreamTranscriptionRequest&,
                                 const Model::StartStreamTranscriptionOutcome& outcome,
                                 const std::shared_ptr<const Aws::Client::AsyncCallerContext>&) {
        // TODO
        qDebug() << "OnResponseCallback";
        qDebug() << outcome.IsSuccess();
        qDebug() << outcome.GetError().GetExceptionName().c_str();
        qDebug() << outcome.GetError().GetMessage().c_str();
    };

    client->StartStreamTranscriptionAsync(request, OnStreamReady, OnResponseCallback, nullptr /*context*/);
}

int AWSTranscribeWrapper::getSampleRate() const
{
    return this->SampleRate;
}
