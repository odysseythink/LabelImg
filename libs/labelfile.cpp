#include "labelfile.h"
#include <QFileInfo>
#include <QCompleter>
#include <QImage>
#include <limits.h>
#include <QtGlobal>
#include <QDebug>
#include "go_interface.h"


QString LabelFile::suffix = XML_EXT;

LabelFile::LabelFile(QString filename)
{
//    self.shapes = ()
    imagePath = "";
//    self.imageData = None;
    verified = false;
}

LabelFile::~LabelFile()
{

}

void BndBox2YoloLine(QImage& img, int xmin, int ymin, int xmax, int ymax, QString label, QStringList& classList, int& classIndex, float& xcen, float& ycen, float& w, float& h){
    xcen = float((xmin + xmax)) / 2 / img.width();
    ycen = float((ymin + ymax)) / 2 / img.height();

    w = float((xmax - xmin)) / img.width();
    h = float((ymax - ymin)) / img.height();

//    # PR387
    if (!classList.contains(label))
        classList.append(label);

    classIndex = classList.indexOf(label);
}

void LabelFile::convertPoints2BndBox(QList<QPointF> points, int& xmin, int& ymin, int& xmax, int& ymax){
    auto d_xmin = double(INT_MAX);
    auto d_ymin = double(INT_MAX);
    auto d_xmax = double(INT_MIN);
    auto d_ymax = double(INT_MIN);
    for (auto p : points){
        auto x = p.x();
        auto y = p.y();
        d_xmin = qMin(x, d_xmin);
        d_ymin = qMin(y, d_ymin);
        d_xmax = qMax(x, d_xmax);
        d_ymax = qMax(y, d_ymax);
    }

//    # Martin Kersner, 2015/11/12
//    # 0-valued coordinates of BB caused an error while
//    # training faster-rcnn object detector.
    if (d_xmin < 1)
        d_xmin = 1;

    if (d_ymin < 1)
        d_ymin = 1;

    xmin = int(d_xmin);
    ymin = int(d_ymin);
    xmax = int(d_xmax);
    ymax = int(d_ymax);
}

void LabelFile::savePascalVocFormat(QString filename, QList<QSharedPointer<Shape> >& shapes, QString imagePath, QByteArray imageData){

    QFileInfo fi(imagePath);
    auto imgFolderPath = fi.path();
    QCompleter cp;
    auto tmplist = cp.splitPath(imgFolderPath);
    auto imgFolderName = tmplist[tmplist.size() - 1];
    auto imgFileName = fi.fileName();
    qDebug() << QString("----imagePath=%1, imgFolderPath=%2, imgFolderName=%3,imgFileName=%4").arg(imagePath).arg(imgFolderPath).arg(imgFolderName).arg(imgFileName);
//    #imgFileNameWithoutExt = os.path.splitext(imgFileName)[0]
//    # Read from file path because self.imageData might be empty if saving to
//    # Pascal format
    QImage image(imagePath);

    pb::PK_PASCAL_VOC_SAVE_REQ req;
    pb::PK_PASCAL_VOC_SAVE_RSP rsp;
//    req.set_host(ui->m_iHostEdit->text().toStdString());
//    req.set_port(ui->m_iPortSpinBox->value());
//    req.set_username(ui->m_iUsernameEdit->text().toStdString());
//    req.set_passwd(ui->m_iPasswdEdit->text().toStdString());
//    req.set_dbname(ui->m_iDatabaseNameEdit->text().toStdString());


    pb::PK_PASCAL_VOC_ANNOTATION* anotation = req.mutable_annotation();
    anotation->set_folder(imgFolderPath.toStdString());
    anotation->set_filename(imgFileName.toStdString());
    anotation->set_path(imagePath.toStdString());
    anotation->set_sourcedatabase("Unknown");
    anotation->set_szwidth(image.width());
    anotation->set_szheight(image.height());
    anotation->set_szdepth(image.isGrayscale()?1:3);
    anotation->set_segmented(0);
    for (auto shape : shapes){
        pb::PK_PASCAL_VOC_ANNOTATION_OBJ* obj = anotation->add_objs();
        obj->set_objname(shape->label.toStdString());
        obj->set_objpose("Unspecified");
        int xmin,ymin,xmax,ymax;
        LabelFile::convertPoints2BndBox(shape->points, xmin,ymin,xmax,ymax);
        if (ymax == image.height() || ymin == 1)
            obj->set_objtruncated(1);//      truncated.text = "1" # max == height or min
        else if (xmax ==image.width() || xmin == 1)
            obj->set_objtruncated(1);//truncated.text = "1" # max == width or min
        else
            obj->set_objtruncated(0);//truncated.text = "0"
        obj->set_objdifficult(shape->difficult);
        obj->set_bndboxxmin(xmin);
        obj->set_bndboxxmax(xmax);
        obj->set_bndboxymin(ymin);
        obj->set_bndboxymax(ymax);
    }

    req.set_filename(filename.toStdString());
    Call_Go_Func(&req, &rsp, PascalVocSave);
    return;
}

