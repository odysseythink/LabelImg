#ifndef SHAPE_H
#define SHAPE_H

#include <QObject>
#include <QColor>
#include <QList>
#include <QPoint>
#include <QPointF>
#include <QMap>
#include <QPair>
#include <QPainter>
#include <QPainterPath>

#define sys_maxsize 2147483647

#define DEFAULT_LINE_COLOR  QColor(0, 255, 0, 128)
#define DEFAULT_FILL_COLOR  QColor(255, 0, 0, 128)
#define DEFAULT_SELECT_LINE_COLOR  QColor(255, 255, 255)
#define DEFAULT_SELECT_FILL_COLOR  QColor(0, 128, 255, 155)
#define DEFAULT_VERTEX_FILL_COLOR  QColor(0, 255, 0, 255)
#define DEFAULT_HVERTEX_FILL_COLOR  QColor(255, 0, 0)

class Shape : public QObject
{
    Q_OBJECT
public:
    explicit Shape(QString label="", QColor line_color=QColor(), bool difficult=false, bool paintLabel=false,QObject *parent = nullptr);
    ~Shape();

    enum {P_SQUARE, P_ROUND};

    enum {MOVE_VERTEX, NEAR_VERTEX};
    void close();
    bool reachMaxPoints();
    void addPoint(QPointF point);
    QPointF popPoint();
    bool isClosed();
    void setOpen();
    void paint(QPainter* painter);
    void drawVertex(QPainterPath& path, int i);
    int nearestVertex(QPointF point, double epsilon);
    bool containsPoint(QPointF point);
    QPainterPath makePath();
    QRectF boundingRect();
    void moveBy(QPointF offset);
    void moveVertexBy(int i, QPointF offset);
    void highlightVertex(int i, int action);
    void highlightClear();
    Shape &operator=(const Shape &arr);

signals:

private:
    int _highlightIndex;
    int _highlightMode;
    QMap<int, QPair<float, int> > _highlightSettings;

    bool _closed;

public:
   bool paintLabel;
    QList<QPointF> points;
    static QColor line_color;
    static QColor fill_color;
    static QColor select_line_color;
    static QColor select_fill_color;
    static QColor vertex_fill_color;
    static QColor h_vertex_fill_color;
    static int point_type;
    static int point_size;
    static float scale;
    static int label_font_size;  
    static int difficult;

    QString label;
    bool selected;
    bool fill;
};

#endif // SHAPE_H
