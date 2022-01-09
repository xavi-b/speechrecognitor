#include "googlespeechwrapper.h"

GoogleSpeechWrapper::GoogleSpeechWrapper(QObject* parent)
    : QObject(parent)
{
    mgr = new QNetworkAccessManager(this);
    connect(mgr, &QNetworkAccessManager::finished, this, &GoogleSpeechWrapper::replyFinished);
}

void GoogleSpeechWrapper::process(QByteArray const& audio, QAudioEncoderSettings const& settings)
{

    const QUrl      url("https://speech.googleapis.com/v1/speech:recognize");
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, true);

    QJsonDocument data{
        QJsonObject{
            {"audio",
             QJsonObject{
                 {"content", QJsonValue::fromVariant(audio.toBase64())}}},
            {"config",
             QJsonObject{
                 {"encoding", settings.codec()},
                 {"languageCode", "en-US"},
                 {"model", "command_and_search"},
                 {"sampleRateHertz", settings.sampleRate()}}}}};

    reply = mgr->post(req, data.toJson(QJsonDocument::Compact));
}

void GoogleSpeechWrapper::replyFinished(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        emit error(reply->errorString());
    }
    else
    {
        parseResponse(reply);
    }
    this->reply->deleteLater();
    this->reply = nullptr;
}

void GoogleSpeechWrapper::parseResponse(QIODevice* reply)
{
    auto data = QJsonDocument::fromJson(reply->readAll());
    auto err  = data["error"]["message"];

    if (err.isUndefined())
    {
        auto command = data["results"][0]["alternatives"][0]["transcript"].toString();
        emit log(command);
    }
    else
    {
        emit error(err.toString());
    }
}
