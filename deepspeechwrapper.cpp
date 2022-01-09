#include "deepspeechwrapper.h"
#include <time.h>
#include <sstream>

DeepSpeechWrapper::DeepSpeechWrapper(QObject* parent)
    : QObject(parent)
{
}

int DeepSpeechWrapper::setup()
{
    this->model  = DEEPSPEECHDATA "/deepspeech-0.9.3-models.pbmm";
    this->scorer = DEEPSPEECHDATA "/deepspeech-0.9.3-models.scorer";

    // Initialise DeepSpeech
    int status = 0;

    status = DS_CreateModel(model.toStdString().c_str(), &ctx);
    if (status != 0)
    {
        emit error("Could not create model.");
        return 1;
    }

    status = DS_SetModelBeamWidth(ctx, beam_width);
    if (status != 0)
    {
        emit error("Could not set model beam width.");
        return 1;
    }

    status = DS_EnableExternalScorer(ctx, scorer.toStdString().c_str());
    if (status != 0)
    {
        emit error("Could not enable external scorer.");
        return 1;
    }

    status = DS_SetScorerAlphaBeta(ctx, lm_alpha, lm_beta);
    if (status != 0)
    {
        emit error("Error setting scorer alpha and beta.");
        return 1;
    }

    return 0;
}

DeepSpeechWrapper::~DeepSpeechWrapper()
{
    if (ctx != nullptr)
        DS_FreeModel(ctx);
}

int DeepSpeechWrapper::getModelSampleRate()
{
    if (ctx != nullptr)
        return DS_GetModelSampleRate(ctx);

    return 0;
}

char* DeepSpeechWrapper::CandidateTranscriptToString(const CandidateTranscript* transcript)
{
    std::string retval = "";
    for (uint i = 0; i < transcript->num_tokens; i++)
    {
        const TokenMetadata& token = transcript->tokens[i];
        retval += token.text;
    }
    return strdup(retval.c_str());
}

std::vector<DeepSpeechWrapper::meta_word> DeepSpeechWrapper::CandidateTranscriptToWords(const CandidateTranscript* transcript)
{
    std::vector<meta_word> word_list;

    std::string word            = "";
    float       word_start_time = 0;

    // Loop through each token
    for (uint i = 0; i < transcript->num_tokens; i++)
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
        if (strcmp(token.text, u8" ") == 0 || i == transcript->num_tokens - 1)
        {
            float word_duration = token.start_time - word_start_time;

            if (word_duration < 0)
            {
                word_duration = 0;
            }

            meta_word w;
            w.word       = word;
            w.start_time = word_start_time;
            w.duration   = word_duration;

            word_list.push_back(w);

            // Reset
            word            = "";
            word_start_time = 0;
        }
    }

    return word_list;
}

