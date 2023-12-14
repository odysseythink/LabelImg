#ifndef MAINWIN_H
#define MAINWIN_H

#define False false
#define True true

#include <QMainWindow>
#include <QCheckBox>
#include <QLineEdit>
#include <QToolButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDockWidget>
#include <QScrollArea>
#include <QMap>
#include <QScrollBar>
#include <QDockWidget>
#include <QAction>
#include <QWidgetAction>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QKeyEvent>
#include <QListWidgetItem>
#include <functional>
#include <QCursor>
#include <QWidget>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QMessageBox>
#include "libs/label_dialog.h"
#include "libs/zoom_widget.h"
#include "libs/color_dialog.h"
#include "libs/canvas.h"
#include "libs/toolbar.h"
#include "libs/labelfile.h"

using ScalerFunCB = std::function<double()>;
using EventFunCB = std::function<void ()>;

struct ActionSet{
    QAction* save;
    QAction* save_format;
    QAction* saveAs;
    QAction* open;
    QAction* close;
    QAction* resetAll;
    QAction* lineColor;
    QAction* create;
    QAction* deleteAction;
    QAction* edit;
    QAction* copy;
    QAction* createMode;
    QAction* editMode;
    QAction* advancedMode;
    QAction* shapeLineColor;
    QAction* shapeFillColor;
    QAction* zoom;
    QAction* zoomIn;
    QAction* zoomOut;
    QAction* zoomOrg;
    QAction* fitWindow;
    QAction* fitWidth;
    QList<QObject*> zoomActions;
    QList<QObject*> fileMenuActions;
    QList<QObject*> beginner;
    QList<QObject*> advanced;
    QList<QObject*> editMenu;
    QList<QObject*> beginnerContext;
    QList<QObject*> advancedContext;
    QList<QObject*> onLoadActive;
    QList<QObject*> onShapesPresent;
};

struct MenuSet{
    QMenu* file;
    QMenu* edit;
    QMenu* view;
    QMenu* help;
    QMenu* recentFiles;
    QMenu* labelList;
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWin; }
QT_END_NAMESPACE

