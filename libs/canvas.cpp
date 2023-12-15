#include "canvas.h"
#include "common.h"
#include <QtGlobal>
#include <QApplication>
#include "libs/color_dialog.h"

Canvas::Canvas(QString *filePath, QWidget *parent)
    : QWidget(parent), m_nEpsilon(11.0)
{
    this->m_iFilePath = filePath;
    m_enMode = Canvas::EDIT;
//    shapes = [];
    m_iCurrentShape = nullptr;
    m_nSelectedShape = -1;//  # save the selected shape here
    m_iSelectedShapeCopy.clear();
    m_DrawingLineColor = QColor(0, 0, 255);
    m_DrawingRectColor = QColor(0, 0, 255);
    m_iCurrentLineShape = QSharedPointer<Shape>(new Shape("",m_DrawingLineColor), &QObject::deleteLater);
    m_PrevPoint = QPointF();
    m_arrayOffsets[0]= QPointF();
    m_arrayOffsets[1]= QPointF();
    scale = 1.0;
    pixmap = QPixmap();
    m_bHideBackround = false;
    m_nHighlightShape = -1;
    m_nHightlightVertex = -1;
    m_iPainter = new QPainter();
    m_CursorShape = CURSOR_DEFAULT;
//    # Menus:
//    self.menus = (QMenu(), QMenu());
    menus[true] = new QMenu();
    menus[false] = new QMenu();
//    # Set widget options.
    setMouseTracking(true);
    setFocusPolicy(Qt::WheelFocus);
    verified = false;
    m_bDrawSquare = false;
    m_iImage = nullptr;
}

Canvas::~Canvas()
{

}


void Canvas::setDrawingColor(QColor qcolor){
    m_DrawingLineColor = qcolor;
    m_DrawingRectColor = qcolor;
}
void Canvas::enterEvent(QEvent *ev){
    __OverrideCursor(m_CursorShape);
}
void Canvas::leaveEvent(QEvent *ev){
    __RestoreCursor();
}
void Canvas::focusOutEvent(QFocusEvent *ev){
    __RestoreCursor();
}


bool Canvas::drawing(){
    return m_enMode == Canvas::CREATE;

}
bool Canvas::editing(){
    return m_enMode == Canvas::EDIT;
}
void Canvas::setEditing(bool value){
    m_enMode = value?Canvas::EDIT:Canvas::CREATE;
    if(!value){  //# Create
        unHighlight();
        deSelectShape();
    }
    m_PrevPoint = QPointF();
    repaint();
}

void Canvas::SetMode(Mode mode){
    m_enMode = mode;
    if(mode == CREATE){  //# Create
        unHighlight();
        deSelectShape();
    } else if(mode == EDIT) {
        __RestoreCursor();
    }
    m_PrevPoint = QPointF();
    repaint();
}

void Canvas::unHighlight(){
    if (m_nHighlightShape >=0)
        shapes[m_nHighlightShape]->highlightClear();
    m_nHightlightVertex = -1;
    m_nHighlightShape = -1;

}
bool Canvas::selectedVertex(){
    return m_nHightlightVertex != -1;
}

QPointF Canvas::transformPos(QPointF point){
//    """Convert from widget-logical coordinates to painter-logical coordinates."""
    return point / scale - offsetToCenter();
}

QPointF Canvas::offsetToCenter(){
    float s = scale;
    QSize area = size();
    double w = pixmap.width() * s;
    double h = pixmap.height() * s;
    double aw = area.width();
    double ah = area.height();
    double x = aw > w?((aw - w) / (2 * s)):0;
    double y = ah > h ? ((ah - h) / (2 * s)) :0;
    return QPointF(x, y);

}
bool Canvas::outOfPixmap(QPointF p){
    double w= pixmap.width();
    double h =pixmap.height();
    return !(0 <= p.x() <= w and 0 <= p.y() <= h);

}
void Canvas::finalise(){
    if (m_iCurrentShape == nullptr){
        qCritical("no current");
        return;
    }
    if (m_iCurrentShape.get()->pointAt(0) == m_iCurrentShape.get()->lastPoint()){
        m_iCurrentShape = nullptr;
        emit drawingPolygon(false);
        update();
        return;
    }

    m_iCurrentShape.get()->close();
    shapes.append(QSharedPointer<Shape>(m_iCurrentShape));
    m_iCurrentShape.clear();
    m_iCurrentLineShape->points.clear();
    setHiding(false);
    emit newShape();
    update();
}
bool Canvas::closeEnough(QPointF p1,QPointF p2){
//    # d = distance(p1 - p2)
//    # m = (p1-p2).manhattanLength()
//    # print "d %.2f, m %d, %.2f" % (d, m, d - m)
    return distance(p1 - p2) < m_nEpsilon;

//# These two, along with a call to adjustSize are required for the
//# scroll area.
}
QSize Canvas::sizeHint() const{
    return minimumSizeHint();
}
//QSize Canvas::minimumSizeHint() const{
////    if (!pixmap.isNull())
////        return scale * pixmap.size();
//    return minimumSizeHint();
//}

