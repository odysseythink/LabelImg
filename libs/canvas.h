#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPair>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QCursor>
#include <memory>
#include <QSharedPointer>
#include "libs/shape.h"

#define CURSOR_DEFAULT  Qt::ArrowCursor
#define CURSOR_POINT  Qt::PointingHandCursor
#define CURSOR_DRAW  Qt::CrossCursor
#define CURSOR_MOVE  Qt::ClosedHandCursor
#define CURSOR_GRAB  Qt::OpenHandCursor

class Canvas : public QWidget
{
    Q_OBJECT

public:
    explicit Canvas(QString *m_iFilePath, QWidget *parent = nullptr);
    ~Canvas();

    enum Mode{ CREATE, EDIT };
    void setDrawingColor(QColor qcolor);
    bool drawing();
    bool editing();
    void setEditing(bool value=true);
    void unHighlight();
    bool selectedVertex();
    QPointF transformPos(QPointF point);
    QPointF offsetToCenter();
    bool outOfPixmap(QPointF p);
    void finalise();
    bool closeEnough(QPointF p1,QPointF p2);

    virtual QSize sizeHint() const;
//    virtual QSize minimumSizeHint() const;
    void endMove(bool copy=false);
    void handleDrawing(QPointF pos);
    void setHiding(bool enable=true);
    bool canCloseShape();
    void selectShape(Shape* shape);
    void selectShapePoint(QPointF point);
    void calculateOffsets(Shape* shape,QPointF point);
    void boundedMoveVertex(QPointF pos);
    bool boundedMoveShape(Shape* shape,QPointF pos);
    void deSelectShape();
    Shape* deleteSelected();
    Shape* copySelectedShape();
    void boundedShiftShape(Shape* shape);
    QPointF intersectionPoint(QPointF p1, QPointF p2);
    void minIntersectingEdges(double x1,double y1,double x2,double y2, QList<QPointF> points, double &d, int &i, QPointF xy);
    void moveOnePixel(QString direction);
    bool moveOutOfBound(QPointF step);
    Shape* setLastLabel(QString text, QColor line_color  = QColor(), QColor fill_color = QColor());
    void UndoLastLine();
    void ResetAllLines();
    void SetShapes(QList<QSharedPointer<Shape> > shapes);
    void resetState();
    void SetMode(Mode mode);
    Shape* GetSelectedShape(){
        if (m_nSelectedShape == -1){
            return nullptr;
        }
        return shapes[m_nSelectedShape].get();
    }
    void SetSelectedShapeLineColor(QColor& color){
        if (m_nSelectedShape == -1){
            return;
        }
        shapes[m_nSelectedShape]->line_color = color;
        update();
    }
    void SetSelectedShapeFillColor(QColor& color){
        if (m_nSelectedShape == -1){
            return;
        }
        shapes[m_nSelectedShape]->line_color = color;
        update();
    }
    bool SetImage(QString imgfilename){
        auto image = new QImage(imgfilename);
        if (image->isNull()){
            return false;
        }
        if (m_iImage != nullptr){
            delete m_iImage;
            m_iImage = nullptr;
        }
        m_iImage = image;
        m_strImageFilename = imgfilename;
        __LoadPixmap();
        setEnabled(true);
        return true;
    }
    QString GetImageFilename(){
        return m_strImageFilename;
    }

public slots:
    void OnCreateShape();
    void OnDrawingShapeToSquare(bool status);
    void Paint(int val);
    void OnChangeShapeLineColor();
    void OnChangeShapeFillColor();

protected:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void focusOutEvent(QFocusEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent * ev);
    virtual void mouseReleaseEvent(QMouseEvent * ev);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

private:
    void __LoadPixmap();
    void __RestoreCursor();
    void __OverrideCursor(Qt::CursorShape cursor);
    int __CurrentCursor();

signals:
    void zoomRequest(int);
    void scrollRequest(double, int);
    void sigNewShape();
    void sigSelectionChanged(bool);
    void sigShapeSelected(Shape*);
    void shapeMoved();
    void drawingPolygon(bool);
    void labelCoordinatesMsg(QString);
    void sigDirty();

public:
    //    # Menus:
    QMap<bool,QMenu*> menus;
    bool verified;
    QPixmap pixmap;
    QList<QSharedPointer<Shape> > shapes;


private:
    float m_nScale;
    QString m_strImageFilename;
    QImage* m_iImage;
    QSharedPointer<Shape> m_iSelectedShapeCopy;
    QString *m_iFilePath;
    double m_nEpsilon;
    int m_nSelectedShape;//  # save the selected shape here
    Mode m_enMode;
    QSharedPointer<Shape> m_iCurrentShape;
    QColor m_DrawingLineColor;
    QColor m_DrawingRectColor;
    QSharedPointer<Shape> m_iCurrentLineShape;
    QPointF m_PrevPoint;
    QPointF m_arrayOffsets[2];
    int m_nlabelFontSize;
    bool m_bHideBackround;
    int m_nHighlightShape;
    int m_nHightlightVertex;
    QPainter* m_iPainter;
    Qt::CursorShape m_CursorShape;
//    # Set widget options.
    bool m_bDrawSquare;

//    # initialisation for panning
    QPointF m_PanInitialPos;
};

#endif // CANVAS_H
