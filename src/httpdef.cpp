#include "httpdef.h"
#include <QMap>
#include <QFile>

QString getErrMsg(ErrCode code)
{
    QMap<int, QString> errCodeMap;
    errCodeMap.insert(Success, QString("成功"));//ok
    errCodeMap.insert(ErrNoLicense, QString("无效许可证"));//no valid license found
    errCodeMap.insert(ErrLicenseLimit, QString("达到最大在线客户数"));//max online clients count reached
    errCodeMap.insert(ErrTokenInvalid, QString("令牌无效"));//token is invalid
    errCodeMap.insert(ErrTokenExpired, QString("令牌过期"));//token is out of time
    errCodeMap.insert(ErrTokenCreate, QString("令牌创建失败"));//token create failed
    errCodeMap.insert(ErrServiceExpiry, QString("服务授权到期"));//service is out of time
    errCodeMap.insert(ErrSignNotMatch, QString("签名不匹配"));//signature not match

    errCodeMap.insert(ErrNotFound, QString("未找到"));//not found
    errCodeMap.insert(ErrUnauthorized, QString("未经授权"));//unauthorized action
    errCodeMap.insert(ErrAlreadyExist, QString("已经存在"));//already exist
    errCodeMap.insert(ErrParamMissing, QString("缺少必需的参数"));//required parameter is missing
    errCodeMap.insert(ErrParamInvalid, QString("参数无效"));//parameter is invalid
    errCodeMap.insert(ErrSqlExecFailed, QString("数据库操作失败"));//sql executing failed
    errCodeMap.insert(ErrServerInternal, QString("服务器出错"));//server internal error

    errCodeMap.insert(ErrPassword, QString("用户密码不正确"));//user password is not correct
    errCodeMap.insert(ErrClientName, QString("客户端类型不正确"));//deli client type is not correct
    return errCodeMap.value(code);
}

QByteArray getCertsInfo(QString fileName)
{
    QByteArray certsData;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return certsData;
    certsData = file.readAll();
    return certsData;
}