void LabelFile::saveYoloFormat(QString filename, QList<QSharedPointer<Shape> >& shapes, QString imagePath, QByteArray imageData, QStringList classList){
    QFileInfo fi(imagePath);
    auto imgFolderPath = fi.path();
    QCompleter cp;
    auto tmplist = cp.splitPath(imgFolderPath);
    auto imgFolderName = tmplist[tmplist.size() - 1];
    auto imgFileName = fi.fileName();

    QFile file;
    QFile classesFile;
    if (filename == ""){
        file.setFileName(imgFolderPath+"/"+ fi.completeBaseName()+TXT_EXT);
        classesFile.setFileName(imgFolderPath+"/classes.txt");

    }else{
        file.setFileName(filename);
        classesFile.setFileName(QFileInfo(filename).path()+"/classes.txt");
    }

    if(!file.open(QIODevice::WriteOnly)){
        qCritical("open file(%s) failed:%s", file.fileName().toStdString().c_str(), file.errorString().toStdString().c_str());
        return;
    }
    if(!classesFile.open(QIODevice::WriteOnly|QIODevice::Truncate)){
        file.close();
        qCritical("open file(%s) failed:%s", classesFile.fileName().toStdString().c_str(), file.errorString().toStdString().c_str());
        return;
    }
    QTextStream filestream(&file);
    QTextStream classesFileStream(&classesFile);
    filestream.setCodec("utf-8");
    classesFileStream.setCodec("utf-8");

//    #imgFileNameWithoutExt = os.path.splitext(imgFileName)[0]
//    # Read from file path because self.imageData might be empty if saving to
//    # Pascal format
    QImage img(imagePath);
//    imageShape = [image.height(), image.width(),
//                  1 if image.isGrayscale() else 3]

    for (auto shape : shapes){
        int xmin,ymin,xmax,ymax;
        LabelFile::convertPoints2BndBox(shape->points, xmin,ymin,xmax,ymax);
        int classIndex = 0;
        float xcen = 0., ycen = 0., w = 0., h = 0.;
        BndBox2YoloLine(img, xmin, ymin, xmax, ymax, shape->label, classList, classIndex, xcen, ycen, w, h);
        filestream << QString::asprintf("%d %.6f %.6f %.6f %.6f\n",classIndex, xcen, ycen, w, h);
    }
    for(auto c : classList){
        classesFileStream << c << "\n";
    }

    filestream.flush();
    file.close();
    classesFileStream.flush();
    classesFile.close();
    return;
}
bool LabelFile::isLabelFile(QString filename){
    return filename.toLower().endsWith(LabelFile::suffix);
}

void LabelFile::toggleVerify(){
    verified = ! verified;
}
