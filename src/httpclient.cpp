#include "httpclient.h"
#include "httpdef.h"
#include <QEventLoop>
#include <QSslKey>
#include <QCoreApplication>
#include <QUrlQuery>
#include <QDebug>

const QString clientKeyPath = "/config/certs/client.key";
const QString clientCrtPath = "/config/certs/client.crt";
const QString serverCrtPath = "/config/certs/ca.crt";

HttpClient::HttpClient()
{

}

void HttpClient::initSslConfig()
{
    QByteArray clientKey = getCertsInfo(QCoreApplication::applicationDirPath() + clientKeyPath);
    QByteArray clientCrt = getCertsInfo(QCoreApplication::applicationDirPath() + clientCrtPath);
    QByteArray serverCrt = getCertsInfo(QCoreApplication::applicationDirPath() + serverCrtPath);
    sslConfig.setProtocol(QSsl::TlsV1_2);
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);
    sslConfig.setPrivateKey(QSslKey(clientKey, QSsl::Rsa));
    QList<QSslCertificate> certList;
    certList.append(QSslCertificate(serverCrt, QSsl::Pem));
    sslConfig.setCaCertificates(certList);
    sslConfig.setLocalCertificate(QSslCertificate(clientCrt, QSsl::Pem));
}

QNetworkRequest HttpClient::getNetworkRequest(const QString &interface,
                                              const QMap<QByteArray, QByteArray> &headers,
                                              const QMap<QString, QString> &queryItems)
{
    QUrlQuery query;
    QUrl url(interface);
    //params
    foreach(QString key, queryItems.keys()) {
        query.addQueryItem(key, queryItems.value(key));
    }
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, headers.value("Content-Type"));
    //header
    foreach(QByteArray key, headers.keys()) {
        if(key == "Content-Type")
            continue;
        request.setRawHeader(key, headers.value(key));
    }
    return request;
}

Response HttpClient::request(const QString &method, QNetworkRequest *req, QByteArray data)
{
    req->setSslConfiguration(sslConfig);

    QEventLoop loop;
    Response result;
    QNetworkAccessManager manager;
    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply* reply;
    if(method == "GET"){
        reply = manager.get(*req);
    } else if(method == "PUT") {
        reply = manager.put(*req, data);
    } else if(method == "POST") {
        reply = manager.post(*req, data);
    } else {
        result.statusCode = 404;
        return result;
    }
    loop.exec();
    result.error = reply->error();
    auto codeVar = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(codeVar.isValid()){
        result.statusCode = codeVar.toInt();
    }
    result.ContentTypeHeader = reply->header(QNetworkRequest::ContentTypeHeader);
    result.ContentLengthHeader = reply->header(QNetworkRequest::ContentLengthHeader);
    result.ServerHeader = reply->header(QNetworkRequest::ServerHeader);
    result.body = reply->readAll();
    return result;
}
