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
#include <QActionGroup>
#include <QFileSystemModel>
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
    QActionGroup* modeActionGroup;
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
                  QString tip="", bool enabled=true, bool checkable=false, QWidget* parent = nullptr);
    double scaleFitWindow();
    double scaleManualZoom();
    QMenu* menu(QString title, QList<QObject *> actions=QList<QObject *>());
    ToolBar* toolbar(QString title, QList<QObject *> actions=QList<QObject *>());
    void set_format(const QString strSaveFormat);
    bool noShapes();
    void populateModeActions();
    bool beginner();
    bool advanced();
    void setBeginner();
    void setAdvanced();
    void setClean();
    void queueEvent(EventFunCB function);
    void status(QString  message, int delay=5000);
    void resetState();
    void addRecentFile(QString m_CurrentFilePath);
    void addLabel(Shape* shape);
    void loadLabels(QList<QSharedPointer<Shape>> shapes);
    bool saveLabels(QString annotationFilePath);
    double scaleFitWidth();
    void loadRecent(QString filename);
    QStringList scanAllImages(QString folderPath);
    void importDirImages(QString dirpath);
    QString saveFileDialog(bool removeExt=true);
    void _saveFile(QString annotationFilePath);
    bool mayContinue();
    bool discardChangesDialog();
    QMessageBox::StandardButton errorMessage(QString title, QString message);
    QString currentPath();
    void loadPredefinedClasses(QString predefClassesFile);
    void loadPascalXMLByFilename(QString xmlPath);
    void loadYOLOTXTByFilename(QString txtPath);

private slots:
    void copyShape();
    void moveShape();
    void togglePaintLabelsOption();
    void showInfoDialog();
    void showTutorialDialog();
    void copySelectedShape();
    void deleteSelectedShape();
    void createShape();
    void __OnShapeModeChanged();
    void chooseColor1();
    void change_format();
    void openDirDialog(bool _value=false, QString dirpath="");
    void newShape();
    void setDirty();
    void toggleAdvancedMode(bool value=true);
    void toggleActions(bool value=true);
    void btnstate(int stat);
    void editLabel(QListWidgetItem *item = nullptr);
    void labelItemChanged(QListWidgetItem *item);
    void fileItemDoubleClicked(QListWidgetItem *item = nullptr);
    void toggleDrawingSensitive(bool drawing=true);
    void updateFileMenu();
    void shapeSelectionChanged(bool selected=false);
    void scrollRequest(double delta, int orientation);
    void setZoom(int value = 0);
    void addZoom(int increment=10);
    void zoomRequest(int delta);
    void setFitWindow(bool value=true);
    void setFitWidth(bool value=true);
    void togglePolygons(bool value = false);
    bool loadFile(QString m_CurrentFilePath="");
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

signals:
    void sigCreateShape();

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
    QString m_CurrentFilePath;
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
    QStringList recentFiles;
    int maxRecent;
    QColor lineColor;
    QColor fillColor;
    int zoom_level;
    bool fit_window;
    bool difficult;
    QString lastOpenDir;
    QLabel* labelCoordinates;
    QString dirname;
    QStringList mImgList;
    bool dirty;
    bool _noSelectionSlot;
    bool _beginner;
    QByteArray imageData;
    LabelFile* labelFile;
    QString prevLabelText;
    QFileSystemModel m_iFileListView;
};
#endif // MAINWIN_H