class MainWin : public QMainWindow
{
    Q_OBJECT

public:
    MainWin(QString defaultFilename="", QString defaultPrefdefClassFile="", QString defaultSaveDir="", QWidget *parent = nullptr);
    ~MainWin();
    enum {FIT_WINDOW, FIT_WIDTH, MANUAL_ZOOM};

protected:
    virtual void keyReleaseEvent(QKeyEvent *ev);
    virtual void keyPressEvent(QKeyEvent *ev);
//    virtual void resizeEvent(QResizeEvent *ev);
    virtual void closeEvent(QCloseEvent *event);

private:
    QAction* newAction(QString text, const char* slot = nullptr, QString shortcut = "", QString icon="",
                  QString tip="", bool enabled=true, bool checkable=false);
    double scaleFitWindow();
    double scaleManualZoom();
    QMenu* menu(QString title, QList<QObject *> actions=QList<QObject *>());
    ToolBar* toolbar(QString title, QList<QObject *> actions=QList<QObject *>());
    void set_format(const QString strSaveFormat);
    void change_format();
    bool noShapes();
    void populateModeActions();
    bool beginner();
    bool advanced();
    void setBeginner();
    void setAdvanced();
    void setDirty();
    void setClean();
    void queueEvent(EventFunCB function);
    void status(QString  message, int delay=5000);
    void resetState();
    QListWidgetItem* currentItem();
    void addRecentFile(QString filePath);
    void showInfoDialog();
    void createShape();
    void setCreateMode();
    void setEditMode();
    void popLabelListMenu(QPointF point);
    void addLabel(Shape* shape);
    void remLabel(Shape* shape);
    void loadLabels(QList<Shape*> shapes);
    bool saveLabels(QString annotationFilePath);
    void copySelectedShape();
    void newShape();
    void paintCanvas();
    double scaleFitWidth();
    void loadRecent(QString filename);
    QStringList scanAllImages(QString folderPath);
    void openDirDialog(bool _value=false, QString dirpath="");
    void importDirImages(QString dirpath);
    QString saveFileDialog(bool removeExt=true);
    void _saveFile(QString annotationFilePath);
    bool mayContinue();
    bool discardChangesDialog();
    QMessageBox::StandardButton errorMessage(QString title, QString message);
    QString currentPath();
    void chooseColor1();
    void deleteSelectedShape();
    void chshapeLineColor();
    void chshapeFillColor();
    void copyShape();
    void moveShape();
    void loadPredefinedClasses(QString predefClassesFile);
    void loadPascalXMLByFilename(QString xmlPath);
    void loadYOLOTXTByFilename(QString txtPath);
    void togglePaintLabelsOption();
    void toogleDrawSquare();

private slots:
    void toggleAdvancedMode(bool value=true);
    void toggleActions(bool value=true);
    void btnstate(int stat);
    void labelSelectionChanged(QListWidgetItem *item = nullptr);
    void editLabel(QListWidgetItem *item);
    void labelItemChanged(QListWidgetItem *item);
    void fileitemDoubleClicked(QListWidgetItem *item = nullptr);
    void toggleDrawingSensitive(bool drawing=true);
    void toggleDrawMode(bool edit=true);
    void updateFileMenu();
    void shapeSelectionChanged(bool selected=false);
    void scrollRequest(double delta, int orientation);
    void setZoom(int value = 0);
    void addZoom(int increment=10);
    void zoomRequest(int delta);
    void setFitWindow(bool value=true);
    void setFitWidth(bool value=true);
    void togglePolygons(bool value);
    bool loadFile(QString filePath="");
    void adjustScale(bool initial=false);
    void changeSavedirDialog(bool _value=false);
    void openAnnotationDialog(bool _value=false);
    void verifyImg(bool _value=false);
    void openPrevImg(bool _value=false);
    void openNextImg(bool _value=false);
    void openFile(bool _value=false);
    void saveFile(bool _value=false);
    void saveFileAs(bool _value=false);
    void closeFile(bool _value=false);
    void resetAll();

private:
    Ui::MainWin *ui;
    bool usingYoloFormat;
    bool usingPascalVocFormat;
    QString defaultSaveDir;
    QStringList labelHist;
    LabelDialog* labelDialog;
    QCheckBox* useDefaultLabelCheckbox;
    QLineEdit* defaultLabelTextLine;
    QCheckBox* diffcButton;
    QToolButton* editButton;
    QListWidget* labelList;
//    QDockWidget* dock;
    QListWidget* fileListWidget;
//    QDockWidget* filedock;
    ZoomWidget* zoomWidget;
    ColorDialog* colorDialog;
    Canvas* canvas;
    QString filePath;
    QMap<Qt::Orientation, QScrollBar *> scrollBars;
    QScrollArea *scrollArea;
    int dockFeatures;
    int zoomMode;
    QMap<int, ScalerFunCB> scalers;
    QAction* drawSquaresOption;
    ActionSet actions;
    MenuSet menus;
    QAction* autoSaving;
    QAction* singleClassMode;
    QString lastLabel;
    QAction* displayLabelOption;
    QImage* image;
    QStringList recentFiles;
    int maxRecent;
    QColor lineColor;
    QColor fillColor;
    int zoom_level;
    bool fit_window;
    bool difficult;
    QString lastOpenDir;
    QLabel* labelCoordinates;
    QMap<QListWidgetItem*, Shape*> itemsToShapes;
    QMap<Shape*, QListWidgetItem*> shapesToItems;
    QString dirname;
    QStringList mImgList;
    bool dirty;
    bool _noSelectionSlot;
    bool _beginner;
    QByteArray imageData;
    LabelFile* labelFile;
    QString prevLabelText;
};
#endif // MAINWIN_H