void Canvas::mouseMoveEvent(QMouseEvent * ev){
//    """Update line with last point and current coordinates."""
   QPointF pos = transformPos(ev->pos());

//    # Update coordinates in status bar if image is opened
//    window = self.parent().window();
    if (m_iFilePath != nullptr && *m_iFilePath != "")
        emit labelCoordinatesMsg(QString("X: %1; Y: %2").arg(pos.x()).arg(pos.y()));

//    # Polygon drawing.
    if (drawing()){
        __OverrideCursor(CURSOR_DRAW);
        if (!m_iCurrentShape.isNull()){
            qDebug("-----------current.pointCount()=%d", m_iCurrentLineShape->pointCount());
//            # Display annotation width and height while drawing
            double current_width = qAbs(m_iCurrentShape.get()->pointAt(0).x() - pos.x());
            double current_height = qAbs(m_iCurrentShape.get()->pointAt(0).y() - pos.y());
            emit labelCoordinatesMsg(QString("Width: %1, Height: %2 / X: %3; Y: %4").arg(current_width).arg(current_height).arg(pos.x()).arg(pos.y()));

            QColor color = m_DrawingLineColor;
            if (outOfPixmap(pos)){
//                # Don't allow the user to draw outside the pixmap->
//                # Clip the coordinates to 0 or max,
//                # if they are outside the range [0, max]
                QSize size = pixmap.size();
                double clipped_x = qMin(qMax(double(0), pos.x()), double(size.width()));
                double clipped_y = qMin(qMax(double(0), pos.y()), double(size.height()));
                pos = QPointF(clipped_x, clipped_y);
            }else if (m_iCurrentShape->pointCount() > 1 && closeEnough(pos, m_iCurrentShape->pointAt(0))){
//                # Attract line to starting point and colorise to alert the
//                # user:
                QPointF pos = m_iCurrentShape->pointAt(0);
                QColor color = m_iCurrentShape->line_color;
                __OverrideCursor(CURSOR_POINT);
                m_iCurrentShape->highlightVertex(0, Shape::NEAR_VERTEX);
            }

            if (m_bDrawSquare){
                QPointF init_pos = m_iCurrentShape->pointAt(0);
                double min_x = init_pos.x();
                double min_y = init_pos.y();
                double min_size = qMin(qAbs(pos.x() - min_x), qAbs(pos.y() - min_y));
                double direction_x = pos.x() - min_x < 0 ? -1 : 1;
                double direction_y = pos.y() - min_y < 0 ? -1 : 1;
                m_iCurrentLineShape->SetPoint(1,QPointF(min_x + direction_x * min_size, min_y + direction_y * min_size));
            }else{
                m_iCurrentLineShape->SetPoint(1,pos);
            }

            m_iCurrentLineShape->line_color = color;
            m_PrevPoint = QPointF();
            m_iCurrentShape->highlightClear();
        }else{
            m_PrevPoint = pos;
        }
        repaint();
        return;
    }

//    # Polygon copy moving.
    if (Qt::RightButton & ev->buttons()){
        if (!m_iSelectedShapeCopy.isNull() && m_PrevPoint.isNull()){
            __OverrideCursor(CURSOR_MOVE);
            boundedMoveShape(m_iSelectedShapeCopy.get(), pos);
            repaint();
        }else if (m_nSelectedShape >=0){
            if (m_iSelectedShapeCopy.isNull()) m_iSelectedShapeCopy = QSharedPointer<Shape>(new Shape);
            *m_iSelectedShapeCopy = *shapes[m_nSelectedShape].get();
            repaint();
        }
        return;
    }

//    # Polygon/Vertex moving.
    if (Qt::LeftButton & ev->buttons()){
        if (selectedVertex()){
            boundedMoveVertex(pos);
            emit shapeMoved();
            repaint();

//            # Display annotation width and height while moving vertex
            if(m_nHighlightShape >= 0){
                QPointF point1 = shapes[m_nHighlightShape]->pointAt(1);
                QPointF point3 = shapes[m_nHighlightShape]->pointAt(3);
                double current_width = qAbs(point1.x() - point3.x());
                double current_height = qAbs(point1.y() - point3.y());
                emit labelCoordinatesMsg(QString("Width: %1, Height: %2 / X: %3; Y: %4").arg(current_width).arg(current_height).arg(pos.x()).arg(pos.y()));
            }
        }else if (m_nSelectedShape >= 0 && m_PrevPoint.isNull()){
            __OverrideCursor(CURSOR_MOVE);
            boundedMoveShape(shapes[m_nSelectedShape].get(), pos);
            emit shapeMoved();
            repaint();

//            # Display annotation width and height while moving shape
            QPointF point1 = shapes[m_nSelectedShape]->pointAt(1);
            QPointF point3 = shapes[m_nSelectedShape]->pointAt(3);
            double current_width = qAbs(point1.x() - point3.x());
            double current_height = qAbs(point1.y() - point3.y());
            emit labelCoordinatesMsg(QString("Width: %d, Height: %d / X: %d; Y: %d").arg(current_width).arg(current_height).arg(pos.x()).arg(pos.y()));
        }else{
//            # pan
            double delta_x = pos.x() - m_PanInitialPos.x();
            double delta_y = pos.y() - m_PanInitialPos.y();
            emit scrollRequest(delta_x, Qt::Horizontal);
            emit scrollRequest(delta_y, Qt::Vertical);
            update();
        }
        return;
    }

//    # Just hovering over the canvas, 2 possibilities:
//    # - Highlight shapes
//    # - Highlight vertex
//    # Update shape/vertex fill and tooltip value accordingly.
    setToolTip("Image");
    bool isHightLight =false;
    for(int iLoop = shapes.size() - 1; iLoop >= 0; iLoop--){
        if (shapes.at(iLoop)->Visible()){
            isHightLight = true;
            Shape* shape = shapes.at(iLoop).get();

            //        # Look for a nearby vertex to highlight. If that fails,
            //        # check if we happen to be inside a shape.
            int index = shape->nearestVertex(pos, m_nEpsilon);
            if (index >= 0){
                if (selectedVertex())
                    shapes[m_nHighlightShape]->highlightClear();
                m_nHightlightVertex = index;
                m_nHighlightShape = iLoop;
                shape->highlightVertex(index, Shape::MOVE_VERTEX);
                __OverrideCursor(CURSOR_POINT);
                setToolTip("Click & drag to move point");
                setStatusTip(toolTip());
                update();
                break;
            }else if (shape->containsPoint(pos)){
                if (selectedVertex())
                    shapes[m_nHighlightShape]->highlightClear();
                m_nHightlightVertex = -1;
                m_nHighlightShape = iLoop;
                setToolTip(QString("Click & drag to move shape '%s'").arg(shape->label));
                setStatusTip(toolTip());
                __OverrideCursor(CURSOR_GRAB);
                update();
                break;
            }
        }
    }
    if (!isHightLight){ //# Nothing found, clear highlights, reset state.
        if (m_nHighlightShape >= 0){
            shapes[m_nHighlightShape]->highlightClear();
            update();
        }
        m_nHightlightVertex = -1;
        m_nHighlightShape = -1;
        __OverrideCursor(CURSOR_DEFAULT);
    }
}
void Canvas::mousePressEvent(QMouseEvent * ev){
    QPointF pos = transformPos(ev->pos());

    if (ev->button() == Qt::LeftButton){
        if (drawing())
            handleDrawing(pos);
        else{
            selectShapePoint(pos);
            m_PrevPoint = pos;
            repaint();
        }
    }else if (ev->button() == Qt::RightButton && editing()){
        selectShapePoint(pos);
        m_PrevPoint = pos;
        repaint();
    }
}
void Canvas::mouseReleaseEvent(QMouseEvent * ev){
    if (ev->button() == Qt::RightButton){
        QMenu* menu = menus[!m_iSelectedShapeCopy.isNull()];
        __RestoreCursor();
        if (! menu->exec(mapToGlobal(ev->pos())) && !m_iSelectedShapeCopy.isNull()){
//            # Cancel the move by deleting the shadow copy.
            m_iSelectedShapeCopy.clear();
            repaint();
        }
    }else if (ev->button() == Qt::LeftButton && m_nSelectedShape >= 0){
        if (selectedVertex())
            __OverrideCursor(CURSOR_POINT);
        else
            __OverrideCursor(CURSOR_GRAB);
    }else if (ev->button() == Qt::LeftButton){
        QPointF pos = transformPos(ev->pos());
        if (drawing())
            handleDrawing(pos);
    }
}
void Canvas::endMove(bool copy){
    if(m_nSelectedShape == -1 || m_iSelectedShapeCopy.isNull()){
        qCritical("invalid selectedShape or selectedShapeCopy");
        return;
    }
    Shape* shape = m_iSelectedShapeCopy.get();
//    #del shape.fill_color
//    #del shape.line_color
    if (copy){
        shapes.push_back(QSharedPointer<Shape>(shape));
        shapes[m_nSelectedShape]->selected = false;
        m_nSelectedShape = shapes.count()-1;
        repaint();
    }else{
        shapes[m_nSelectedShape]->points = shape->points;
    }
    m_iSelectedShapeCopy.clear();
}

