#include "shape.h"
#include <QPen>
#include <QPainterPath>
#include <QDebug>
#include "common.h"

QColor Shape::line_color = DEFAULT_LINE_COLOR;
QColor Shape::fill_color = DEFAULT_FILL_COLOR;
QColor Shape::select_line_color = DEFAULT_SELECT_LINE_COLOR;
QColor Shape::select_fill_color = DEFAULT_SELECT_FILL_COLOR;
QColor Shape::vertex_fill_color = DEFAULT_VERTEX_FILL_COLOR;
QColor Shape::h_vertex_fill_color = DEFAULT_HVERTEX_FILL_COLOR;
int Shape::point_type = P_ROUND;
int Shape::point_size = 8;
float Shape::scale = 1.0;
int Shape::label_font_size = 8;
int Shape::difficult = 0;


Shape::Shape(QString label, QColor line_color, bool difficult, bool paint_label,QListWidget *parent)
    : QObject(parent), QListWidgetItem(label, parent), m_bVisible(true)
{
    this->label = label;
//    points = [];
    fill = false;
    selected = false;

    this->paintLabel = paint_label;
    Shape::difficult = difficult;
    _highlightIndex = -1;
    _highlightMode = NEAR_VERTEX;
    _highlightSettings[NEAR_VERTEX]= QPair<float, int>(4, P_ROUND);
    _highlightSettings[MOVE_VERTEX]= QPair<float, int>(1.5, P_SQUARE);

    _closed = false;

    if (line_color.isValid()){
//        # Override the class line_color attribute
//        # with an object attribute. Currently this
//        # is used for drawing the pending line a different color.
        Shape::line_color = line_color;
    } else {
        Shape::line_color = DEFAULT_LINE_COLOR;
    }

}

Shape::Shape(const Shape &s)
{
    if( this != &s){
        this->label = s.label;
        this->points = s.points;
        this->fill = s.fill;
        this->selected = s.selected;
        this->_closed = s._closed;
//        this->line_color = arr.line_color;
//        this->fill_color = arr.fill_color;
//        this->difficult = arr.difficult;
    }
}

Shape::~Shape()
{
    points.clear();
    _highlightSettings.clear();
}

void Shape::close(){
    _closed = true;
}
bool Shape::reachMaxPoints(){
    if (points.size() >= 4) return true;
    return false;
}
void Shape::addPoint(QPointF point){
    if(!reachMaxPoints())  points.append(point);
}
QPointF Shape::popPoint(){
    if (points.size()>0)
        return points.takeLast();
    return QPointF();
}
bool Shape::isClosed(){
    return _closed;
}
void Shape::setOpen(){
    _closed = false;
}
void Shape::paint(QPainter* painter){
    if (points.size() > 0){
        QColor color = selected? select_line_color:line_color;
        QPen pen(color);
//        # Try using integer sizes for smoother drawing(?)
        pen.setWidth(qMax(1, int(qRound(2.0 / Shape::scale))));
        painter->setPen(pen);

        QPainterPath line_path = QPainterPath();
        QPainterPath vertex_path = QPainterPath();

        line_path.moveTo(points[0]);
//        # Uncommenting the following line will draw 2 paths
//        # for the 1st vertex, and make it non-filled, which
//        # may be desirable.
//        # self.drawVertex(vertex_path, 0)
        for(int iLoop = 0; iLoop < points.size(); iLoop++){
            line_path.lineTo(points.at(iLoop));
            drawVertex(vertex_path, iLoop);
        }
        if (isClosed())
            line_path.lineTo(points[0]);

        painter->drawPath(line_path);
        painter->drawPath(vertex_path);
        painter->fillPath(vertex_path, vertex_fill_color);

//        # Draw text at the top-left
        if (paintLabel){
            double min_x = sys_maxsize;
            double min_y = sys_maxsize;
            double min_y_label = 1.25 * Shape::label_font_size;
            for (auto point : points){
                min_x = qMin(min_x, point.x());
                min_y = qMin(min_y, point.y());
            }
            if (min_x != sys_maxsize and min_y != sys_maxsize){
                QFont font = QFont();
                font.setPointSize(Shape::label_font_size);
                font.setBold(true);
                painter->setFont(font);
//                if self.label is None:
//                    self.label = "";
                if (min_y < min_y_label)
                    min_y += min_y_label;
                painter->drawText(min_x, min_y, label);
            }
        }

        if (fill){
            color = selected?select_fill_color:fill_color;
            painter->fillPath(line_path, color);
        }
    }

}
void Shape::drawVertex(QPainterPath& path, int i){
    float d = Shape::point_size / Shape::scale;
//    int shapeType = Shape::point_type;
    QPointF point = points[i];
    if (i == _highlightIndex){
//        size, shape = _highlight_settings[_highlight_mode];
        d *=  _highlightSettings[_highlightMode].first;
    }
    if (_highlightIndex >=0)
        Shape::vertex_fill_color = h_vertex_fill_color;
    else
        Shape::vertex_fill_color = Shape::vertex_fill_color;
    if (_highlightSettings[_highlightMode].second == P_SQUARE)
        path.addRect(point.x() - d / 2, point.y() - d / 2, d, d);
    else if(_highlightSettings[_highlightMode].second == P_ROUND)
        path.addEllipse(point, d / 2.0, d / 2.0);
    else
        qCritical("unsupported vertex shape");
}
int Shape::nearestVertex(QPointF point, double epsilon){
    for(int iLoop = 0; iLoop < points.size(); iLoop++){
        if (distance(points[iLoop] - point) <= epsilon)
            return iLoop;
    }

    return -1;
}
bool Shape::containsPoint(QPointF point){
    return makePath().contains(point);

}
QPainterPath Shape::makePath(){
    QPainterPath path(points[0]);
    for(int iLoop = 1; iLoop < points.size(); iLoop++){
        path.lineTo(points[iLoop]);
    }
    return path;
}
QRectF Shape::boundingRect(){
    return makePath().boundingRect();
}
void Shape::moveBy(QPointF offset){
    for(int iLoop = 0; iLoop < points.size(); iLoop++){
        points[iLoop] = points[iLoop] + offset;
    }
}
void Shape::moveVertexBy(int i, QPointF offset){
    points[i] = points[i] + offset;

}
void Shape::highlightVertex(int i, int action){
    _highlightIndex = i;
    _highlightMode = action;

} void Shape::highlightClear(){
    _highlightIndex = -1;

}

Shape &Shape::operator=(const Shape &arr){
    if( this != &arr){
        this->label = arr.label;
        this->points = arr.points;
        this->fill = arr.fill;
        this->selected = arr.selected;
        this->_closed = arr._closed;
//        this->line_color = arr.line_color;
//        this->fill_color = arr.fill_color;
//        this->difficult = arr.difficult;
    }
    return *this;
}

QPointF Shape::pointAt(int idx)
{
    if(idx < 0 && idx >= points.count()){
        qCritical("idx(%d) is beyond range(0,%d)", idx, points.count());
        return QPointF();
    }
    return points[idx];
}

void Shape::SetPoint(int idx, QPointF p)
{
    if(idx < 0 && idx >= points.count()){
        qCritical("idx(%d) is beyond range(0,%d)", idx, points.count());
        return;
    }
    points[idx] = p;
}

bool Shape::Visible() const
{
    return m_bVisible;
}

void Shape::SetVisible(bool visible)
{
    m_bVisible = visible;
}


