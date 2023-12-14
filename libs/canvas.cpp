#include "canvas.h"
#include "common.h"
#include <QtGlobal>
#include <QApplication>

Canvas::Canvas(QString *filePath, QWidget *parent)
    : QWidget(parent), epsilon(11.0)
{
    this->filePath = filePath;
    m_enMode = Canvas::EDIT;
//    shapes = [];
    current = nullptr;
    selectedShape = nullptr;//  # save the selected shape here
    selectedShapeCopy = nullptr;
    drawingLineColor = QColor(0, 0, 255);
    drawingRectColor = QColor(0, 0, 255);
    line = QSharedPointer<Shape>(new Shape("",drawingLineColor), &QObject::deleteLater);
    prevPoint = QPointF();
//    offsets = QPointF(), QPointF();
    scale = 1.0;
    pixmap = QPixmap();
//    visible = {};
    _hideBackround = false;
    hideBackround = false;
    hShape = nullptr;
    hVertex = -1;
    _painter = new QPainter();
    _cursor = CURSOR_DEFAULT;
//    # Menus:
//    self.menus = (QMenu(), QMenu());
    menus[true] = new QMenu();
    menus[false] = new QMenu();
//    # Set widget options.
    setMouseTracking(true);
    setFocusPolicy(Qt::WheelFocus);
    verified = false;
    drawSquare = false;
    image = QImage();
}

Canvas::~Canvas()
{

}


void Canvas::setDrawingColor(QColor qcolor){
    drawingLineColor = qcolor;
    drawingRectColor = qcolor;
}
void Canvas::enterEvent(QEvent *ev){
    __OverrideCursor(_cursor);
}
void Canvas::leaveEvent(QEvent *ev){
    __RestoreCursor();
}
void Canvas::focusOutEvent(QFocusEvent *ev){
    __RestoreCursor();
}
bool Canvas::isVisible(Shape * shape){
    if(visible.contains(shape))
        return visible[shape];
    return true;
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
    prevPoint = QPointF();
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
    prevPoint = QPointF();
    repaint();
}

