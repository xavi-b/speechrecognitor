#ifndef GOOGLESPEECHWRAPPER_H
#define GOOGLESPEECHWRAPPER_H

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QAudioEncoderSettings>

class GoogleSpeechWrapper : public QObject
{
    Q_OBJECT
private:
    QNetworkAccessManager* mgr;
    QNetworkReply* reply = nullptr;

public:
    GoogleSpeechWrapper(QObject *parent = nullptr);

    void process(QByteArray const& audio, QAudioEncoderSettings const& settings);
    void replyFinished(QNetworkReply* reply);
    void parseResponse(QIODevice* reply);

signals:
    void log(QString const& log);
    void error(QString const& error);
};

#endif // GOOGLESPEECHWRAPPER_H
