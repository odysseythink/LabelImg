#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QStringList>
#include <QSettings>
#include <QDebug>
#include <QList>
#include <QMap>
#include <QMessageBox>
#include <QUuid>
#include <QFileInfo>
#include <QDir>
#include <QMutex>
#include <QDateTime>

#define SETTING_FILENAME  "filename"
#define SETTING_RECENT_FILES  "recentFiles"
#define SETTING_WIN_SIZE  "window/size"
#define SETTING_WIN_POSE  "window/position"
#define SETTING_WIN_GEOMETRY  "window/geometry"
#define SETTING_LINE_COLOR  "line/color"
#define SETTING_FILL_COLOR  "fill/color"
#define SETTING_ADVANCE_MODE  "advanced"
#define SETTING_WIN_STATE  "window/state"
#define SETTING_SAVE_DIR  "savedir"
#define SETTING_PAINT_LABEL  "paintlabel"
#define SETTING_LAST_OPEN_DIR  "lastOpenDir"
#define SETTING_AUTO_SAVE  "autosave"
#define SETTING_SINGLE_CLASS  "singleclass"
#define FORMAT_PASCALVOC "PascalVOC"
#define FORMAT_YOLO "YOLO"
#define SETTING_DRAW_SQUARE   "draw/square"
#define DEFAULT_ENCODING  "utf-8"
#define SETTING_LABEL_HIST  "label_histories"


void LogHandler(QtMsgType type, const QMessageLogContext &info, const QString &msg);



class Settings : public QSettings
{
public:
    static Settings* GetInstance();
    void outputLog(const QString &type, const char* file, const char* func, int line, const QString &msg);
    ~Settings();
    bool GetBool(QString key, bool defaultVal);
    QStringList GetStringList(QString key);
    template <typename T> T Get(QString key, T defaultVal)
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
            auto gettypename = typeid(T).name();
            qDebug("gettypename=%s", gettypename);
            return qvariant_cast<T>(parentVal.value(tmplist[tmplist.size()-1]));
//            if (strcmp(gettypename,"bool") == 0){
//                return parentVal.value(tmplist[tmplist.size()-1]).toBool();
//            } else if(strcmp(gettypename,"QString") == 0){
//                return parentVal.value(tmplist[tmplist.size()-1]).toString();
//            } else if(strcmp(gettypename,"QSize") == 0){
//                return parentVal.value(tmplist[tmplist.size()-1]).toSize();
//            } else if(strcmp(gettypename,"int") == 0){
//                return parentVal.value(tmplist[tmplist.size()-1]).toInt();
//            } else if(strcmp(gettypename,"QPoint") == 0){
//                return parentVal.value(tmplist[tmplist.size()-1]).toPoint();
//            } else if(strcmp(gettypename,"QPointF") == 0){
//                return parentVal.value(tmplist[tmplist.size()-1]).toPointF();
//            } else if(strcmp(gettypename,"QByteArray") == 0){
//                return parentVal.value(tmplist[tmplist.size()-1]).toByteArray();
//            } else {
//                qWarning("unsurported gettypename=%s", gettypename);
//                return defaultVal;
//            }
        } else {
            if (value(key).isNull() || !value(key).isValid()){
                return defaultVal;
            }
            auto gettypename = typeid(T).name();
            qDebug("gettypename=%s", gettypename);
//            if (strcmp(gettypename,"bool") == 0){
//                return value(key).toBool();
//            } else if(strcmp(gettypename,"QString") == 0){
//                return value(key).toString();
//            } else if(strcmp(gettypename,"QSize") == 0){
//                return value(key).toSize();
//            } else if(strcmp(gettypename,"int") == 0){
//                return value(key).toInt();
//            } else if(strcmp(gettypename,"QPoint") == 0){
//                return value(key).toPoint();
//            } else if(strcmp(gettypename,"QPointF") == 0){
//                return value(key).toPointF();
//            } else if(strcmp(gettypename,"QByteArray") == 0){
//                return value(key).toByteArray();
//            } else {
//                qWarning("unsurported gettypename=%s", gettypename);
//                return defaultVal;
//            }
            return qvariant_cast<T>(value(key));
        }
    }
    void Set(QString key, QVariant val);

private:
    Settings();

public:
    static QString APP_NAME;
    static QString APP_VERSION;


private:
    static Settings* m_iInstance;
    QMutex          m_LogMux;
    QString m_LogPath;
    QDateTime m_LastLogTime;
    QFile* m_iLogFile;
    QTextStream* m_iLogStream;
};

#endif // SETTINGS_H


