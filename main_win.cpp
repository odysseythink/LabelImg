#include "main_win.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QVBoxLayout>
#include <QApplication>
#include <functional>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <QImageReader>
#include <QFileDialog>
#include "settings.h"
#include "common.h"
#include "ui_main_win.h"

MainWin::MainWin(QString defaultFilename, QString defaultPrefdefClassFile, QString defaultSaveDir, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWin)
{
    ui->setupUi(this);
    setWindowTitle(Settings::APP_NAME);

//# Save as Pascal voc xml
this->defaultSaveDir = defaultSaveDir;
usingPascalVocFormat = true;
usingYoloFormat = false;

//# For loading all image under a directory
//mImgList = []
dirname = "";
lastOpenDir = "";

//# Whether we need to save or not.
dirty = false;

_noSelectionSlot = false;
_beginner = true;
//self.screencastViewer = self.getAvailableScreencastViewer();
//self.screencast = "https://youtu.be/p0nR2YsCY_U";
    canvas = new Canvas(&m_CurrentFilePath, this);

//    # Load predefined classes to the list
    loadPredefinedClasses(defaultPrefdefClassFile);
//# Main widgets and related state.

    ui->m_iLabelsWidget->SetCanvas(canvas);
    connect(ui->m_iLabelsWidget, SIGNAL(sigDifficultChanged(int)), this, SLOT(btnstate(int)));
    connect(ui->m_iFileListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(fileItemDoubleClicked(QListWidgetItem *)));

    zoomWidget = new ZoomWidget();
    colorDialog = new ColorDialog(this);


    connect(canvas, SIGNAL(zoomRequest(int)), this, SLOT(zoomRequest(int)));

    canvas->OnDrawingShapeToSquare(Settings::GetInstance()->GetBool(SETTING_DRAW_SQUARE, false));

    ui->m_iCentralScrollArea->setWidget(canvas);
    ui->m_iCentralScrollArea->setWidgetResizable(true);
    scrollBars[Qt::Vertical] = ui->m_iCentralScrollArea->verticalScrollBar();
    scrollBars[Qt::Horizontal] = ui->m_iCentralScrollArea->horizontalScrollBar();
    connect(canvas, SIGNAL(scrollRequest(double, int)), this, SLOT(scrollRequest(double, int)));

    connect(canvas, SIGNAL(sigNewShape()), this, SLOT(newShape()));
    connect(canvas, SIGNAL(shapeMoved()), this, SLOT(setDirty()));
    connect(canvas, SIGNAL(sigSelectionChanged(bool)), this, SLOT(shapeSelectionChanged(bool)));
    connect(canvas, SIGNAL(sigShapeSelected(Shape*)), ui->m_iLabelsWidget, SLOT(OnShapeSelected(Shape*)));
    connect(canvas, SIGNAL(drawingPolygon(bool)), this, SLOT(toggleDrawingSensitive(bool)));
    connect(canvas, SIGNAL(sigDirty()), this, SLOT(setDirty()));
    connect(ui->m_iLabelsWidget, SIGNAL(sigDirty()), this, SLOT(setDirty()));

    setCentralWidget(ui->m_iCentralScrollArea);
//    addDockWidget(Qt::RightDockWidgetArea, dock);
//    addDockWidget(Qt::RightDockWidgetArea, ui->m_iFilesDock);
    ui->m_iFilesDock->setFeatures(QDockWidget::DockWidgetFloatable);

    dockFeatures = QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable;
    ui->m_iLabelsDock->setFeatures(QDockWidget::DockWidgetFeatures(ui->m_iLabelsDock->features() ^ dockFeatures));

    auto quit = newAction("Quit", SLOT(close()), "Ctrl+Q", "quit", "Quit application");
    auto open = newAction("openFile", SLOT(openFile()), "Ctrl+O", "open", "Open image or label file");
    auto opendir = newAction("Open Dir", SLOT(openDirDialog()), "Ctrl+u", "open", "Open Dir");
    auto changeSavedir = newAction("Change Save Dir", SLOT(changeSavedirDialog()), "Ctrl+r", "open", "Change default saved Annotation dir");
    auto openAnnotation = newAction("Open Annotation", SLOT(openAnnotationDialog()), "Ctrl+Shift+O", "open", "Open an annotation file");
    auto openNextImg = newAction("Next Image", SLOT(openNextImg()), "d", "next", "Open the next Image");
    auto openPrevImg = newAction("Prev Image", SLOT(openPrevImg()), "a", "prev", "Open the previous Image");
    auto verify = newAction("Verify Image", SLOT(verifyImg()),"space", "verify", "Verify Image");
    auto save = newAction("Save", SLOT(saveFile()),"Ctrl+S", "save", "Save the labels to a file", false);
    auto save_format = newAction("&PascalVOC", SLOT(change_format()),"Ctrl+", "format_voc", "Change save format", true);
    auto saveAs = newAction("Save As", SLOT(saveFileAs()),"Ctrl+Shift+S", "save-as", "Save the labels to a different file", false);
    auto close = newAction("Close", SLOT(closeFile()), "Ctrl+W", "close", "Close the current file");
    auto resetAll = newAction("Reset All", SLOT(resetAll()), "", "resetall", "Reset All");
    auto color1 = newAction("Box Line Color", SLOT(chooseColor1()),"Ctrl+L", "color_line", "Choose Box line color");
    auto createMode = newAction("Create\nRectBox", SLOT(__OnShapeModeChanged()),"w", "new", "Draw a new box", false);
    auto editMode = newAction("&Edit\nRectBox", SLOT(__OnShapeModeChanged()),"Ctrl+J", "edit", "Move and edit Boxs", false);
    auto create = newAction("Create\nRectBox", SLOT(createShape()),"w", "new", "Draw a new box", false);
    connect(this, SIGNAL(sigCreateShape()), canvas, SLOT(OnCreateShape()));
    auto deleteAction = newAction("Delete\nRectBox", SLOT(deleteSelectedShape()),"Delete", "delete", "Remove the box", false);
    auto copy = newAction("Duplicate\nRectBox", SLOT(copySelectedShape()),"Ctrl+D", "copy", "Create a duplicate of the selected box",false);
    auto advancedMode = newAction("Advanced Mode", SLOT(toggleAdvancedMode()), "Ctrl+Shift+A", "expert", "Swtich to advanced mode",true,true);
    auto hideAll = newAction("&Hide\nRectBox", SLOT(togglePolygons()),"Ctrl+H", "hide", "Hide all bounding boxes",false);
    auto showAll = newAction("&Show\nRectBox", SLOT(togglePolygons()),"Ctrl+A", "hide", "Show all bounding boxes",false);
    auto help = newAction("Tutorial", SLOT(showTutorialDialog()), "", "help", "Show demo");
    auto showInfo = newAction("Information", SLOT(showInfoDialog()), "", "help", "Information");

    auto zoom = new QWidgetAction(this);
    zoom->setDefaultWidget(zoomWidget);
    zoomWidget->setWhatsThis(QString("Zoom in or out of the image. Also accessible with"
        " %1 and %2 from the canvas.").arg(fmtShortcut("Ctrl+[-+]")).arg(fmtShortcut("Ctrl+Wheel")));
    zoomWidget->setEnabled(false);

    auto zoomIn = newAction("zoomin", SLOT(addZoom()),"Ctrl++", "zoom-in", "zoominDetail", false);
    auto zoomOut = newAction("zoomout", SLOT(addZoom()),"Ctrl+-", "zoom-out", "zoomoutDetail", false);
    auto zoomOrg = newAction("originalsize", SLOT(setZoom()),"Ctrl+=", "zoom", "originalsizeDetail", false);
    auto fitWindow = newAction("fitWin", SLOT(setFitWindow()),"Ctrl+F", "fit-window", "fitWinDetail",false,true);
    auto fitWidth = newAction("fitWidth", SLOT(setFitWidth()),"Ctrl+Shift+F", "fit-width", "fitWidthDetail",false,true);
    QList<QObject* > zoomActions;
//    # Group zoom controls into a list for easier toggling.
    zoomActions << zoomWidget<< zoomIn<< zoomOut<<zoomOrg<< fitWindow<< fitWidth;
//    zoomActions << zoomIn<< zoomOut<<zoomOrg<< fitWindow<< fitWidth;
    zoomMode = MANUAL_ZOOM;
    ScalerFunCB fun = std::bind(&MainWin::scaleFitWindow, this);
    scalers[FIT_WINDOW] = fun;
    fun = std::bind(&MainWin::scaleFitWidth, this);
    scalers[FIT_WIDTH] = fun;
//        # Set to one to scale to 100% when loading files.
    fun = std::bind(&MainWin::scaleManualZoom, this);
    scalers[MANUAL_ZOOM] = fun;

    auto edit = newAction("Edit Label", SLOT(editLabel()),"Ctrl+E", "edit", "Modify the label of the selected Box", false);
    ui->m_iLabelsWidget->SetEditAction(edit);

    auto shapeLineColor = newAction("Shape Line Color", nullptr,"","color_line", "Change the line color for this specific shape",false, canvas);
    connect(shapeLineColor, SIGNAL(triggered()), canvas, SLOT(OnChangeShapeLineColor()));
    auto shapeFillColor = newAction("Shape Fill Color", nullptr,"","color", "Change the fill color for this specific shape", false, canvas);
    connect(shapeLineColor, SIGNAL(triggered()), canvas, SLOT(OnChangeShapeFillColor()));

    QAction* labels = ui->m_iLabelsDock->toggleViewAction();
    labels->setText("Show/Hide Label Panel");
    labels->setShortcut(QString("Ctrl+Shift+L"));

    //# Lavel list context menu.
    QMenu* labelMenu = new QMenu();
    QList<QObject *> labelMenuActions;
    labelMenuActions << edit << deleteAction;
    AddActions(labelMenu, labelMenuActions);


//    # Draw squares/rectangles
    drawSquaresOption = new QAction("Draw Squares", this);
    drawSquaresOption->setShortcut(QString("Ctrl+Shift+R"));
    drawSquaresOption->setCheckable(true);
    drawSquaresOption->setChecked(Settings::GetInstance()->GetBool(SETTING_DRAW_SQUARE, false));
    connect(drawSquaresOption, SIGNAL(triggered(bool)), canvas, SLOT(OnDrawingShapeToSquare(bool)));

//# Store actions for further handling.
    actions.save=save;
    actions.save_format=save_format;
    actions.saveAs=saveAs;
    actions.open=open;
    actions.close=close;
    actions.resetAll = resetAll,
    actions.lineColor=color1;
    actions.create=create;
    actions.deleteAction=deleteAction;
    actions.edit=edit;
    actions.copy=copy,
    actions.createMode=createMode;
    actions.editMode=editMode;
    actions.modeActionGroup = new QActionGroup(this);
    actions.modeActionGroup->addAction(createMode);
    actions.modeActionGroup->addAction(editMode);
    actions.modeActionGroup->setExclusive(true);
    actions.advancedMode=advancedMode,
    actions.shapeLineColor=shapeLineColor;
    actions.shapeFillColor=shapeFillColor,
    actions.zoom=zoom;
    actions.zoomIn=zoomIn;
    actions.zoomOut=zoomOut;
    actions.zoomOrg=zoomOrg,
    actions.fitWindow=fitWindow;
    actions.fitWidth=fitWidth,
    actions.zoomActions=zoomActions,
    actions.fileMenuActions << open << opendir << save << saveAs << close << resetAll << quit;
//    actions.beginner=();
//    actions.advanced=(),
    actions.editMenu << edit << copy << deleteAction << nullptr << color1 << drawSquaresOption;
    actions.beginnerContext << create << edit << copy << deleteAction;
    actions.advancedContext << createMode << editMode << edit << copy << deleteAction << shapeLineColor << shapeFillColor;
    actions.onLoadActive << close << create << createMode << editMode;
    actions.onShapesPresent << saveAs << hideAll << showAll;

    menus.file= menu("&File");
    menus.edit= menu("&Edit");
    menus.view= menu("&View");
    menus.help= menu("&Help");
    menus.recentFiles= new QMenu("Open &Recent");
    menus.labelList=labelMenu;
    ui->m_iLabelsWidget->SetLabelListMenu(menus.labelList);

//    # Auto saving : Enable auto saving if pressing next
    autoSaving = new QAction("Auto Save mode", this);
    autoSaving->setCheckable(true);
    autoSaving->setChecked(Settings::GetInstance()->GetBool(SETTING_AUTO_SAVE, false));
//    # Sync single class mode from PR#106
    singleClassMode = new QAction("Single Class Mode", this);
    singleClassMode->setShortcut(QString("Ctrl+Shift+S"));
    singleClassMode->setCheckable(true);
    singleClassMode->setChecked(Settings::GetInstance()->GetBool(SETTING_SINGLE_CLASS, false));
    lastLabel = "";
//    # Add option to enable/disable labels being displayed at the top of bounding boxes
    displayLabelOption = new QAction("Display Labels", this);
    displayLabelOption->setShortcut(QString("Ctrl+Shift+P"));
    displayLabelOption->setCheckable(true);
    displayLabelOption->setChecked(Settings::GetInstance()->GetBool(SETTING_PAINT_LABEL, false));
    connect(displayLabelOption, SIGNAL(triggered()), this, SLOT(togglePaintLabelsOption()));

    QList<QObject*> tmpActions;
    AddActions(menus.file,
               tmpActions << open << opendir << changeSavedir << openAnnotation << menus.recentFiles << save << save_format << saveAs << close << resetAll << quit);
    tmpActions.clear();
    AddActions(menus.help, tmpActions << help<< showInfo);
    tmpActions.clear();
    AddActions(menus.view, tmpActions << autoSaving << singleClassMode << displayLabelOption << labels << advancedMode << nullptr << hideAll << showAll << nullptr << zoomIn << zoomOut << zoomOrg << nullptr << fitWindow << fitWidth);

    connect(menus.file, SIGNAL(aboutToShow()), this, SLOT(updateFileMenu()));

//    # Custom context menu for the canvas widget:
    AddActions(canvas->menus[false], actions.beginnerContext);
    tmpActions.clear();
    AddActions(canvas->menus[true], tmpActions << newAction("&Copy here", SLOT(copyShape())) << newAction("&Move here", SLOT(moveShape())));

    toolbar("Tools");
    actions.beginner << open << opendir << changeSavedir << openNextImg << openPrevImg << verify << save << save_format << nullptr << create << copy << deleteAction << nullptr <<
        zoomIn << zoom << zoomOut << fitWindow << fitWidth;

    actions.advanced << open << opendir << changeSavedir << openNextImg << openPrevImg << save << save_format << nullptr <<
        createMode << editMode << nullptr <<
        hideAll << showAll;

    statusBar()->showMessage(QString("%1 started.").arg(Settings::APP_NAME));
    statusBar()->show();

//    # Application state.
    m_CurrentFilePath = defaultFilename;
//    self.recentFiles = [];
    maxRecent = 7;
//    self.lineColor = None;
//    self.fillColor = None;
    zoom_level = 100;
    fit_window = false;
//    # Add Chris
    difficult = false;

//    ## Fix the compatible issue for qt4 and qt5. Convert the QStringList to python list
    recentFiles = Settings::GetInstance()->GetStringList(SETTING_RECENT_FILES);

    auto size = Settings::GetInstance()->Get(SETTING_WIN_SIZE, QSize(600, 500));
    QPoint position(0, 0);
//    auto saved_position = Settings::GetInstance()->Get(SETTING_WIN_POSE, position);
//    # Fix the multiple monitors issue
//    auto screens = QGuiApplication::screens();
//    for (int i = 0; i < screens.count(); i++){
//        if (QApplication::desktop()->availableGeometry(i).contains(saved_position)){
//            position = saved_position;
//            break;
//        }
//    }
    resize(size);
    move(position);
    auto saveDir = Settings::GetInstance()->Get(SETTING_SAVE_DIR, QString(""));
    lastOpenDir = Settings::GetInstance()->Get(SETTING_LAST_OPEN_DIR, QString(""));
    if (defaultSaveDir == "" && saveDir != "" && QDir(saveDir).exists()){
        defaultSaveDir = saveDir;
        statusBar()->showMessage(QString("%s started. Annotation will be saved to %s").arg(Settings::APP_NAME).arg(defaultSaveDir));
        statusBar()->show();
    }

    restoreState(Settings::GetInstance()->Get(SETTING_WIN_STATE, QByteArray()));
    Shape::line_color = lineColor = QColor(Settings::GetInstance()->Get(SETTING_LINE_COLOR, DEFAULT_LINE_COLOR.name()));
    Shape::fill_color = fillColor = QColor(Settings::GetInstance()->Get(SETTING_FILL_COLOR, DEFAULT_FILL_COLOR.name()));
    canvas->setDrawingColor(lineColor);
//    # Add chris
    Shape::difficult = difficult;

    if (Settings::GetInstance()->Get(SETTING_ADVANCE_MODE, false)){
        actions.advancedMode->setChecked(true);
        toggleAdvancedMode();
    }

//    # Populate the File menu dynamically.
    updateFileMenu();

//    # Since loading the file may take some time, make sure it runs in the background.
//    if (filePath != "" && QFileInfo(filePath).isDir())
//        queueEvent(importDirImages(filePath));
//    else if (filePath != "")
//        queueEvent(loadFile(filePath));

//    # Callbacks:
    connect(zoomWidget, SIGNAL(valueChanged(int)), canvas, SLOT(Paint(int)));

    populateModeActions();

//    # Display cursor coordinates at the right of status bar
    labelCoordinates = new QLabel("");
    statusBar()->addPermanentWidget(labelCoordinates);

//    # Open Dir if deafult file
    if (m_CurrentFilePath != "" && QFileInfo(m_CurrentFilePath).isDir())
        openDirDialog(false, m_CurrentFilePath);
}

