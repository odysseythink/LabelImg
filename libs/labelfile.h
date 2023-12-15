#ifndef LABELFILE_H
#define LABELFILE_H

#include <QObject>
#include <QList>
#include <QPointF>
#include <QSharedPointer>
#include "libs/shape.h"
#define XML_EXT  ".xml"
#define TXT_EXT  ".txt"

class LabelFile
{
public:
    LabelFile(QString filename = "");
    ~LabelFile();
    void savePascalVocFormat(QString filename, QList<QSharedPointer<Shape> >& shapes, QString imagePath, QByteArray imageData);
    void saveYoloFormat(QString filename, QList<QSharedPointer<Shape> >& shapes, QString imagePath, QByteArray imageData, QStringList classList);
    static void convertPoints2BndBox(QList<QPointF> points, int& xmin, int& ymin, int& xmax, int& ymax);
    static bool isLabelFile(QString filename);
    void toggleVerify();

public:
    static QString suffix;
    QList<QSharedPointer<Shape>>  shapes;
    QString imagePath;
    bool verified;
    QByteArray imageData;
};

#endif // LABELFILE_H
