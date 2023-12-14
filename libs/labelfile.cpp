#include "labelfile.h"
#include <QFileInfo>
#include <QCompleter>
#include <QImage>
#include <limits.h>
#include <QtGlobal>
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

void LabelFile::convertPoints2BndBox(QList<QPointF> points, int& xmin, int& ymin, int& xmax, int& ymax){
    auto d_xmin = double(INT_MAX);
    auto d_ymin = double(INT_MAX);
    auto d_xmax = double(INT_MIN);
    auto d_ymax = double(INT_MIN);
    for (auto p : points){
        auto x = p.x();
        auto y = p.y();
        xmin = qMin(x, d_xmin);
        ymin = qMin(y, d_ymin);
        xmax = qMax(x, d_xmax);
        ymax = qMax(y, d_ymax);
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

void LabelFile::savePascalVocFormat(QString filename, QList<Shape*>& shapes, QString imagePath, QByteArray imageData){
    QFileInfo fi(imagePath);
    auto imgFolderPath = fi.path();
    QCompleter cp;
    auto tmplist = cp.splitPath(imgFolderPath);
    auto imgFolderName = tmplist[tmplist.size() - 1];
    auto imgFileName = fi.fileName();
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


    pb::PK_PASCAL_VOC_ANNOTATION* anotation = req.mutable_annotation();;
    anotation->set_folder(imgFolderName.toStdString());
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


bool LabelFile::isLabelFile(QString filename){
    return filename.toLower().endsWith(LabelFile::suffix);
}

void LabelFile::toggleVerify(){
    verified = ! verified;
}
