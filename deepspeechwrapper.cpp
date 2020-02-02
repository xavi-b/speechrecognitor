#include "deepspeechwrapper.h"

DeepSpeechWrapper::DeepSpeechWrapper(QObject *parent) : QObject(parent)
{
    this->lm = "/usr/share/mozilla/deepspeech/models/lm.binary";
    this->model = "/usr/share/mozilla/deepspeech/models/output_graph.pbmm";
    this->trie = "/usr/share/mozilla/deepspeech/models/trie";

    // Initialise DeepSpeech
    int status = DS_CreateModel(model.toStdString().c_str(), beam_width, &ctx);

    if(status != 0)
    {
        emit error("Could not create model.");
        return;
    }

    if(!lm.isEmpty() && (!trie.isEmpty() || load_without_trie))
    {
        int status = DS_EnableDecoderWithLM(ctx,
                                            lm.toStdString().c_str(),
                                            trie.toStdString().c_str(),
                                            lm_alpha,
                                            lm_beta);
        if(status != 0)
            emit error("Could not enable CTC decoder with LM.");
    }
}

DeepSpeechWrapper::~DeepSpeechWrapper()
{
    if(ctx != nullptr)
        DS_FreeModel(ctx);
}

int DeepSpeechWrapper::getModelSampleRate()
{
    if(ctx != nullptr)
        return DS_GetModelSampleRate(ctx);

    return 0;
}

#include <time.h>

QString DeepSpeechWrapper::metadataToString(Metadata* metadata)
{
    QString retval = "";

    for(int i = 0; i < metadata->num_items; i++)
    {
        MetadataItem item = metadata->items[i];
        retval += item.character;
    }

    return retval;
}

std::vector<DeepSpeechWrapper::meta_word> DeepSpeechWrapper::wordsFromMetadata(Metadata* metadata)
{
    std::vector<meta_word> word_list;

    QString word = "";
    float word_start_time = 0;

    // Loop through each character
    for(int i = 0; i < metadata->num_items; i++)
    {
        MetadataItem item = metadata->items[i];

        qDebug() << item.character;

        // Append character to word if it's not a space
        if(strcmp(item.character, u8" ") != 0)
        {
            // Log the start time of the new word
            if(word.length() == 0)
                word_start_time = item.start_time;

            word.append(item.character);
        }

        // Word boundary is either a space or the last character in the array
        if(strcmp(item.character, " ") == 0
        || strcmp(item.character, u8" ") == 0
        || i == metadata->num_items-1)
        {
            float word_duration = item.start_time - word_start_time;

            if(word_duration < 0)
                word_duration = 0;

            meta_word w;
            w.word = word;
            w.start_time = word_start_time;
            w.duration = word_duration;

            word_list.push_back(w);

            // Reset
            word = "";
            word_start_time = 0;
        }
    }

    return word_list;
}

QJsonObject DeepSpeechWrapper::metadataToJson(Metadata* metadata)
{
    std::vector<meta_word> words = wordsFromMetadata(metadata);

    QJsonObject object;

    object["metadata"].toObject()["confidence"] = metadata->confidence;

    for(size_t i = 0; i < words.size(); i++)
    {
        meta_word w = words[i];

        object["words"].toArray()[i].toObject()["word"] = w.word;
        object["words"].toArray()[i].toObject()["time"] = w.start_time;
        object["words"].toArray()[i].toObject()["duration"] = w.duration;
    }

    return object;
}

int DeepSpeechWrapper::process(QByteArray const& audio)
{
    processAudio(ctx, audio, show_times);
}

void DeepSpeechWrapper::processAudio(ModelState* context, QByteArray const& audio, bool show_times)
{
    // Pass audio to DeepSpeech
    // We take half of buffer_size because buffer is a char* while
    // LocalDsSTT() expected a short*
    ds_result result = runLocalDsSTT(context,
                                 (const short*)audio.data(),
                                  audio.size() / 2,
                                  extended_metadata,
                                  json_output);

    emit log(QString("Result %1").arg(result.string));

    if(show_times)
        emit log(QString("cpu_time_overall=%1").arg(result.cpu_time_overall));
}

DeepSpeechWrapper::ds_result DeepSpeechWrapper::runLocalDsSTT(ModelState* aCtx, const short* aBuffer, size_t aBufferSize, bool extended_output, bool json_output)
{
    ds_result res;

    clock_t ds_start_time = clock();

    if(extended_output)
    {
        Metadata* metadata = DS_SpeechToTextWithMetadata(aCtx, aBuffer, aBufferSize);
        res.string = metadataToString(metadata);
        DS_FreeMetadata(metadata);
    }
    else if(json_output)
    {
        Metadata* metadata = DS_SpeechToTextWithMetadata(aCtx, aBuffer, aBufferSize);
        QJsonDocument doc(metadataToJson(metadata));
        res.string = doc.toJson(QJsonDocument::Compact);
        DS_FreeMetadata(metadata);
    }
    else if(stream_size > 0)
    {
        StreamingState* ctx;
        int status = DS_CreateStream(aCtx, &ctx);
        if(status != DS_ERR_OK)
        {
            res.string = strdup("");
            return res;
        }

        size_t off = 0;
        const char *last = nullptr;
        while(off < aBufferSize)
        {
            size_t cur = aBufferSize - off > stream_size ? stream_size : aBufferSize - off;
            DS_FeedAudioContent(ctx, aBuffer + off, cur);
            off += cur;

            const char* partial = DS_IntermediateDecode(ctx);
            if(last == nullptr || strcmp(last, partial))
            {
                printf("%s\n", partial);
                last = partial;
            }
            else
            {
                DS_FreeString((char *) partial);
            }
        }
        if(last != nullptr)
        {
            DS_FreeString((char *) last);
        }
        res.string = DS_FinishStream(ctx);
    }
    else
    {
        res.string = DS_SpeechToText(aCtx, aBuffer, aBufferSize);
    }

    clock_t ds_end_infer = clock();

    res.cpu_time_overall =((double)(ds_end_infer - ds_start_time)) / CLOCKS_PER_SEC;

    return res;
}
