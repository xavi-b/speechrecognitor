#ifndef DEEPSPEECHWRAPPER_H
#define DEEPSPEECHWRAPPER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <deepspeech.h>
#include <vector>
#include <QtConcurrent/QtConcurrent>
#include <QDebug>

class DeepSpeechWrapper : public QObject
{
    Q_OBJECT

public:
    struct ds_result
    {
        std::string string;
        double      cpu_time_overall = 0;
    };

    struct meta_word
    {
        std::string word;
        float       start_time = 0;
        float       duration   = 0;
    };

    DeepSpeechWrapper(QObject* parent = nullptr);
    ~DeepSpeechWrapper();

    int setup();
    int getModelSampleRate();

    char*
                           CandidateTranscriptToString(const CandidateTranscript* transcript);
    std::vector<meta_word> CandidateTranscriptToWords(const CandidateTranscript* transcript);
    std::string
    CandidateTranscriptToJSON(const CandidateTranscript* transcript);
    char*
    MetadataToJSON(Metadata* result);

    void      process(QByteArray const& audio);
    void      processAudio(ModelState* context, QByteArray const& audio, bool show_times);
    ds_result runLocalDsSTT(ModelState* aCtx, const short* aBuffer, size_t aBufferSize, bool extended_output, bool json_output);

signals:
    void log(QString const& log);
    void error(QString const& error);

private:
    QString model;
    QString scorer;
    int     beam_width                 = 500;
    float   lm_alpha                   = 0.75f;
    float   lm_beta                    = 1.85f;
    bool    load_without_trie          = false;
    bool    show_times                 = false;
    bool    has_versions               = false;
    bool    extended_metadata          = true;
    bool    json_output                = true;
    size_t  stream_size                = 0;
    int     json_candidate_transcripts = 3;
    int     extended_stream_size       = 0;

    ModelState* ctx = nullptr;
};

#endif // DEEPSPEECHWRAPPER_H