void Canvas::handleDrawing(QPointF pos){
  if (m_iCurrentShape != nullptr && m_iCurrentShape->reachMaxPoints() == false){
      QPointF initPos = m_iCurrentShape->pointAt(0);
      double minX = initPos.x();
      double minY = initPos.y();
      QPointF targetPos = m_iCurrentLineShape->pointAt(1);
      double maxX = targetPos.x();
      double maxY = targetPos.y();
      m_iCurrentShape->addPoint(QPointF(maxX, minY));
      m_iCurrentShape->addPoint(targetPos);
      m_iCurrentShape->addPoint(QPointF(minX, maxY));
      finalise();
  }else if (!outOfPixmap(pos)){
      m_iCurrentShape = QSharedPointer<Shape>(new Shape());
      m_iCurrentShape->addPoint(pos);
      m_iCurrentLineShape->points << pos << pos;
      setHiding();
      emit drawingPolygon(true);
      update();
  }
}
void Canvas::setHiding(bool enable){
    m_bHideBackround = enable;
}
bool Canvas::canCloseShape(){
    return drawing() && m_iCurrentShape != nullptr && m_iCurrentShape->pointCount() > 2;
}
void Canvas::mouseDoubleClickEvent(QMouseEvent *ev){
//    # We need at least 4 points here, since the mousePress handler
//    # adds an extra one before this handler is called.
    if (canCloseShape() and m_iCurrentShape->pointCount() > 3){
        m_iCurrentShape->popPoint();
        finalise();
    }
}
void Canvas::selectShape(Shape* shape){
    deSelectShape();
    shape->selected = true;
    for(int iLoop = 0; iLoop < shapes.size(); iLoop++){
        if(shape == shapes[iLoop].get()){
            m_nSelectedShape = iLoop;
            setHiding();
            emit sigSelectionChanged(true);
            update();
            return;
        }
    }
}
void Canvas::selectShapePoint(QPointF point){
//    """Select the first shape created which contains this point."""
    deSelectShape();
    if (selectedVertex()){  //# A vertex is marked for selection.
        int index= m_nHightlightVertex;
        shapes[m_nHighlightShape].get()->highlightVertex(index, Shape::MOVE_VERTEX);
        selectShape(shapes[m_nHighlightShape].get());
        return;
    }
    for(int iLoop = shapes.size() - 1; iLoop >= 0; iLoop--){
        Shape* shape = shapes.at(iLoop).get();
        if(shape->Visible() && shape->containsPoint(point)){
            selectShape(shape);
            calculateOffsets(shape, point);
            return;
        }
    }
}
void Canvas::calculateOffsets(Shape* shape,QPointF point){
    QRectF rect = shape->boundingRect();
    double x1 = rect.x() - point.x();
    double y1 = rect.y() - point.y();
    double x2 = (rect.x() + rect.width()) - point.x();
    double y2 = (rect.y() + rect.height()) - point.y();

    m_arrayOffsets[0] = QPointF(x1, y1);
    m_arrayOffsets[1] = QPointF(x2, y2);
}

