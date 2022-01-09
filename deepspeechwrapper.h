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
        QString string;
        double  cpu_time_overall = 0;
    };

    struct meta_word
    {
        QString word;
        float   start_time = 0;
        float   duration   = 0;
    };

    DeepSpeechWrapper(QObject* parent = nullptr);
    ~DeepSpeechWrapper();

    int getModelSampleRate();

    static QString                metadataToString(Metadata* metadata);
    static std::vector<meta_word> wordsFromMetadata(Metadata* metadata);
    static QJsonObject            metadataToJson(Metadata* metadata);

    void      process(QByteArray const& audio);
    void      processAudio(ModelState* context, QByteArray const& audio, bool show_times);
    ds_result runLocalDsSTT(ModelState* aCtx, const short* aBuffer, size_t aBufferSize, bool extended_output, bool json_output);

signals:
    void log(QString const& log);
    void error(QString const& error);

private:
    QString model;
    QString lm;
    QString trie;
    int     beam_width        = 500;
    float   lm_alpha          = 0.75f;
    float   lm_beta           = 1.85f;
    bool    load_without_trie = false;
    bool    show_times        = false;
    bool    has_versions      = false;
    bool    extended_metadata = true;
    bool    json_output       = true;
    size_t  stream_size       = 0;

    ModelState* ctx = nullptr;
};

#endif // DEEPSPEECHWRAPPER_H