MainWin::~MainWin()
{
}

double MainWin::scaleManualZoom(){
    return 100;
}

void MainWin::loadPredefinedClasses(QString predefClassesFile){
    bool changed = false;
    QStringList labels = Settings::GetInstance()->Get(SETTING_LABEL_HIST, QStringList());
    QFile file(predefClassesFile);
   if (file.open(QIODevice::ReadOnly | QIODevice::Text))
   {
       QString line;
       QTextStream in(&file);  //用文件构造流
       in.setCodec("utf-8");
       line = in.readLine().trimmed();//读取一行放到字符串里
       qDebug() << line;
       while(!line.isNull())//字符串有内容
       {
           line=in.readLine();//循环读取下行
           if(!labels.contains(line)){
                labels.append(line);
                changed = true;
           }
       }
   }
   file.close();
   if (changed){
       Settings::GetInstance()->Set(SETTING_LABEL_HIST, labels);
   }
}

QAction *MainWin::newAction(QString text, const char *slot, QString shortcut, QString icon, QString tip, bool enabled, bool checkable, QWidget* parent)
{
    if (parent == nullptr) parent = this;
//    """Create a new action and assign callbacks, shortcuts, etc."""
    QAction* a = new QAction(text, this);
    if (icon != "")
        a->setIcon(*newIcon(icon));
    if (shortcut != ""){
//        if isinstance(shortcut, (list, tuple)):
//            a->setShortcuts(shortcut);
//        else:
            a->setShortcut(shortcut);
    }
    if (tip != ""){
        a->setToolTip(tip);
        a->setStatusTip(tip);
    }
    if (slot != nullptr)
        connect(a, SIGNAL(triggered()), parent, slot);
//        a.triggered.connect(slot);
    if (checkable)
        a->setCheckable(true);
    a->setEnabled(enabled);
    return a;
}