void Canvas::boundedMoveVertex(QPointF pos){
    int index = m_nHightlightVertex;
    Shape* shape = shapes[m_nHighlightShape].get();
    QPointF point = shape->pointAt(index);
    if (outOfPixmap(pos))
        pos = intersectionPoint(point, pos);

    QPointF shiftPos;
    if (m_bDrawSquare){
        int opposite_point_index = (index + 2) % 4;
        QPointF opposite_point = shape->pointAt(opposite_point_index);

        double min_size = qMin(qAbs(pos.x() - opposite_point.x()), abs(pos.y() - opposite_point.y()));
        double directionX = pos.x() - opposite_point.x()?-1 : 1;
        double directionY = pos.y() - opposite_point.y() < 0?-1 : 1;
        shiftPos = QPointF(opposite_point.x() + directionX * min_size - point.x(),
                           opposite_point.y() + directionY * min_size - point.y());
    }else{
        shiftPos = pos - point;
    }

    shape->moveVertexBy(index, shiftPos);

    int lindex = (index + 1) % 4;
    int rindex = (index + 3) % 4;
    QPointF lshift;
    QPointF rshift;
    if (index % 2 == 0){
        rshift = QPointF(shiftPos.x(), 0);
        lshift = QPointF(0, shiftPos.y());
    }else{
        lshift = QPointF(shiftPos.x(), 0);
        rshift = QPointF(0, shiftPos.y());
    }
    shape->moveVertexBy(rindex, rshift);
    shape->moveVertexBy(lindex, lshift);
}
bool Canvas::boundedMoveShape(Shape* shape,QPointF pos){
    if (outOfPixmap(pos))
        return false;//  # No need to move;
    QPointF o1 = pos + m_arrayOffsets[0];
    if (outOfPixmap(o1))
        pos -= QPointF(qMin(double(0), o1.x()), qMin(double(0), o1.y()));
    QPointF o2 = pos + m_arrayOffsets[1];
    if (outOfPixmap(o2))
        pos += QPointF(qMin(double(0), pixmap.width() - o2.x()),
                       qMin(double(0), pixmap.height() - o2.y()));
//    # The next line tracks the new position of the cursor
//    # relative to the shape, but also results in making it
//    # a bit "shaky" when nearing the border and allows it to
//    # go outside of the shape's area for some reason. XXX
//    #self.calculateOffsets(self.selectedShape, pos)
    QPointF dp = pos - m_PrevPoint;
    if (dp.isNull())
        shape->moveBy(dp);
        m_PrevPoint = pos;
        return true;
    return false;
}
void Canvas::deSelectShape(){
    if (m_nSelectedShape != -1){
        shapes[m_nSelectedShape]->selected = false;
        m_nSelectedShape = -1;
        setHiding(false);
        emit sigSelectionChanged(false);
        update();
    }
}
Shape* Canvas::deleteSelected(){
    if (m_nSelectedShape != -1){
        Shape* shape = shapes[m_nSelectedShape].get();
        QList<QSharedPointer<Shape>>::iterator iter = shapes.begin();
        while (iter != shapes.end()) {
            QSharedPointer<Shape> value = *iter;
            if(value.get() == shape){
                iter = shapes.erase(iter);
                value.clear();
            } else {
                ++iter;
            }
        }
        m_nSelectedShape = -1;
        update();
        return shape;
    }
    return nullptr;
}
Shape* Canvas::copySelectedShape(){
    if (m_nSelectedShape != -1){
        Shape* shape = new Shape();
        shape = shapes[m_nSelectedShape].get();
        deSelectShape();
        shapes.push_back(QSharedPointer<Shape>(shape));
        shape->selected = true;
        m_nSelectedShape = shapes.count()-1;
        boundedShiftShape(shape);
        return shape;
    }
    return nullptr;
}
void Canvas::boundedShiftShape(Shape* shape){
//    # Try to move in one direction, and if it fails in another.
//    # Give up if both fail.
    QPointF point = shape->pointAt(0);
    QPointF offset = QPointF(2.0, 2.0);
    calculateOffsets(shape, point);
    m_PrevPoint = point;
    if (!boundedMoveShape(shape, point - offset))
        boundedMoveShape(shape, point + offset);
}
void Canvas::paintEvent(QPaintEvent *event){
    if (pixmap.isNull()){
        return;
    }
//        return paintEvent(event);
    QPainter* p = m_iPainter;
    p->begin(this);
    p->setRenderHint(QPainter::Antialiasing);
    p->setRenderHint(QPainter::HighQualityAntialiasing);
    p->setRenderHint(QPainter::SmoothPixmapTransform);

    p->scale(scale, scale);
    p->translate(offsetToCenter());

    p->drawPixmap(0, 0, pixmap);
    Shape::scale = scale;
    for(auto shape : shapes){
        if ((shape->selected || !m_bHideBackround) && shape->Visible()){
            shape->fill = shape->selected || shape == shapes[m_nHighlightShape].get();
            shape->paint(p);
        }
    }

    if (m_iCurrentShape != nullptr){
        m_iCurrentShape->paint(p);
        m_iCurrentLineShape->paint(p);
    }
    if (!m_iSelectedShapeCopy.isNull())
        m_iSelectedShapeCopy->paint(p);

//    # Paint rect
    if (m_iCurrentShape != nullptr && m_iCurrentLineShape->pointCount() == 2){
        QPointF leftTop = m_iCurrentLineShape->pointAt(0);
        QPointF rightBottom = m_iCurrentLineShape->pointAt(1);
        double rectWidth = rightBottom.x() - leftTop.x();
        double rectHeight = rightBottom.y() - leftTop.y();
        p->setPen(m_DrawingRectColor);
        QBrush brush(Qt::BDiagPattern);
        p->setBrush(brush);
        p->drawRect(leftTop.x(), leftTop.y(), rectWidth, rectHeight);
    }
    if (drawing() && !m_PrevPoint.isNull() && !outOfPixmap(m_PrevPoint)){
        p->setPen(QColor(0, 0, 0));
        p->drawLine(m_PrevPoint.x(), 0, m_PrevPoint.x(), pixmap.height());
        p->drawLine(0, m_PrevPoint.y(), pixmap.width(), m_PrevPoint.y());
    }

    setAutoFillBackground(true);
    if (verified){
        QPalette pal = palette();
        pal.setColor(backgroundRole(), QColor(184, 239, 38, 128));
        setPalette(pal);
    }else{
        QPalette pal = palette();
        pal.setColor(backgroundRole(), QColor(232, 232, 232, 255));
        setPalette(pal);
    }
    p->end();
}

