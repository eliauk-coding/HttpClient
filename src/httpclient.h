#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QByteArray>
#include <QNetworkReply>

struct Response
{
    int statusCode;//状态码
    QVariant ContentTypeHeader;
    QVariant ContentLengthHeader;
    QVariant ServerHeader;
    QByteArray body;
    QNetworkReply::NetworkError error;
};

class HttpClient
{
public:
    HttpClient();
    void initSslConfig();
    QNetworkRequest getNetworkRequest(const QString &interface,
                                      const QMap<QByteArray, QByteArray> &headers,
                                      const QMap<QString, QString> &queryItems);
    Response request(const QString &method, QNetworkRequest* req, QByteArray data);
private:
    QSslConfiguration sslConfig;
};

#endif // HTTPCLIENT_H