QMenu* MainWin::menu(QString title, QList<QObject *> actions){
    QMenu* menu = menuBar()->addMenu(title);
    if (actions.size() > 0)
        AddActions(menu, actions);
    return menu;
}

ToolBar* MainWin::toolbar(QString title, QList<QObject *> actions){
//    ui->m_iToolsBar
//    auto toolbar = new ToolBar(title);
    ui->m_iToolsBar->setObjectName(QString("%1ToolBar").arg(title));
//    # toolbar.setOrientation(Qt.Vertical)
    ui->m_iToolsBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    if (actions.size() > 0)
        AddActions(ui->m_iToolsBar, actions);
    addToolBar(Qt::LeftToolBarArea, ui->m_iToolsBar);
    return ui->m_iToolsBar;
}


void MainWin::keyReleaseEvent(QKeyEvent *ev){
    if (ev->key() == Qt::Key_Control)
        canvas->OnDrawingShapeToSquare(false);
}
void MainWin::keyPressEvent(QKeyEvent *ev){
    if (ev->key() == Qt::Key_Control){
//        # Draw rectangle if Ctrl is pressed
        canvas->OnDrawingShapeToSquare(true);
    }
}


//## Support Functions ##
void MainWin::set_format(const QString strSaveFormat){
    if (strSaveFormat == FORMAT_PASCALVOC){
        actions.save_format->setText(FORMAT_PASCALVOC);
        actions.save_format->setIcon(*newIcon("format_voc"));
        usingPascalVocFormat = true;
        usingYoloFormat = false;
        LabelFile::suffix = XML_EXT;

    }else if (strSaveFormat == FORMAT_YOLO){
        actions.save_format->setText(FORMAT_YOLO);
        actions.save_format->setIcon(*newIcon("format_yolo"));
        usingPascalVocFormat = false;
        usingYoloFormat = true;
        LabelFile::suffix = TXT_EXT;
    }
}