void Canvas::minIntersectingEdges(double x1,double y1,double x2,double y2, QList<QPointF> points, double &minD, int &i, QPointF xy){
//    """For each edge formed by `points', yield the intersection
//    with the line segment `(x1,y1) - (x2,y2)`, if it exists.
//    Also return the distance of `(x2,y2)' to the middle of the
//    edge along with its index, so that the one closest can be chosen."""
//    x1, y1 = x1y1
//    x2, y2 = x2y2
    double tmpMinD = sys_maxsize;
    int tmpIdx = -1;
    QPointF tmpPoints[4];
    for(int iLoop = 0; iLoop < 4; iLoop++){
        double x3 = points[iLoop].x();
        double y3 = points[iLoop].y();
        double x4 = points[(iLoop + 1) % 4].x();
        double y4 = points[(iLoop + 1) % 4].y();
        double denom = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
        double nua = (x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3);
        double nub = (x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3);
        if (denom == 0)
//            # This covers two cases:
//            #   nua == nub == 0: Coincident
//            #   otherwise: Parallel
            continue;
        double ua = nua / denom;
        double ub = nub / denom;
        if (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1){
            double x = x1 + ua * (x2 - x1);
            double y = y1 + ua * (y2 - y1);
            QPointF m = QPointF((x3 + x4) / 2, (y3 + y4) / 2);
            double d = distance(m - QPointF(x2, y2));
//            i = iLoop;
            tmpPoints[iLoop].setX(x);
            tmpPoints[iLoop].setY(y);
            if (d < tmpMinD){
                minD = d;
                tmpIdx = iLoop;
            }
//            yield d, i, (x, y);
        }
    }
    minD = tmpMinD;
    i = tmpIdx;
    xy = tmpPoints[tmpIdx];
}

