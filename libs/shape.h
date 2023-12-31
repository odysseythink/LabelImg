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
#include <QMetaType>
#include <QListWidgetItem>
#include <QListWidget>

#define sys_maxsize 2147483647

#define DEFAULT_LINE_COLOR  QColor(0, 255, 0, 128)
#define DEFAULT_FILL_COLOR  QColor(255, 0, 0, 128)
#define DEFAULT_SELECT_LINE_COLOR  QColor(255, 255, 255)
#define DEFAULT_SELECT_FILL_COLOR  QColor(0, 128, 255, 155)
#define DEFAULT_VERTEX_FILL_COLOR  QColor(0, 255, 0, 255)
#define DEFAULT_HVERTEX_FILL_COLOR  QColor(255, 0, 0)

class Shape : public QObject, public QListWidgetItem
{
    Q_OBJECT
public:
    explicit Shape(QString label="", QColor line_color=QColor(), bool difficult=false, bool paintLabel=false,QListWidget *parent = nullptr);
    Shape(const Shape& s);
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
    QPointF pointAt(int idx);
    QPointF lastPoint(){
        return pointAt(pointCount()-1);
    }
    int pointCount(){
        return points.count();
    }
    void SetPoint(int idx, QPointF p);
    bool Visible() const;
    void SetVisible(bool visible);

signals:

private:
    int _highlightIndex;
    int _highlightMode;
    QMap<int, QPair<float, int> > _highlightSettings;

    bool _closed;
    bool m_bVisible;

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
    bool selected;
    bool fill;
};

Q_DECLARE_METATYPE(Shape);

#endif // SHAPE_H