std::string DeepSpeechWrapper::CandidateTranscriptToJSON(const CandidateTranscript* transcript)
{
    std::ostringstream out_string;

    std::vector<meta_word> words = CandidateTranscriptToWords(transcript);

    out_string << R"("metadata":{"confidence":)" << transcript->confidence << R"(},"words":[)";

    for (uint i = 0; i < words.size(); i++)
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

char* DeepSpeechWrapper::MetadataToJSON(Metadata* result)
{
    std::ostringstream out_string;
    out_string << "{\n";

    for (uint j = 0; j < result->num_transcripts; ++j)
    {
        const CandidateTranscript* transcript = &result->transcripts[j];

        if (j == 0)
        {
            out_string << CandidateTranscriptToJSON(transcript);

            if (result->num_transcripts > 1)
            {
                out_string << ",\n"
                           << R"("alternatives")"
                           << ":[\n";
            }
        }
        else
        {
            out_string << "{" << CandidateTranscriptToJSON(transcript) << "}";

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

    return strdup(out_string.str().c_str());
}

void DeepSpeechWrapper::process(QByteArray const& audio)
{
    QtConcurrent::run(std::bind(&DeepSpeechWrapper::processAudio, this, ctx, audio, show_times));
}

void DeepSpeechWrapper::processAudio(ModelState* context, QByteArray const& audio, bool show_times)
{
    qDebug() << "Starting processAudio";
    // Pass audio to DeepSpeech
    // We take half of buffer_size because buffer is a char* while
    // LocalDsSTT() expected a short*
    ds_result result = runLocalDsSTT(context,
                                     (const short*)audio.data(),
                                     audio.size() / 2,
                                     extended_metadata,
                                     json_output);

    emit log(QString("Result: '%1'").arg(result.string.c_str()));

    if (show_times)
        emit log(QString("cpu_time_overall=%1").arg(result.cpu_time_overall));
    qDebug() << "Ending processAudio";
}

DeepSpeechWrapper::ds_result DeepSpeechWrapper::runLocalDsSTT(ModelState* aCtx, const short* aBuffer, size_t aBufferSize, bool extended_output, bool json_output)
{
    qDebug() << "runLocalDsSTT";
    ds_result res;

    clock_t ds_start_time = clock();

    if (extended_output)
    {
        qDebug() << "extended_output";
        Metadata* result = DS_SpeechToTextWithMetadata(aCtx, aBuffer, aBufferSize, 1);
        res.string       = CandidateTranscriptToString(&result->transcripts[0]);
        DS_FreeMetadata(result);
    }
    else if (json_output)
    {
        qDebug() << "json_output";
        Metadata* result = DS_SpeechToTextWithMetadata(aCtx, aBuffer, aBufferSize, json_candidate_transcripts);
        res.string       = MetadataToJSON(result);
        DS_FreeMetadata(result);
    }
    else if (stream_size > 0)
    {
        qDebug() << "stream_size > 0";
        StreamingState* ctx;
        int             status = DS_CreateStream(aCtx, &ctx);
        if (status != DS_ERR_OK)
        {
            res.string = strdup("");
            return res;
        }
        size_t      off  = 0;
        const char* last = nullptr;
        const char* prev = nullptr;
        while (off < aBufferSize)
        {
            size_t cur = aBufferSize - off > stream_size ? stream_size : aBufferSize - off;
            DS_FeedAudioContent(ctx, aBuffer + off, cur);
            off += cur;
            prev                = last;
            const char* partial = DS_IntermediateDecode(ctx);
            if (last == nullptr || strcmp(last, partial))
            {
                printf("%s\n", partial);
                last = partial;
            }
            else
            {
                DS_FreeString((char*)partial);
            }
            if (prev != nullptr && prev != last)
            {
                DS_FreeString((char*)prev);
            }
        }
        if (last != nullptr)
        {
            DS_FreeString((char*)last);
        }
        res.string = DS_FinishStream(ctx);
    }
    else if (extended_stream_size > 0)
    {
        qDebug() << "extended_stream_size > 0";
        StreamingState* ctx;
        int             status = DS_CreateStream(aCtx, &ctx);
        if (status != DS_ERR_OK)
        {
            res.string = strdup("");
            return res;
        }
        size_t      off  = 0;
        const char* last = nullptr;
        const char* prev = nullptr;
        while (off < aBufferSize)
        {
            size_t cur = aBufferSize - off > extended_stream_size ? extended_stream_size : aBufferSize - off;
            DS_FeedAudioContent(ctx, aBuffer + off, cur);
            off += cur;
            prev                    = last;
            const Metadata* result  = DS_IntermediateDecodeWithMetadata(ctx, 1);
            const char*     partial = CandidateTranscriptToString(&result->transcripts[0]);
            if (last == nullptr || strcmp(last, partial))
            {
                printf("%s\n", partial);
                last = partial;
            }
            else
            {
                free((char*)partial);
            }
            if (prev != nullptr && prev != last)
            {
                free((char*)prev);
            }
            DS_FreeMetadata((Metadata*)result);
        }
        const Metadata* result = DS_FinishStreamWithMetadata(ctx, 1);
        res.string             = CandidateTranscriptToString(&result->transcripts[0]);
        DS_FreeMetadata((Metadata*)result);
        free((char*)last);
    }
    else
    {
        qDebug() << "else";
        res.string = DS_SpeechToText(aCtx, aBuffer, aBufferSize);
    }

    clock_t ds_end_infer = clock();

    res.cpu_time_overall =
        ((double)(ds_end_infer - ds_start_time)) / CLOCKS_PER_SEC;

    return res;
}