void MainWin::change_format(){
    if (usingPascalVocFormat) set_format(FORMAT_YOLO);
    else if (usingYoloFormat) set_format(FORMAT_PASCALVOC);

}
bool MainWin::noShapes(){
    return ui->m_iLabelsWidget->NoShapes();
}
void MainWin::toggleAdvancedMode(bool value){
    value = actions.advancedMode->isChecked();
    qDebug() << "-------------------value" << value;
    _beginner = not value;
    canvas->SetMode(Canvas::EDIT);
    populateModeActions();
    ui->m_iLabelsWidget->SetEditVisible(!value);
    if (value){
        actions.createMode->setEnabled(true);
        actions.editMode->setEnabled(false);
        ui->m_iLabelsDock->setFeatures(QDockWidget::DockWidgetFeatures(ui->m_iLabelsDock->features() | dockFeatures));
    }else{
        ui->m_iLabelsDock->setFeatures(QDockWidget::DockWidgetFeatures(ui->m_iLabelsDock->features() ^ dockFeatures));
    }
}
void MainWin::populateModeActions(){
    QList<QObject*> tool;
    QList<QObject*> menu;
    if (beginner()){
        tool = actions.beginner;
        menu = actions.beginnerContext;
    }else{
        tool = actions.advanced;
        menu = actions.advancedContext;
    }
    ui->m_iToolsBar->clear();
    AddActions(ui->m_iToolsBar, tool);
    canvas->menus[false]->clear();
    AddActions(canvas->menus[false], menu);
    menus.edit->clear();
    QList<QObject*> tmpactions;
    beginner() ? tmpactions << actions.create : tmpactions << actions.createMode<< actions.editMode;
    tmpactions << actions.editMenu;
    AddActions(menus.edit, tmpactions);
}
void MainWin::setBeginner(){
    ui->m_iToolsBar->clear();
    AddActions(ui->m_iToolsBar, actions.beginner);
}
void MainWin::setAdvanced(){
    ui->m_iToolsBar->clear();
    AddActions(ui->m_iToolsBar, actions.advanced);
}
void MainWin::setDirty(){
    dirty = true;
    actions.save->setEnabled(true);
}
void MainWin::setClean(){
    dirty = false;
    actions.save->setEnabled(false);
    actions.create->setEnabled(true);
}
void MainWin::toggleActions(bool value){
//    """Enable/Disable widgets which depend on an opened image."""
    for (auto z : actions.zoomActions){
        if (qobject_cast<QMenu*>(z) != nullptr){
            qobject_cast<QMenu*>(z)->setEnabled(value);
        }else if (qobject_cast<QAction*>(z) != nullptr){
            qobject_cast<QAction*>(z)->setEnabled(value);
        }
    }
    for (auto action : actions.onLoadActive){
        if (qobject_cast<QMenu*>(action) != nullptr){
            qobject_cast<QMenu*>(action)->setEnabled(value);
        }else if (qobject_cast<QAction*>(action) != nullptr){
            qobject_cast<QAction*>(action)->setEnabled(value);
        }
    }
}

void MainWin::queueEvent(EventFunCB function){
    QTimer::singleShot(0, function);
}
void MainWin::status(QString  message, int delay){
    statusBar()->showMessage(message, delay);
}
void MainWin::resetState(){
    ui->m_iLabelsWidget->ClearLabel();
    m_CurrentFilePath = "";
    imageData.clear();
    labelFile = nullptr;
    canvas->resetState();
    labelCoordinates->clear();
}

void MainWin::addRecentFile(QString filePath){
    if (recentFiles.contains(filePath)){
        recentFiles.removeAll(filePath);
    }else if (recentFiles.size() >= maxRecent){
        recentFiles.takeLast();
    }
    recentFiles.push_front(filePath);
}
bool MainWin::beginner(){
    return _beginner;
}
bool MainWin::advanced(){
    return !beginner();
}
void MainWin::showInfoDialog(){
    auto msg = QString("Name:%1 \nApp Version:%2 \n ").arg(Settings::APP_NAME).arg(Settings::APP_VERSION);
    QMessageBox::information(this, "Information", msg);
}
void MainWin::createShape(){
    if(!beginner()){
        qFatal("createShape must in beginner");
        return;
    }
    actions.create->setEnabled(false);
    emit sigCreateShape();
}
void MainWin::toggleDrawingSensitive(bool drawing){
    qDebug("---------");
//    """In the middle of drawing, toggling between modes should be disabled."""
    actions.editMode->setEnabled(not drawing);
    if (!drawing && beginner()){
//        # Cancel creation.
        qDebug("Cancel creation.");
        canvas->SetMode(Canvas::EDIT);
        actions.create->setEnabled(true);
    }
}

void MainWin::__OnShapeModeChanged(){
    qDebug("---------");
    if(!advanced()){
        qFatal("__OnShapeModeChanged must in advanced");
        return;
    }
    QAction* pAction = qobject_cast<QAction*>(sender());
    if (pAction == actions.createMode){
        canvas->SetMode(Canvas::CREATE);
        actions.createMode->setEnabled(false);
        actions.editMode->setEnabled(true);
    } else if(pAction == actions.editMode){
        canvas->SetMode(Canvas::EDIT);
        actions.createMode->setEnabled(true);
        actions.editMode->setEnabled(false);
        ui->m_iLabelsWidget->OnLabelSelectionChanged();
    }
}