void Canvas::unHighlight(){
    if (hShape != nullptr)
        hShape->highlightClear();
    hVertex = -1;
    hShape = nullptr;

}
bool Canvas::selectedVertex(){
    return hVertex != -1;
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
    if (current == nullptr){
        qCritical("no current");
        return;
    }
    if (current.get()->pointAt(0) == current.get()->lastPoint()){
        current = nullptr;
        emit drawingPolygon(false);
        update();
        return;
    }

    current.get()->close();
    shapes.append(QSharedPointer<Shape>(current));
    current.clear();
    line->points.clear();
    setHiding(false);
    emit newShape();
    update();
}
bool Canvas::closeEnough(QPointF p1,QPointF p2){
//    # d = distance(p1 - p2)
//    # m = (p1-p2).manhattanLength()
//    # print "d %.2f, m %d, %.2f" % (d, m, d - m)
    return distance(p1 - p2) < epsilon;

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
    if (filePath != nullptr && *filePath != "")
        emit labelCoordinatesMsg(QString("X: %1; Y: %2").arg(pos.x()).arg(pos.y()));

//    # Polygon drawing.
    if (drawing()){
        __OverrideCursor(CURSOR_DRAW);
        if (!current.isNull()){
            qDebug("-----------current.pointCount()=%d", line->pointCount());
//            # Display annotation width and height while drawing
            double current_width = qAbs(current.get()->pointAt(0).x() - pos.x());
            double current_height = qAbs(current.get()->pointAt(0).y() - pos.y());
            emit labelCoordinatesMsg(QString("Width: %1, Height: %2 / X: %3; Y: %4").arg(current_width).arg(current_height).arg(pos.x()).arg(pos.y()));

            QColor color = drawingLineColor;
            if (outOfPixmap(pos)){
//                # Don't allow the user to draw outside the pixmap->
//                # Clip the coordinates to 0 or max,
//                # if they are outside the range [0, max]
                QSize size = pixmap.size();
                double clipped_x = qMin(qMax(double(0), pos.x()), double(size.width()));
                double clipped_y = qMin(qMax(double(0), pos.y()), double(size.height()));
                pos = QPointF(clipped_x, clipped_y);
            }else if (current->pointCount() > 1 && closeEnough(pos, current->pointAt(0))){
//                # Attract line to starting point and colorise to alert the
//                # user:
                QPointF pos = current->pointAt(0);
                QColor color = current->line_color;
                __OverrideCursor(CURSOR_POINT);
                current->highlightVertex(0, Shape::NEAR_VERTEX);
            }

            if (drawSquare){
                QPointF init_pos = current->pointAt(0);
                double min_x = init_pos.x();
                double min_y = init_pos.y();
                double min_size = qMin(qAbs(pos.x() - min_x), qAbs(pos.y() - min_y));
                double direction_x = pos.x() - min_x < 0 ? -1 : 1;
                double direction_y = pos.y() - min_y < 0 ? -1 : 1;
                line->SetPoint(1,QPointF(min_x + direction_x * min_size, min_y + direction_y * min_size));
            }else{
                line->SetPoint(1,pos);
            }

            line->line_color = color;
            prevPoint = QPointF();
            current->highlightClear();
        }else{
            prevPoint = pos;
        }
        repaint();
        return;
    }

//    # Polygon copy moving.
    if (Qt::RightButton & ev->buttons()){
        if (selectedShapeCopy != nullptr && prevPoint.isNull()){
            __OverrideCursor(CURSOR_MOVE);
            boundedMoveShape(selectedShapeCopy, pos);
            repaint();
        }else if (selectedShape !=nullptr){
            selectedShapeCopy = selectedShape;
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
            QPointF point1 = hShape->pointAt(1);
            QPointF point3 = hShape->pointAt(3);
            double current_width = qAbs(point1.x() - point3.x());
            double current_height = qAbs(point1.y() - point3.y());
            emit labelCoordinatesMsg(QString("Width: %1, Height: %2 / X: %3; Y: %4").arg(current_width).arg(current_height).arg(pos.x()).arg(pos.y()));
        }else if (selectedShape != nullptr && prevPoint.isNull()){
            __OverrideCursor(CURSOR_MOVE);
            boundedMoveShape(selectedShape, pos);
            emit shapeMoved();
            repaint();

//            # Display annotation width and height while moving shape
            QPointF point1 = selectedShape->pointAt(1);
            QPointF point3 = selectedShape->pointAt(3);
            double current_width = qAbs(point1.x() - point3.x());
            double current_height = qAbs(point1.y() - point3.y());
            emit labelCoordinatesMsg(QString("Width: %d, Height: %d / X: %d; Y: %d").arg(current_width).arg(current_height).arg(pos.x()).arg(pos.y()));
        }else{
//            # pan
            double delta_x = pos.x() - pan_initial_pos.x();
            double delta_y = pos.y() - pan_initial_pos.y();
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
        if (isVisible(shapes.at(iLoop).get())){
            isHightLight = true;
            Shape* shape = shapes.at(iLoop).get();

            //        # Look for a nearby vertex to highlight. If that fails,
            //        # check if we happen to be inside a shape.
            int index = shape->nearestVertex(pos, epsilon);
            if (index >= 0){
                if (selectedVertex())
                    hShape->highlightClear();
                hVertex = index;
                hShape = shape;
                shape->highlightVertex(index, Shape::MOVE_VERTEX);
                __OverrideCursor(CURSOR_POINT);
                setToolTip("Click & drag to move point");
                setStatusTip(toolTip());
                update();
                break;
            }else if (shape->containsPoint(pos)){
                if (selectedVertex())
                    hShape->highlightClear();
                hVertex = -1;
                hShape = shape;
                setToolTip(QString("Click & drag to move shape '%s'").arg(shape->label));
                setStatusTip(toolTip());
                __OverrideCursor(CURSOR_GRAB);
                update();
                break;
            }
        }
    }
    if (!isHightLight){ //# Nothing found, clear highlights, reset state.
        if (hShape != nullptr){
            hShape->highlightClear();
            update();
        }
        hVertex = -1;
        hShape = nullptr;
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
            prevPoint = pos;
            repaint();
        }
    }else if (ev->button() == Qt::RightButton && editing()){
        selectShapePoint(pos);
        prevPoint = pos;
        repaint();
    }
}
void Canvas::mouseReleaseEvent(QMouseEvent * ev){
    if (ev->button() == Qt::RightButton){
        QMenu* menu = menus[bool(selectedShapeCopy != nullptr)];
        __RestoreCursor();
        if (! menu->exec(mapToGlobal(ev->pos())) && selectedShapeCopy != nullptr){
//            # Cancel the move by deleting the shadow copy.
            selectedShapeCopy = nullptr;
            repaint();
        }
    }else if (ev->button() == Qt::LeftButton && selectedShape != nullptr){
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
    if(selectedShape == nullptr || selectedShapeCopy == nullptr){
        qCritical("invalid selectedShape or selectedShapeCopy");
        return;
    }
    Shape* shape = selectedShapeCopy;
//    #del shape.fill_color
//    #del shape.line_color
    if (copy){
        shapes.append(QSharedPointer<Shape>(shape));
        selectedShape->selected = false;
        selectedShape = shape;
        repaint();
    }else{
        selectedShape->points = shape->points;
    }
    selectedShapeCopy = nullptr;
}
void Canvas::hideBackroundShapes(bool value){
    hideBackround = value;
    if (selectedShape != nullptr){
//        # Only hide other shapes if there is a current selection.
//        # Otherwise the user will not be able to select a shape.
        setHiding(true);
        repaint();
    }
}
void Canvas::handleDrawing(QPointF pos){
  if (current != nullptr && current->reachMaxPoints() == false){
      QPointF initPos = current->pointAt(0);
      double minX = initPos.x();
      double minY = initPos.y();
      QPointF targetPos = line->pointAt(1);
      double maxX = targetPos.x();
      double maxY = targetPos.y();
      current->addPoint(QPointF(maxX, minY));
      current->addPoint(targetPos);
      current->addPoint(QPointF(minX, maxY));
      finalise();
  }else if (!outOfPixmap(pos)){
      current = QSharedPointer<Shape>(new Shape());
      current->addPoint(pos);
      line->points << pos << pos;
      setHiding();
      emit drawingPolygon(true);
      update();
  }
}
void Canvas::setHiding(bool enable){
    _hideBackround = enable ? hideBackround : false;
}
bool Canvas::canCloseShape(){
    return drawing() && current != nullptr && current->pointCount() > 2;
}
void Canvas::mouseDoubleClickEvent(QMouseEvent *ev){
//    # We need at least 4 points here, since the mousePress handler
//    # adds an extra one before this handler is called.
    if (canCloseShape() and current->pointCount() > 3){
        current->popPoint();
        finalise();
    }
}
void Canvas::selectShape(Shape* shape){
    deSelectShape();
    shape->selected = true;
    selectedShape = shape;
    setHiding();
    emit selectionChanged(true);
    update();
}
void Canvas::selectShapePoint(QPointF point){
//    """Select the first shape created which contains this point."""
    deSelectShape();
    if (selectedVertex()){  //# A vertex is marked for selection.
        int index= hVertex;
        Shape* shape =hShape;
        shape->highlightVertex(index, Shape::MOVE_VERTEX);
        selectShape(shape);
        return;
    }
    for(int iLoop = shapes.size() - 1; iLoop >= 0; iLoop--){
        Shape* shape = shapes.at(iLoop);
        if(isVisible(shape) and shape->containsPoint(point)){
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
    offsets = QPair<QPointF, QPointF>(QPointF(x1, y1), QPointF(x2, y2));
}

void Canvas::boundedMoveVertex(QPointF pos){
    int index = hVertex;
    Shape* shape = hShape;
    QPointF point = shape->pointAt(index);
    if (outOfPixmap(pos))
        pos = intersectionPoint(point, pos);

    QPointF shiftPos;
    if (drawSquare){
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
    QPointF o1 = pos + offsets.first;
    if (outOfPixmap(o1))
        pos -= QPointF(qMin(double(0), o1.x()), qMin(double(0), o1.y()));
    QPointF o2 = pos + offsets.second;
    if (outOfPixmap(o2))
        pos += QPointF(qMin(double(0), pixmap.width() - o2.x()),
                       qMin(double(0), pixmap.height() - o2.y()));
//    # The next line tracks the new position of the cursor
//    # relative to the shape, but also results in making it
//    # a bit "shaky" when nearing the border and allows it to
//    # go outside of the shape's area for some reason. XXX
//    #self.calculateOffsets(self.selectedShape, pos)
    QPointF dp = pos - prevPoint;
    if (dp.isNull())
        shape->moveBy(dp);
        prevPoint = pos;
        return true;
    return false;
}
void Canvas::deSelectShape(){
    if (selectedShape != nullptr){
        selectedShape->selected = false;
        selectedShape = nullptr;
        setHiding(false);
        emit selectionChanged(false);
        update();
    }
}
Shape* Canvas::deleteSelected(){
    if (selectedShape != nullptr){
        Shape* shape = selectedShape;
        shapes.removeAll(selectedShape);
        selectedShape = nullptr;
        update();
        return shape;
    }
    return nullptr;
}
Shape* Canvas::copySelectedShape(){
    if (selectedShape != nullptr){
        Shape* shape = new Shape();
        shape = selectedShape;
        deSelectShape();
        shapes.append(shape);
        shape->selected = true;
        selectedShape = shape;
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
    prevPoint = point;
    if (!boundedMoveShape(shape, point - offset))
        boundedMoveShape(shape, point + offset);
}
void Canvas::paintEvent(QPaintEvent *event){
    if (pixmap.isNull()){
        return;
    }
//        return paintEvent(event);

    QPainter* p = _painter;
    p->begin(this);
    p->setRenderHint(QPainter::Antialiasing);
    p->setRenderHint(QPainter::HighQualityAntialiasing);
    p->setRenderHint(QPainter::SmoothPixmapTransform);

    p->scale(scale, scale);
    p->translate(offsetToCenter());

    p->drawPixmap(0, 0, pixmap);
    Shape::scale = scale;
    for(auto shape : shapes){
        if ((shape->selected || !_hideBackround) && isVisible(shape)){
            shape->fill = shape->selected || shape == hShape;
            shape->paint(p);
        }
    }

    if (current != nullptr){
        current->paint(p);
        line->paint(p);
    }
    if (selectedShapeCopy!= nullptr)
        selectedShapeCopy->paint(p);

//    # Paint rect
    if (current != nullptr && line->pointCount() == 2){
        QPointF leftTop = line->pointAt(0);
        QPointF rightBottom = line->pointAt(1);
        double rectWidth = rightBottom.x() - leftTop.x();
        double rectHeight = rightBottom.y() - leftTop.y();
        p->setPen(drawingRectColor);
        QBrush brush(Qt::BDiagPattern);
        p->setBrush(brush);
        p->drawRect(leftTop.x(), leftTop.y(), rectWidth, rectHeight);
    }
    if (drawing() && !prevPoint.isNull() && !outOfPixmap(prevPoint)){
        p->setPen(QColor(0, 0, 0));
        p->drawLine(prevPoint.x(), 0, prevPoint.x(), pixmap.height());
        p->drawLine(0, prevPoint.y(), pixmap.width(), prevPoint.y());
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
    if (key == Qt::Key_Escape && current != nullptr){
        printf("ESC press");
        current = nullptr;
        emit drawingPolygon(false);
        update();
    }else if (key == Qt::Key_Return and canCloseShape()){
        finalise();
    }else if (key == Qt::Key_Left && selectedShape != nullptr){
        moveOnePixel("Left");
    }else if (key == Qt::Key_Right && selectedShape != nullptr){
        moveOnePixel("Right");
    }else if (key == Qt::Key_Up && selectedShape != nullptr){
        moveOnePixel("Up");
    }else if (key == Qt::Key_Down && selectedShape != nullptr){
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
        selectedShape->pointAt(0) += QPointF(-1.0, 0);
        selectedShape->pointAt(1) += QPointF(-1.0, 0);
        selectedShape->pointAt(2) += QPointF(-1.0, 0);
        selectedShape->pointAt(3) += QPointF(-1.0, 0);
    }else if (direction == "Right" && !moveOutOfBound(QPointF(1.0, 0))){
//        # print("move Right one pixel")
        selectedShape->pointAt(0) += QPointF(1.0, 0);
        selectedShape->pointAt(1) += QPointF(1.0, 0);
        selectedShape->pointAt(2) += QPointF(1.0, 0);
        selectedShape->pointAt(3) += QPointF(1.0, 0);
    }else if (direction == "Up" && !moveOutOfBound(QPointF(0, -1.0))){
//        # print("move Up one pixel")
        selectedShape->pointAt(0) += QPointF(0, -1.0);
        selectedShape->pointAt(1) += QPointF(0, -1.0);
        selectedShape->pointAt(2) += QPointF(0, -1.0);
        selectedShape->pointAt(3) += QPointF(0, -1.0);
    }else if (direction == "Down" && !moveOutOfBound(QPointF(0, 1.0))){
//        # print("move Down one pixel")
        selectedShape->pointAt(0) += QPointF(0, 1.0);
        selectedShape->pointAt(1) += QPointF(0, 1.0);
        selectedShape->pointAt(2) += QPointF(0, 1.0);
        selectedShape->pointAt(3) += QPointF(0, 1.0);
    }
    emit shapeMoved();
    repaint();
}
bool Canvas::moveOutOfBound(QPointF step){
    QList<QPointF> points;
    for(int iLoop = 0; iLoop < 4; iLoop++){
        points << selectedShape->pointAt(iLoop) + step;
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
    current = shapes.takeLast();
    current->setOpen();
    line->points <<current->points[current->points.size()-1] << current->points[0];
    emit drawingPolygon(true);
}
void Canvas::resetAllLines(){
    if (shapes.size() == 0){
        qCritical("empty shapes");
        return;
    }
    current = shapes.takeLast();
    current->setOpen();
    line->points <<current->points[current->points.size()-1] << current->points[0];
    emit drawingPolygon(true);
    current = nullptr;
    emit drawingPolygon(false);
    update();
}
void Canvas::loadPixmap(){
    if(!image.isNull()){
        this->pixmap = QPixmap::fromImage(image);;
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
    current = nullptr;
    repaint();
}
void Canvas::SetShapeVisible(Shape * shape, bool value){
    visible[shape] = value;
    repaint();
}
int Canvas::__CurrentCursor(){
    QCursor* cursor = QApplication::overrideCursor();
    if (cursor != nullptr)
        return cursor->shape();
    return -1;
}
void Canvas::__OverrideCursor(Qt::CursorShape cursor){
    _cursor = cursor;
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
    drawSquare = status;
}

void Canvas::Paint(int scaleVal)
{
    if (image.isNull()){
        qFatal("cannot paint null image");
        return;
    }
    scale = 0.01 * scaleVal;
    adjustSize();
    update();
}