QPointF Canvas::intersectionPoint(QPointF p1, QPointF p2){
//    # Cycle through each image edge in clockwise fashion,
//    # and find the one intersecting the current line segment.
//    # http://paulbourke.net/geometry/lineline2d/
    auto size = pixmap.size();
    QList<QPointF> points;
    points<< QPointF(0, 0)<< QPointF(size.width(), 0) << QPointF(size.width(), size.height()) << QPointF(0, size.height());
    double x1 = p1.x();
    double y1 = p1.y();
    double x2 = p2.x();
    double y2 = p2.y();
    double d;
    int i;
    QPointF xy;
    minIntersectingEdges(x1, y1, x2, y2, points, d, i, xy);
    double x3 = points[i].x();
    double y3 = points[i].y();
    double x4 = points[(i + 1) % 4].x();
    double y4 = points[(i + 1) % 4].y();
    if (xy == p1){
//        # Handle cases where previous point is on one of the edges.
        if (x3 == x4)
            return QPointF(x3, qMin(qMax(double(0), y2), qMax(y3, y4)));
        else  //# y3 == y4
            return QPointF(qMin(qMax(double(0), x2), qMax(x3, x4)), y3);
    }
    return xy;
}
void Canvas::wheelEvent(QWheelEvent *ev){
    QPoint delta = ev->angleDelta();
    int h_delta = delta.x();
    int v_delta = delta.y();

    Qt::KeyboardModifiers mods = ev->modifiers();
    if (Qt::ControlModifier == int(mods) && v_delta)
        emit zoomRequest(v_delta);
    else{
        emit scrollRequest(v_delta, Qt::Vertical);
        emit scrollRequest(h_delta, Qt::Horizontal);
    }
    ev->accept();
}
void Canvas::keyPressEvent(QKeyEvent *ev){
    int key = ev->key();
    if (key == Qt::Key_Escape && m_iCurrentShape != nullptr){
        printf("ESC press");
        m_iCurrentShape = nullptr;
        emit drawingPolygon(false);
        update();
    }else if (key == Qt::Key_Return and canCloseShape()){
        finalise();
    }else if (key == Qt::Key_Left && m_nSelectedShape != -1){
        moveOnePixel("Left");
    }else if (key == Qt::Key_Right && m_nSelectedShape != -1){
        moveOnePixel("Right");
    }else if (key == Qt::Key_Up && m_nSelectedShape != -1){
        moveOnePixel("Up");
    }else if (key == Qt::Key_Down && m_nSelectedShape != -1){
        moveOnePixel("Down");
    }
}

