#include "settings.h"
#include <QFile>
#include <QJsonParseError>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDirIterator>
#include <QRandomGenerator>
#include <stdarg.h>



void LogHandler(QtMsgType type, const QMessageLogContext &info, const QString &msg)
{
    QString lType;
    switch (type) {
    case QtDebugMsg:
        lType = "D";
        break;
    case QtInfoMsg:
        lType = "I";
        break;
    case QtWarningMsg:
        lType = "W";
        break;
    case QtCriticalMsg:
        lType = "E";
        break;
    case QtFatalMsg:
        lType = "F";
        break;
    default:
        break;
    }
    Settings::GetInstance()->outputLog(lType, info.file, info.function, info.line, msg);
}

static bool readJsonFile(QIODevice &device, QSettings::SettingsMap &map)
{
    QJsonParseError jsonError;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(device.readAll(), &jsonError));
    if(jsonError.error != QJsonParseError::NoError)
    {
        qDebug()<< "json error:" << jsonError.errorString();
        return false;
    }
    map = jsonDoc.toVariant().toMap();
    //    for(QMap<QString, QVariant>::const_iterator iter = map1.begin(); iter != map1.end(); ++iter){
    //        map[iter.key()] = iter.value();
    //    }
    return true;
}

static bool writeJsonFile(QIODevice &device, const QSettings::SettingsMap &map)
{
    bool ret = false;

    QJsonObject rootObj;
    /*QJsonDocument jsonDocument; = QJsonDocument::fromVariant(QVariant::fromValue(map));
    if ( device.write(jsonDocument.toJson()) != -1 )
        ret = true;*/
    for(QMap<QString, QVariant>::const_iterator iter = map.begin(); iter != map.end(); ++iter){
        rootObj[iter.key()] = QJsonValue::fromVariant(iter.value());
    }
    QJsonDocument jsonDoc;
    jsonDoc.setObject(rootObj);
    if ( device.write(jsonDoc.toJson()) != -1 )
        ret = true;
    return ret;
}

Settings* Settings::m_iInstance = nullptr;
QString Settings::APP_NAME = "LabelImg";
QString Settings::APP_VERSION="0.0.0";

Settings* Settings::GetInstance(){
    if(m_iInstance == nullptr){
        m_iInstance = new Settings();
    }
    return m_iInstance;
}

Settings::Settings():QSettings(Settings::APP_NAME+".json" ,QSettings::registerFormat("json", readJsonFile, writeJsonFile))
    , m_iLogFile(nullptr), m_iLogStream(nullptr)
{
    m_LastLogTime = QDateTime::currentDateTime();
    sync();
    m_LogPath = "logs";
}

Settings::~Settings()
{
    if (m_iLogStream != nullptr){
        m_iLogStream->flush();
        delete m_iLogStream;
        m_iLogStream = nullptr;
    }
    if (m_iLogFile != nullptr){
        m_iLogFile->flush();
        m_iLogFile->close();
        delete m_iLogFile;
        m_iLogFile = nullptr;
    }
    printf("-----------Settings destruction\n");
}

bool Settings::GetBool(QString key, bool defaultVal)
{
    if (key.contains("/")){
        QStringList tmplist = key.split("/");
        if (value(tmplist[0]).isNull() || !value(tmplist[0]).isValid()){
            return defaultVal;
        }
        QMap<QString, QVariant> parentVal = value(tmplist[0]).toMap();
        for(int iLoop = 1; iLoop < tmplist.size()-1; iLoop++){
            if (parentVal.value(tmplist[iLoop]).isNull() || !parentVal.value(tmplist[iLoop]).isValid()){
                return defaultVal;
            }
        }
        if (parentVal.value(tmplist[tmplist.size()-1]).isNull() || !parentVal.value(tmplist[tmplist.size()-1]).isValid()){
            return defaultVal;
        }
        return parentVal.value(tmplist[tmplist.size()-1]).toBool();
    } else {
        if (value(key).isNull() || !value(key).isValid()){
            return defaultVal;
        }
        return value(key).toBool();
    }
}

QStringList Settings::GetStringList(QString key)
{
    QStringList ret;
    if (key.contains("/")){
        QStringList tmplist = key.split("/");
        if (value(tmplist[0]).isNull() || !value(tmplist[0]).isValid()){
            return ret;
        }
        QMap<QString, QVariant> parentVal = value(tmplist[0]).toMap();
        for(int iLoop = 1; iLoop < tmplist.size()-1; iLoop++){
            if (parentVal.value(tmplist[iLoop]).isNull() || !parentVal.value(tmplist[iLoop]).isValid()){
                return ret;
            }
        }
        if (parentVal.value(tmplist[tmplist.size()-1]).isNull() || !parentVal.value(tmplist[tmplist.size()-1]).isValid()){
            return ret;
        }
        return parentVal.value(tmplist[tmplist.size()-1]).toStringList();
    } else {
        if (value(key).isNull() || !value(key).isValid()){
            return ret;
        }
        return value(key).toStringList();
    }
}

