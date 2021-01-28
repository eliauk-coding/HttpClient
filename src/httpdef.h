#ifndef HTTPDEF_H
#define HTTPDEF_H
#include <QByteArray>
#include <QJsonObject>

enum ErrCode{
    Success = 0x00,

    ErrNoLicense    = 0x01, // no valid license found
    ErrLicenseLimit = 0x02, // max online client reached
    ErrTokenInvalid = 0x03, // token is invalid
    ErrTokenExpired = 0x04, // token is out of time
    ErrTokenCreate  = 0x05, // token create faile
    ErrServiceExpiry = 0x06,// service is out of time
    ErrSignNotMatch = 0x0F, // signature not match

    ErrNotFound       = 0x10, // resource not found
    ErrUnauthorized   = 0x11, // action rejected, mostly caused by authorization
    ErrAlreadyExist   = 0x12, // resource not found
    ErrParamMissing   = 0x13, // required parameter(s) is(are) missing
    ErrParamInvalid   = 0x14, // parameter(s) is(are) invalid
    ErrSqlExecFailed  = 0x15, // database operation error
    ErrServerInternal = 0x16, // server internal error

    ErrPassword       = 0x20, // user password is not match
    ErrClientName     = 0x21 // deli client type is not correct
};

QString getErrMsg(ErrCode code);

QByteArray getCertsInfo(QString fileName);
#endif // HTTPDEF_H
