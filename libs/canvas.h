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
    explicit Canvas(QString *filePath, QWidget *parent = nullptr);
    ~Canvas();

    enum Mode{ CREATE, EDIT };
    void setDrawingColor(QColor qcolor);
    bool isVisible(Shape * shape);
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
    void hideBackroundShapes(bool value);
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
    void undoLastLine();
    void resetAllLines();
    void loadPixmap();
    void SetShapes(QList<QSharedPointer<Shape> > shapes);
    void SetShapeVisible(Shape * shape, bool value);
    void resetState();
    void SetMode(Mode mode);

public slots:
    void OnCreateShape();
    void OnDrawingShapeToSquare(bool status);
    void Paint(int val);

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
    void __RestoreCursor();
    void __OverrideCursor(Qt::CursorShape cursor);
    int __CurrentCursor();

signals:
    void zoomRequest(int);
    void scrollRequest(double, int);
    void newShape();
    void selectionChanged(bool);
    void shapeMoved();
    void drawingPolygon(bool);
    void labelCoordinatesMsg(QString);

public:
    //    # Menus:
    QMap<bool,QMenu*> menus;
    Shape *selectedShape;//  # save the selected shape here
    Shape *selectedShapeCopy;
    bool verified;
    float scale;
    QPixmap pixmap;
    QList<QSharedPointer<Shape> > shapes;
    QImage image;

private:
    QString *filePath;
    double epsilon;

    Mode m_enMode;
    QSharedPointer<Shape> current;
    QColor drawingLineColor;
    QColor drawingRectColor;
    QSharedPointer<Shape> line;
    QPointF prevPoint;
    QPair<QPointF, QPointF> offsets;
    int label_font_size;
    QMap<Shape *, bool> visible;
    bool _hideBackround;
    bool hideBackround;
    Shape *hShape;
    int hVertex;
    QPainter* _painter;
    Qt::CursorShape _cursor;
//    # Set widget options.
    bool drawSquare;

//    # initialisation for panning
    QPointF pan_initial_pos;
};

#endif // CANVAS_H