void Canvas::OnCreateShape()
{
    SetMode(CREATE);
}
void Canvas::moveOnePixel(QString direction){
//    # print(self.selectedShape.points)
    if (direction == "Left" && !moveOutOfBound(QPointF(-1.0, 0))){
//        # print("move Left one pixel")
        shapes[m_nSelectedShape]->pointAt(0) += QPointF(-1.0, 0);
        shapes[m_nSelectedShape]->pointAt(1) += QPointF(-1.0, 0);
        shapes[m_nSelectedShape]->pointAt(2) += QPointF(-1.0, 0);
        shapes[m_nSelectedShape]->pointAt(3) += QPointF(-1.0, 0);
    }else if (direction == "Right" && !moveOutOfBound(QPointF(1.0, 0))){
//        # print("move Right one pixel")
        shapes[m_nSelectedShape]->pointAt(0) += QPointF(1.0, 0);
        shapes[m_nSelectedShape]->pointAt(1) += QPointF(1.0, 0);
        shapes[m_nSelectedShape]->pointAt(2) += QPointF(1.0, 0);
        shapes[m_nSelectedShape]->pointAt(3) += QPointF(1.0, 0);
    }else if (direction == "Up" && !moveOutOfBound(QPointF(0, -1.0))){
//        # print("move Up one pixel")
        shapes[m_nSelectedShape]->pointAt(0) += QPointF(0, -1.0);
        shapes[m_nSelectedShape]->pointAt(1) += QPointF(0, -1.0);
        shapes[m_nSelectedShape]->pointAt(2) += QPointF(0, -1.0);
        shapes[m_nSelectedShape]->pointAt(3) += QPointF(0, -1.0);
    }else if (direction == "Down" && !moveOutOfBound(QPointF(0, 1.0))){
//        # print("move Down one pixel")
        shapes[m_nSelectedShape]->pointAt(0) += QPointF(0, 1.0);
        shapes[m_nSelectedShape]->pointAt(1) += QPointF(0, 1.0);
        shapes[m_nSelectedShape]->pointAt(2) += QPointF(0, 1.0);
        shapes[m_nSelectedShape]->pointAt(3) += QPointF(0, 1.0);
    }
    emit shapeMoved();
    repaint();
}
bool Canvas::moveOutOfBound(QPointF step){
    QList<QPointF> points;
    for(int iLoop = 0; iLoop < 4; iLoop++){
        points << shapes[m_nSelectedShape]->pointAt(iLoop) + step;
    }
    bool isTrue = false;
    for(int iLoop = 0; iLoop < 4; iLoop++){
        if(outOfPixmap(points[iLoop])) isTrue = true;
    }
    return isTrue;
}
Shape* Canvas::setLastLabel(QString text, QColor line_color, QColor fill_color){
      if (text == ""){
          qCritical("invalid text");
          return nullptr;
      }
      shapes[shapes.size() -1]->label = text;
      if (line_color.isValid())
          shapes[shapes.size()-1]->line_color = line_color;

      if (fill_color.isValid())
          shapes[shapes.size()-1]->fill_color = fill_color;

      return shapes[shapes.size()-1].get();
}
void Canvas::undoLastLine(){
    if (shapes.size() == 0){
        qCritical("empty shapes");
        return;
    }
    m_iCurrentShape = shapes.takeLast();
    m_iCurrentShape->setOpen();
    m_iCurrentLineShape->points <<m_iCurrentShape->points[m_iCurrentShape->points.size()-1] << m_iCurrentShape->points[0];
    emit drawingPolygon(true);
}
void Canvas::resetAllLines(){
    if (shapes.size() == 0){
        qCritical("empty shapes");
        return;
    }
    m_iCurrentShape = shapes.takeLast();
    m_iCurrentShape->setOpen();
    m_iCurrentLineShape->points <<m_iCurrentShape->points[m_iCurrentShape->points.size()-1] << m_iCurrentShape->points[0];
    emit drawingPolygon(true);
    m_iCurrentShape = nullptr;
    emit drawingPolygon(false);
    update();
}
void Canvas::loadPixmap(){
    if(m_iImage != nullptr && !m_iImage->isNull()){
        this->pixmap = QPixmap::fromImage(*m_iImage);;
        shapes.clear();
        repaint();
    }
}
void Canvas::SetShapes(QList<QSharedPointer<Shape> > shapes){
    this->shapes.clear();
    for(auto shape : shapes){
        QSharedPointer<Shape> pNew(shape);
        this->shapes.append(pNew);
        shape.clear();
    }
    m_iCurrentShape = nullptr;
    repaint();
}

