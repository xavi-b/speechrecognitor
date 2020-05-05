#include "deepspeechwrapper.h"

DeepSpeechWrapper::DeepSpeechWrapper(QObject *parent) : QObject(parent)
{
    char model[] = "/home/xavier/workspace/speechrecognitor/models/models.pbmm";
    char scorer[] = "/home/xavier/workspace/speechrecognitor/models/models.scorer";

    // Initialise DeepSpeech
    int status = DS_CreateModel(model, &ctx);

    if(status != 0)
    {
        emit error("Could not create model.");
        return;
    }

    status = DS_SetModelBeamWidth(ctx, beam_width);
    if (status != 0)
    {
        emit error("Could not set model beam width.");
        return;
    }

    status = DS_EnableExternalScorer(ctx, scorer);
    if(status != 0)
    {
        emit error("Could not enable external scorer.");
        return;
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

std::string DeepSpeechWrapper::candidateTranscriptToString(CandidateTranscript const* transcript)
{
    std::string retval = "";
    for (int i = 0; i < transcript->num_tokens; i++)
    {
        const TokenMetadata& token = transcript->tokens[i];
        retval += token.text;
    }
    return retval.c_str();
}

std::vector<DeepSpeechWrapper::meta_word> DeepSpeechWrapper::candidateTranscriptToWords(CandidateTranscript const* transcript)
{
    std::vector<meta_word> word_list;

    std::string word = "";
    float word_start_time = 0;

    // Loop through each token
    for (int i = 0; i < transcript->num_tokens; i++)
    {
        const TokenMetadata& token = transcript->tokens[i];

        // Append token to word if it's not a space
        if (strcmp(token.text, u8" ") != 0)
        {
            // Log the start time of the new word
            if (word.length() == 0)
            {
                word_start_time = token.start_time;
            }
            word.append(token.text);
        }

        // Word boundary is either a space or the last token in the array
        if (strcmp(token.text, u8" ") == 0 || i == transcript->num_tokens-1)
        {
            float word_duration = token.start_time - word_start_time;

            if (word_duration < 0)
            {
                word_duration = 0;
            }

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

std::string DeepSpeechWrapper::candidateTranscriptToJSON(CandidateTranscript const* transcript)
{
    std::ostringstream out_string;

    std::vector<meta_word> words = candidateTranscriptToWords(transcript);

    out_string << R"("metadata":{"confidence":)" << transcript->confidence << R"(},"words":[)";

    for (int i = 0; i < words.size(); i++)
    {
        meta_word w = words[i];
        out_string << R"({"word":")" << w.word << R"(","time":)" << w.start_time << R"(,"duration":)" << w.duration << "}";

        if (i < words.size() - 1)
        {
            out_string << ",";
        }
    }

    out_string << "]";

    return out_string.str();
}

QJsonObject DeepSpeechWrapper::metadataToJson(Metadata* result)
{
    std::ostringstream out_string;
    out_string << "{\n";

    for (int j=0; j < result->num_transcripts; ++j)
    {
        const CandidateTranscript *transcript = &result->transcripts[j];

        if (j == 0)
        {
            out_string << candidateTranscriptToJSON(transcript);

            if (result->num_transcripts > 1)
            {
                out_string << ",\n" << R"("alternatives")" << ":[\n";
            }
        }
        else
        {
            out_string << "{" << candidateTranscriptToJSON(transcript) << "}";

            if (j < result->num_transcripts - 1)
            {
                out_string << ",\n";
            }
            else
            {
                out_string << "\n]";
            }
        }
    }

    out_string << "\n}\n";

    QJsonDocument doc = QJsonDocument::fromRawData(out_string.str().data(), out_string.str().size());

    return doc.object();
}

void DeepSpeechWrapper::process(QByteArray const& audio)
{
    QtConcurrent::run(std::bind(&DeepSpeechWrapper::processAudio, this, ctx, audio, show_times));
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

    emit this->result(result.string);
    emit log(QString("Result: %1").arg(result.string));

    if(show_times)
        emit log(QString("cpu_time_overall=%1").arg(result.cpu_time_overall));
}

DeepSpeechWrapper::ds_result DeepSpeechWrapper::runLocalDsSTT(ModelState* aCtx, const short* aBuffer, size_t aBufferSize, bool extended_output, bool json_output)
{
    ds_result res;

    clock_t ds_start_time = clock();

    if(extended_output)
    {
        Metadata* result = DS_SpeechToTextWithMetadata(aCtx, aBuffer, aBufferSize, 1);
        res.string = candidateTranscriptToString(&result->transcripts[0]).c_str();
        DS_FreeMetadata(result);
    }
    else if(json_output)
    {
        Metadata* result = DS_SpeechToTextWithMetadata(aCtx, aBuffer, aBufferSize, 1);
        QJsonDocument doc(metadataToJson(result));
        res.string = doc.toJson(QJsonDocument::Compact);
        DS_FreeMetadata(result);
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