void MainWin::updateFileMenu(){
    QString currFilePath = m_CurrentFilePath;
    auto menu = menus.recentFiles;
    menu->clear();
    QStringList files;
    for (auto f : recentFiles){
        if (f != currFilePath && QFileInfo(f).exists()){
            files << f;
        }
    }
    for (int i = 0; i < files.size(); i++){
        auto icon = newIcon("labels");
        auto action = new QAction(*icon, QString("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName()), this);
        QObject::connect(action, &QAction::triggered, [=](){
            loadRecent(files[i]);
        });
        menu->addAction(action);
    }
}

void MainWin::editLabel(){
    if (! canvas->editing())
        return;
    Shape* currentShape = ui->m_iLabelsWidget->CurrentShape();
    if (currentShape == nullptr){
        return;
    }
    LabelDialog* pLabelDialog = new LabelDialog(Settings::GetInstance()->Get(SETTING_LABEL_HIST, QStringList()), this);
    auto text = pLabelDialog->popUp(currentShape->text());
    if (text != ""){
        currentShape->setText(text);
        currentShape->setBackground(generateColorByText(text));
        setDirty();
    }
    delete pLabelDialog;
//# Tzutalin 20160906 : Add file list and dock to move faster
}

void MainWin::fileItemDoubleClicked(QListWidgetItem *item){
    auto currIndex = mImgList.indexOf(item->text());
    if (currIndex < mImgList.size()){
        auto filename = mImgList[currIndex];
        if (filename != ""){
            loadFile(filename);
        }
    }
//# Add chris
}
void MainWin::btnstate(int stat){
    Q_UNUSED(stat)
    setDirty();
}
void MainWin::shapeSelectionChanged(bool selected){
    actions.deleteAction->setEnabled(selected);
    actions.copy->setEnabled(selected);
    actions.edit->setEnabled(selected);
    actions.shapeLineColor->setEnabled(selected);
    actions.shapeFillColor->setEnabled(selected);
}
void MainWin::addLabel(Shape* shape){
    shape->paintLabel = displayLabelOption->isChecked();
    ui->m_iLabelsWidget->AddLabel(shape);
    for (auto action : actions.onShapesPresent){
        if (qobject_cast<QMenu*>(action) != nullptr){
            qobject_cast<QMenu*>(action)->setEnabled(true);
        }else if (qobject_cast<QAction*>(action) != nullptr){
            qobject_cast<QAction*>(action)->setEnabled(true);
        }
    }
}

void MainWin::loadLabels(QList<QSharedPointer<Shape>> shapes){
//    auto s = [];
//    for (auto s : shapes){

//    }
//    for label, points, line_color, fill_color, difficult in shapes:
//        shape = Shape(label=label)
//        for x, y in points:
//            shape.addPoint(QPointF(x, y))
//        shape.difficult = difficult
//        shape.close()
//        s.append(shape)

//        if line_color:
//            shape.line_color = QColor(*line_color)
//        else:
//            shape.line_color = generateColorByText(label)

//        if fill_color:
//            shape.fill_color = QColor(*fill_color)
//        else:
//            shape.fill_color = generateColorByText(label)

//        self.addLabel(shape)
    canvas->SetShapes(shapes);
}
bool MainWin::saveLabels(QString annotationFilePath){
    if (labelFile == nullptr){
        labelFile = new LabelFile();
        labelFile->verified = canvas->verified;
    }

//    }void MainWin::format_shape(s):
//        return dict(label=s.label,
//                    line_color=s.line_color.getRgb(),
//                    fill_color=s.fill_color.getRgb(),
//                    points=[(p.x(), p.y()) for p in s.points],
//                   # add chris
//                    difficult = s.difficult)

//    shapes = [format_shape(shape) for shape in self.canvas.shapes]
//    # Can add differrent annotation formats here
    if (usingPascalVocFormat){
        if (!annotationFilePath.toLower().endsWith(".xml"))
            annotationFilePath += XML_EXT;
        labelFile->savePascalVocFormat(annotationFilePath, canvas->shapes, m_CurrentFilePath, imageData);
    }else if (usingYoloFormat){
        if (!annotationFilePath.toLower().endsWith(".txt"))
            annotationFilePath += TXT_EXT;
        labelFile->saveYoloFormat(annotationFilePath, canvas->shapes, m_CurrentFilePath, imageData, Settings::GetInstance()->Get(SETTING_LABEL_HIST, QStringList()));
    }else{
//        labelFile.save(annotationFilePath, shapes, self.filePath, self.imageData,
//                            self.lineColor.getRgb(), self.fillColor.getRgb());
    }
    qDebug() << QString("Image:%1 -> Annotation:%2").arg(m_CurrentFilePath).arg(annotationFilePath);
    return true;
}
void MainWin::copySelectedShape(){
    addLabel(canvas->copySelectedShape());
//    # fix copy and delete
    shapeSelectionChanged(true);
}


void MainWin::newShape(){
    qDebug("---------");
//    """Pop-up and give focus to the label editor.

//    position MUST be in global coordinates.
//    """
    QStringList labels = Settings::GetInstance()->Get(SETTING_LABEL_HIST, QStringList());
    QString text = "";
    if (!ui->m_iLabelsWidget->UseDefaultLabel() || ui->m_iLabelsWidget->DefaultLabel() == ""){

        LabelDialog* pLabelDialog = new LabelDialog(labels, this);
//        # Sync single class mode from PR#106

        if (singleClassMode->isChecked() && lastLabel != ""){
            text = lastLabel;
        }else{
            text= prevLabelText;
            text = pLabelDialog->popUp(text);
            lastLabel = text;
        }
        delete pLabelDialog;
    }else{
        text = ui->m_iLabelsWidget->DefaultLabel();
    }
//    # Add Chris
    ui->m_iLabelsWidget->SetDifficult(false);
    if (text != ""){
        prevLabelText = text;
        QColor generate_color = generateColorByText(text);
        auto shape = canvas->SetLastLabel(text, generate_color, generate_color);
        addLabel(shape);
        if (beginner()){//  # Switch to edit mode.
            canvas->SetMode(Canvas::EDIT);
            actions.create->setEnabled(true);
        }else{
            actions.editMode->setEnabled(true);
        }
        setDirty();

        if(!labels.contains(text)){
            labels.append(text);
            Settings::GetInstance()->Set(SETTING_LABEL_HIST, labels);
        }
    }else{
//        # self.canvas.undoLastLine()
        canvas->ResetAllLines();
    }
}
void MainWin::scrollRequest(double delta, int orientation){
    double units = - delta / (8 * 15);
    auto bar = scrollBars[Qt::Orientation(orientation)];
    bar->setValue(bar->value() + bar->singleStep() * units);
}
void MainWin::setZoom(int value){
    actions.fitWidth->setChecked(false);
    actions.fitWindow->setChecked(false);
    zoomMode = MANUAL_ZOOM;
    zoomWidget->setValue(value);
}
void MainWin::addZoom(int increment){
    QAction* pAction = qobject_cast<QAction*>(sender());
    if (pAction == actions.zoomIn){
        setZoom(zoomWidget->value() + 10);
    } else if(pAction == actions.zoomOut){
        setZoom(zoomWidget->value() + -10);
    } else if (pAction == nullptr){
        qDebug("---------increment=%d", increment);
        setZoom(zoomWidget->value() + increment);
    }
}
void MainWin::zoomRequest(int delta){
//    # get the current scrollbar positions
//    # calculate the percentages ~ coordinates
    auto h_bar = scrollBars[Qt::Horizontal];
    auto v_bar = scrollBars[Qt::Vertical];

//    # get the current maximum, to know the difference after zooming
    auto h_bar_max = h_bar->maximum();
    auto v_bar_max = v_bar->maximum();

//    # get the cursor position and canvas size
//    # calculate the desired movement from 0 to 1
//    # where 0 = move left
//    #       1 = move right
//    # up and down analogous
    auto cursor = new QCursor();
    auto pos = cursor->pos();
    auto relative_pos = this->mapFromGlobal(pos);

    auto cursor_x = relative_pos.x();
    auto cursor_y = relative_pos.y();

    auto w = ui->m_iCentralScrollArea->width();
    auto h = ui->m_iCentralScrollArea->height();

//    # the scaling from 0 to 1 has some padding
//    # you don't have to hit the very leftmost pixel for a maximum-left movement
    auto margin = 0.1;
    auto move_x = (cursor_x - margin * w) / (w - 2 * margin * w);
    auto move_y = (cursor_y - margin * h) / (h - 2 * margin * h);

//    # clamp the values from 0 to 1
    move_x = qMin(qMax(move_x, double(0)), double(1));
    move_y = qMin(qMax(move_y, double(0)), double(1));

//    # zoom in
    auto units = delta / (8 * 15);
    auto scale = 10;
    addZoom(int(scale * units));

//    # get the difference in scrollbar values
//    # this is how far we can move
    auto d_h_bar_max = h_bar->maximum() - h_bar_max;
    auto d_v_bar_max = v_bar->maximum() - v_bar_max;

//    # get the new scrollbar values
    auto new_h_bar_value = h_bar->value() + move_x * d_h_bar_max;
    auto new_v_bar_value = v_bar->value() + move_y * d_v_bar_max;

    h_bar->setValue(new_h_bar_value);
    v_bar->setValue(new_v_bar_value);
}
void MainWin::setFitWindow(bool value){
    if (value)
        actions.fitWidth->setChecked(false);
    zoomMode = value?FIT_WINDOW :MANUAL_ZOOM;
    adjustScale();
}
void MainWin::setFitWidth(bool value){
    if (value)
        actions.fitWindow->setChecked(false);
    zoomMode = value?FIT_WIDTH : MANUAL_ZOOM;
    adjustScale();
}
void MainWin::togglePolygons(bool value){
    ui->m_iLabelsWidget->ToggleShapes(value);
}

void MainWin::adjustScale(bool initial){
    auto value = scalers[initial?FIT_WINDOW : zoomMode]();
    zoomWidget->setValue(int(100 * value));
}

bool MainWin::loadFile(QString filePath){
//    """Load the specified file, or the last opened file if None."""
    resetState();
    canvas->setEnabled(false);
    if (filePath == "")
        filePath = Settings::GetInstance()->Get(SETTING_FILENAME, QString(""));
    m_CurrentFilePath = filePath;
//    # Make sure that filePath is a regular python string, rather than QString
//    filePath = ustr(filePath);

//    # Tzutalin 20160906 : Add file list and dock to move faster
//    # Highlight the file item
    if (filePath != "" && ui->m_iFileListWidget->count() > 0){
        auto index = mImgList.indexOf(filePath);
        auto fileWidgetItem = ui->m_iFileListWidget->item(index);
        fileWidgetItem->setSelected(true);
    }

    if (filePath != "" && QFileInfo(filePath).exists()){
        if (LabelFile::isLabelFile(filePath)){
            labelFile = new LabelFile(filePath);
            imageData = labelFile->imageData;
//            lineColor = QColor(*self.labelFile.lineColor);
//            fillColor = QColor(*self.labelFile.fillColor);
            canvas->verified = labelFile->verified;
        }else{
//            # Load image:
//            # read data first and store for saving into label file.
//            self.imageData = read(filePath, None);
            labelFile = nullptr;
            canvas->verified = false;
        }

        if (!canvas->SetImage(filePath)){
            errorMessage("Error opening file", QString("<p>Make sure <i>%1</i> is a valid image file.").arg(filePath));
            status(QString("Error reading %1").arg(filePath));
            return false;
        }
        status(QString("Loaded %1").arg(QFileInfo(filePath).fileName()));
        filePath = filePath;
        if (labelFile != nullptr)
            loadLabels(labelFile->shapes);
        setClean();
        adjustScale(true);
        canvas->Paint(zoomWidget->value());
        addRecentFile(filePath);
        toggleActions(true);

//        # Label xml file and show bound box according to its filename
//        # if self.usingPascalVocFormat is true:
        if (defaultSaveDir != ""){
            auto basename = QFileInfo(filePath).fileName();
            auto xmlPath = defaultSaveDir +"/"+ basename + XML_EXT;
            auto txtPath = defaultSaveDir +"/"+ basename + TXT_EXT;

//            """Annotation file priority:
//            PascalXML > YOLO
//            """
            if (QFileInfo(xmlPath).isFile())
                loadPascalXMLByFilename(xmlPath);
            else if (QFileInfo(txtPath).isFile())
                loadYOLOTXTByFilename(txtPath);
        }else{
            auto xmlPath = filePath + XML_EXT;
            auto txtPath = filePath + TXT_EXT;
            if (QFileInfo(xmlPath).isFile())
                loadPascalXMLByFilename(xmlPath);
            else if (QFileInfo(txtPath).isFile())
                loadYOLOTXTByFilename(txtPath);
        }
        setWindowTitle(Settings::APP_NAME+" "+filePath);

//        # Default : select last item if there is at least one item
        ui->m_iLabelsWidget->SelectLabel(ui->m_iLabelsWidget->LabelCount()-1);

        canvas->setFocus(Qt::MouseFocusReason);
        return true;
    }
    return false;
}
//void MainWin::resizeEvent(QResizeEvent* event){
//    if (canvas != nullptr && zoomMode != MANUAL_ZOOM){
//        adjustScale();
//    }
////    resizeEvent(event);
//}

double MainWin::scaleFitWindow(){
//    """Figure out the size of the pixmap in order to fit the main widget."""
    auto e = 2.0;//  # So that no scrollbars are generated.
    auto w1 = centralWidget()->width() - e;
    auto h1 = centralWidget()->height() - e;
    auto a1 = w1 / h1;
//    # Calculate a new scale value based on the pixmap's aspect ratio.
    auto w2 = canvas->pixmap.width() - 0.0;
    auto h2 = canvas->pixmap.height() - 0.0;
    auto a2 = w2 / h2;
    return a2 >= a1 ? w1 / w2 : h1 / h2;

}
double MainWin::scaleFitWidth(){
//    # The epsilon does not seem to work too well here.
    auto w = centralWidget()->width() - 2.0;
    return w / canvas->pixmap.width();

}
void MainWin::closeEvent(QCloseEvent* event){
    if (!mayContinue())
        event->ignore();
//    # If it loads images from dir, don't load it at the begining
    if (dirname == "")
        Settings::GetInstance()->Set(SETTING_FILENAME, m_CurrentFilePath);
    else
        Settings::GetInstance()->Set(SETTING_FILENAME, "");

    Settings::GetInstance()->Set(SETTING_WIN_SIZE,size());
    Settings::GetInstance()->Set(SETTING_WIN_POSE,pos());
    Settings::GetInstance()->Set(SETTING_WIN_STATE,saveState());
    Settings::GetInstance()->Set(SETTING_LINE_COLOR,lineColor);
    Settings::GetInstance()->Set(SETTING_FILL_COLOR,fillColor);
    Settings::GetInstance()->Set(SETTING_RECENT_FILES,recentFiles);
    Settings::GetInstance()->Set(SETTING_ADVANCE_MODE,!_beginner);
    if (defaultSaveDir != "" && QFileInfo(defaultSaveDir).exists())
        Settings::GetInstance()->Set(SETTING_SAVE_DIR,defaultSaveDir);
    else
        Settings::GetInstance()->Set(SETTING_SAVE_DIR,"");

    if (lastOpenDir != "" && QFileInfo(lastOpenDir).exists())
        Settings::GetInstance()->Set(SETTING_LAST_OPEN_DIR,lastOpenDir);
    else
        Settings::GetInstance()->Set(SETTING_LAST_OPEN_DIR,"");

    Settings::GetInstance()->Set(SETTING_AUTO_SAVE,autoSaving->isChecked());
    Settings::GetInstance()->Set(SETTING_SINGLE_CLASS,singleClassMode->isChecked());
    Settings::GetInstance()->Set(SETTING_PAINT_LABEL,displayLabelOption->isChecked());
    Settings::GetInstance()->Set(SETTING_DRAW_SQUARE,drawSquaresOption->isChecked());
    Settings::GetInstance()->sync();
}
void MainWin::loadRecent(QString filename){
    if (mayContinue())
        loadFile(filename);
}
QStringList MainWin::scanAllImages(QString folderPath){
    QStringList extensions;
    for(auto fmt : QImageReader::supportedImageFormats()){
        extensions << QString("*.%1").arg(QString(fmt).toLower());
    }
    QDir dir(folderPath);
    dir.setNameFilters(extensions);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    QStringList images;
    for(auto fi : list){
        images << fi.absoluteFilePath();
    }
    return images;
}
void MainWin::changeSavedirDialog(bool _value){
    QString path;
    if (defaultSaveDir != "")
        path = defaultSaveDir;
    else
        path = ".";

    QString dirpath = QFileDialog::getExistingDirectory(this,
                                                   QString("%1 - Save annotations to the directory").arg(Settings::APP_NAME), path,  QFileDialog::ShowDirsOnly
                                                   | QFileDialog::DontResolveSymlinks);

    if (dirpath != "")
        defaultSaveDir = dirpath;

    statusBar()->showMessage(QString("%1 . Annotation will be saved to %2").arg("Change saved folder", defaultSaveDir));
    statusBar()->show();
}
void MainWin::openAnnotationDialog(bool _value){
    if (m_CurrentFilePath == ""){
        statusBar()->showMessage("Please select image first");
        statusBar()->show();
        return;
    }

    QString path = (m_CurrentFilePath != "" ? QFileInfo(m_CurrentFilePath).dir().path() : ".");
    if (usingPascalVocFormat){
        QString filters = QString("Open Annotation XML file (*.xml)");
        QString filename = QFileDialog::getOpenFileName(this,QString("%1 - Choose a xml file").arg(Settings::APP_NAME), path, filters);
        loadPascalXMLByFilename(filename);
    }
}
void MainWin::openDirDialog(bool _value, QString dirpath){
    if (!mayContinue())
        return;

    auto defaultOpenDirPath = dirpath != "" ?dirpath : ".";
    if (lastOpenDir != "" && QFileInfo(lastOpenDir).exists())
        defaultOpenDirPath = lastOpenDir;
    else
        defaultOpenDirPath = QFileInfo(m_CurrentFilePath).dir().path();

    auto targetDirPath = QFileDialog::getExistingDirectory(this,
                                                 QString("%1 - Open Directory").arg(Settings::APP_NAME), defaultOpenDirPath,
                                                 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    importDirImages(targetDirPath);
}
void MainWin::importDirImages(QString dirpath){
    if (!mayContinue() || dirpath == "")
        return;

    lastOpenDir = dirpath;
    dirname = dirpath;
    m_CurrentFilePath = "";
    ui->m_iFileListWidget->clear();
    mImgList = scanAllImages(dirpath);
    openNextImg();
    for (auto imgPath : mImgList){
        auto item = new QListWidgetItem(imgPath);
        ui->m_iFileListWidget->addItem(item);
    }
}
void MainWin::verifyImg(bool _value){
    //# Proceding next image without dialog if having any label
    if (m_CurrentFilePath != ""){
        labelFile->toggleVerify();

        canvas->verified = labelFile->verified;
        canvas->Paint(zoomWidget->value());
        saveFile();
    }
}
void MainWin::openPrevImg(bool _value){
//    # Proceding prev image without dialog if having any label
    if (autoSaving->isChecked()){
        if (defaultSaveDir != ""){
            if (dirty){
                saveFile();
            }
        }else{
            changeSavedirDialog();
            return;
        }
    }

    if (!mayContinue())
        return;

    if (mImgList.size() <= 0)
        return;

    if (m_CurrentFilePath == "")
        return;

    auto currIndex = mImgList.indexOf(m_CurrentFilePath);
    if (currIndex - 1 >= 0){
        auto filename = mImgList[currIndex - 1];
        if (filename != ""){
            loadFile(filename);
        }
    }
}
void MainWin::openNextImg(bool _value){
//    # Proceding prev image without dialog if having any label
    if (autoSaving->isChecked()){
        if (defaultSaveDir != ""){
            if (dirty)
                saveFile();
        }else{
            changeSavedirDialog();
            return;
        }
    }

    if (!mayContinue()){
        return;
    }
    if (mImgList.size() <= 0){
        return;
    }
    QString filename = "";
    if (m_CurrentFilePath == ""){
        filename = mImgList[0];
    }else{
        auto currIndex = mImgList.indexOf(m_CurrentFilePath);
        if (currIndex + 1 < mImgList.size()){
            filename = mImgList[currIndex + 1];
        }
    }
    if (filename != ""){
        loadFile(filename);
    }
}
void MainWin::openFile(bool _value){
    Q_UNUSED(_value)
    if (!mayContinue())
        return;
    QString path = QFileInfo(m_CurrentFilePath).dir().path();
    QStringList formats;
    formats << "*" + LabelFile::suffix;
    for(auto fmt : QImageReader::supportedImageFormats()){
     formats << QString("*.%1").arg(QString(fmt).toLower());
    }

    QString filters = QString("Image & Label files (%1)").arg(formats.join(" "));// % ' '.join(formats + ['*%s' % LabelFile.suffix]);
    QString filename = QFileDialog::getOpenFileName(this, QString("%1 - Choose Image or Label file").arg(Settings::APP_NAME), path, filters);
    if (filename != ""){
        loadFile(filename);
    }
}
void MainWin::saveFile(bool _value){
    if (defaultSaveDir != ""){
        if (m_CurrentFilePath != ""){
            QString imgFileName = QFileInfo(m_CurrentFilePath).fileName();
            QString savedFileName = QFileInfo(m_CurrentFilePath).completeBaseName();
            QString savedPath = defaultSaveDir + "/"+ savedFileName;
            _saveFile(savedPath);
        }
    }else{
        QString imgFileDir = QFileInfo(m_CurrentFilePath).dir().path();
        QString imgFileName = QFileInfo(m_CurrentFilePath).fileName();
        QString savedFileName = QFileInfo(m_CurrentFilePath).completeBaseName();
        QString savedPath = imgFileDir + "/"+ savedFileName;
        _saveFile(labelFile != nullptr ? savedPath: saveFileDialog(false));
    }
}
void MainWin::saveFileAs(bool _value){
    _saveFile(saveFileDialog());
}
QString MainWin::saveFileDialog(bool removeExt){
    QString caption = QString("%1 - Choose File").arg(Settings::APP_NAME);
    QString filters = QString("File (*%1)").arg(LabelFile::suffix);
    QString openDialogPath = currentPath();
    QFileDialog* dlg = new QFileDialog(this, caption, openDialogPath, filters);
    dlg->setDefaultSuffix(LabelFile::suffix);
    dlg->setAcceptMode(QFileDialog::AcceptSave);
    auto filenameWithoutExtension = m_CurrentFilePath;
    dlg->selectFile(filenameWithoutExtension);
    dlg->setOption(QFileDialog::DontUseNativeDialog, false);
    if (dlg->exec()){
        auto fullFilePath = dlg->selectedFiles()[0];
        if (removeExt){
            return fullFilePath; //# Return file path without the extension.;
        }else{
            return fullFilePath;
        }
    }
    return "";
}
void MainWin::_saveFile(QString annotationFilePath){
    if (annotationFilePath != "" && saveLabels(annotationFilePath)){
        setClean();
        statusBar()->showMessage(QString("Saved to  %1'").arg(annotationFilePath));
        statusBar()->show();
    }
}
void MainWin::closeFile(bool _value){
    if (!mayContinue())
        return;
    resetState();
    setClean();
    toggleActions(false);
    canvas->setEnabled(false);
    actions.saveAs->setEnabled(false);
}
void MainWin::resetAll(){
//    self.settings.reset()
    close();
//    proc = QProcess();
//    proc.startDetached(os.path.abspath(__file__));

}
bool MainWin::mayContinue(){
    return (! (dirty && ! discardChangesDialog()));
}
bool MainWin::discardChangesDialog(){
    auto yes = QMessageBox::Yes;
    auto no = QMessageBox::No;
    auto msg = "You have unsaved changes, proceed anyway?";
    return yes == QMessageBox::warning(this, "Attention", msg, yes | no);

}
QMessageBox::StandardButton MainWin::errorMessage(QString title, QString message){
    return QMessageBox::critical(this, title,
                                QString("<p><b>%1</b></p>%2").arg(title).arg(message));
}
QString MainWin::currentPath(){
    return QFileInfo(m_CurrentFilePath).dir().path();// (self.filePath) if self.filePath else '.'

}
void MainWin::chooseColor1(){
    QColor color = colorDialog->getColor(lineColor, "Choose line color",DEFAULT_LINE_COLOR);
    if (color.isValid()){
        lineColor = color;
        Shape::line_color = color;
        canvas->setDrawingColor(color);
        canvas->update();
        setDirty();
    }
}
void MainWin::deleteSelectedShape(){
    ui->m_iLabelsWidget->RemLabel(canvas->deleteSelected());
    setDirty();
    if (noShapes()){
        for (auto action : actions.onShapesPresent){
            if (qobject_cast<QMenu*>(action) != nullptr){
             qobject_cast<QMenu*>(action)->setEnabled(false);
            }else if (qobject_cast<QAction*>(action) != nullptr){
             qobject_cast<QAction*>(action)->setEnabled(false);
            }
        }
    }
}

void MainWin::copyShape(){
    canvas->endMove(true);
    addLabel(canvas->GetSelectedShape());
    setDirty();
}
void MainWin::moveShape(){
    canvas->endMove(false);
    setDirty();
}
void MainWin::loadPascalXMLByFilename(QString xmlPath){
//    if self.filePath is None:
//        return
//    if os.path.isfile(xmlPath) is false:
//        return

//    self.set_format(FORMAT_PASCALVOC)

//    tVocParseReader = PascalVocReader(xmlPath)
//    shapes = tVocParseReader.getShapes()
//    self.loadLabels(shapes)
//    self.canvas.verified = tVocParseReader.verified
}
void MainWin::loadYOLOTXTByFilename(QString txtPath){
//    if self.filePath is None:
//        return
//    if os.path.isfile(txtPath) is false:
//        return

//    self.set_format(FORMAT_YOLO)
//    tYoloParseReader = YoloReader(txtPath, self.image)
//    shapes = tYoloParseReader.getShapes()
//    print (shapes)
//    self.loadLabels(shapes)
//    self.canvas.verified = tYoloParseReader.verified

}
void MainWin::togglePaintLabelsOption(){
    for (auto shape : canvas->shapes){
        shape->paintLabel = displayLabelOption->isChecked();
    }
}

void MainWin::showTutorialDialog()
{

}