int Canvas::__CurrentCursor(){
    QCursor* cursor = QApplication::overrideCursor();
    if (cursor != nullptr)
        return cursor->shape();
    return -1;
}
void Canvas::__OverrideCursor(Qt::CursorShape cursor){
    m_CursorShape = cursor;
    if (__CurrentCursor() == -1)
        QApplication::setOverrideCursor(QCursor(cursor));
    else
        QApplication::changeOverrideCursor(QCursor(cursor));
}
void Canvas::__RestoreCursor(){
    QApplication::restoreOverrideCursor();
}
void Canvas::resetState(){
    __RestoreCursor();
    pixmap.detach();
    update();
}
void Canvas::OnDrawingShapeToSquare(bool status){
    m_bDrawSquare = status;
}

void Canvas::Paint(int scaleVal)
{
    if (m_iImage == nullptr || m_iImage->isNull()){
        qFatal("cannot paint null image");
        return;
    }
    scale = 0.01 * scaleVal;
    adjustSize();
    update();
}

void Canvas::OnChangeShapeLineColor(){
    ColorDialog colorDialog(this);
    QColor color = colorDialog.getColor(Shape::line_color, "Choose line color",
                                      DEFAULT_LINE_COLOR);
    if (color.isValid()){
        SetSelectedShapeLineColor(color);
        emit sigDirty();
    }
}
void Canvas::OnChangeShapeFillColor(){
    ColorDialog colorDialog(this);
    QColor color = colorDialog.getColor(Shape::fill_color, "Choose fill color",
                                      DEFAULT_FILL_COLOR);
    if (color.isValid()){
        SetSelectedShapeFillColor(color);
        emit sigDirty();
    }
}