void Settings::Set(QString key, QVariant val)
{
    QStringList ret;
    if (key.contains("/")){
        QStringList tmplist = key.split("/");
        QMap<QString, QVariant> parentVal;
        if (value(tmplist[0]).isNull() || !value(tmplist[0]).isValid()){
            setValue(tmplist[0], parentVal);
        }
        parentVal = value(tmplist[0]).toMap();
        for(int iLoop = 1; iLoop < tmplist.size()-1; iLoop++){
            if (parentVal.value(tmplist[iLoop]).isNull() || !parentVal.value(tmplist[iLoop]).isValid()){
                parentVal[tmplist[iLoop]] = QMap<QString, QVariant>();
            }
        }
        parentVal[tmplist[tmplist.size()-1]] = val;
        setValue(tmplist[0], parentVal);
    } else {
        setValue(key, val);
    }
}

//template<typename T>
//T Settings::Get(QString key, T defaultVal)
//{
//    if (key.contains("/")){
//        QStringList tmplist = key.split("/");
//        if (value(tmplist[0]).isNull() || !value(tmplist[0]).isValid()){
//            return defaultVal;
//        }
//        QMap<QString, QVariant> parentVal = value(tmplist[0]).toMap();
//        for(int iLoop = 1; iLoop < tmplist.size()-1; iLoop++){
//            if (parentVal.value(tmplist[iLoop]).isNull() || !parentVal.value(tmplist[iLoop]).isValid()){
//                return defaultVal;
//            }
//        }
//        if (parentVal.value(tmplist[tmplist.size()-1]).isNull() || !parentVal.value(tmplist[tmplist.size()-1]).isValid()){
//            return defaultVal;
//        }
//        auto gettypename = typeid(T).name();
//        qDebug("gettypename=%s", gettypename);
//        if (strcmp(gettypename,"bool") == 0){
//            return parentVal.value(tmplist[tmplist.size()-1]).toBool();
//        } else if(strcmp(gettypename,"QString") == 0){
//            return parentVal.value(tmplist[tmplist.size()-1]).toString();
//        } else if(strcmp(gettypename,"QSize") == 0){
//            return parentVal.value(tmplist[tmplist.size()-1]).toSize();
//        } else if(strcmp(gettypename,"int") == 0){
//            return parentVal.value(tmplist[tmplist.size()-1]).toInt();
//        } else if(strcmp(gettypename,"QPoint") == 0){
//            return parentVal.value(tmplist[tmplist.size()-1]).toPoint();
//        } else if(strcmp(gettypename,"QPointF") == 0){
//            return parentVal.value(tmplist[tmplist.size()-1]).toPointF();
//        } else if(strcmp(gettypename,"QByteArray") == 0){
//            return parentVal.value(tmplist[tmplist.size()-1]).toByteArray();
//        } else {
//            qWarning("unsurported gettypename=%s", gettypename);
//            return defaultVal;
//        }
//    } else {
//        if (value(key).isNull() || !value(key).isValid()){
//            return defaultVal;
//        }
//        auto gettypename = typeid(T).name();
//        qDebug("gettypename=%s", gettypename);
//        if (strcmp(gettypename,"bool") == 0){
//            return value(key).toBool();
//        } else if(strcmp(gettypename,"QString") == 0){
//            return value(key).toString();
//        } else if(strcmp(gettypename,"QSize") == 0){
//            return value(key).toSize();
//        } else if(strcmp(gettypename,"int") == 0){
//            return value(key).toInt();
//        } else if(strcmp(gettypename,"QPoint") == 0){
//            return value(key).toPoint();
//        } else if(strcmp(gettypename,"QPointF") == 0){
//            return value(key).toPointF();
//        } else if(strcmp(gettypename,"QByteArray") == 0){
//            return value(key).toByteArray();
//        } else {
//            qWarning("unsurported gettypename=%s", gettypename);
//            return defaultVal;
//        }
//    }
//}

void Settings::outputLog(const QString &type, const char* file, const char* func, int line, const QString &msg)
{
    QMutexLocker locker(&m_LogMux);
    QDateTime now = QDateTime::currentDateTime();
    if (m_LastLogTime.daysTo(now) > 0 || nullptr == m_iLogFile){
        if (m_iLogStream != nullptr){
            m_iLogStream->flush();
            delete m_iLogStream;
            m_iLogStream = nullptr;
        }
        if (m_iLogFile != nullptr){
            m_iLogFile->flush();
            m_iLogFile->close();
            delete m_iLogFile;
            m_iLogFile = nullptr;
        }
        QDir dir(m_LogPath);
        if(!dir.exists()) dir.mkpath(m_LogPath);
        QString name = QString("%1/%2.txt").arg(m_LogPath).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd"));

        m_iLogFile = new QFile(name);
        if(!m_iLogFile->open(QIODevice::WriteOnly | QIODevice::Append)){
            delete m_iLogFile;
            m_iLogFile = nullptr;
            return;
        }
        m_iLogStream = new QTextStream(m_iLogFile);
        m_iLogStream->setCodec("utf-8");
    }
    QString time = now.toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString str = QString("[%1][%2 %3:%5][%4] ===> %6\n")
                      .arg(type).arg(file).arg(func).arg(time).arg(line).arg(msg);

    *m_iLogStream << str.toUtf8();
    printf("%s",str.toUtf8().toStdString().c_str());
}


