#ifndef OCCVIEW_HXX
#define OCCVIEW_HXX
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QRubberBand>
#include <AIS_InteractiveContext.hxx>
class OccView:public QOpenGLWidget,QOpenGLFunctions
{
    Q_OBJECT
public:
    enum CurrentAction3d
    {
        CurAction3d_Nothing,
        CurAction3d_DynamicZooming,
        CurAction3d_WindowZooming,
        CurAction3d_DynamicPanning,
        CurAction3d_GlobalPanning,
        CurAction3d_DynamicRotation
    };

    OccView(QWidget* parent);
    const Handle(AIS_InteractiveContext)& getContext() const;
protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent* e);
    //mouse events.
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

    //button events.
    virtual void onLButtonDown(const int theFlags,const QPoint thePoint);
    virtual void onMButtonDown(const int theFlags,const QPoint thePoint);
    virtual void onRButtonDown(const int theFlags,const QPoint thePoint);
    virtual void onMouseWheel(const int theFlags, const int theDelta, const QPoint thePoint);
    virtual void onLButtonUp(const int theFlags, const QPoint thePoint);
    virtual void onMButtonUp(const int theFlags, const QPoint thePoint);
    virtual void onRButtonUp(const int theFlags, const QPoint thePoint);
    virtual void onMouseMove(const int theFlags,const QPoint thePoint);
protected:
    void init(void);    //initview
    void popup(const int x,const int y);
    void dragEvent(const int x, const int y);
    void inputEvent(const int x,const int y);
    void moveEvent(const int x, const int y);
    void multiMoveEvent(const int x, const int y);
    void multiDragEvent(const int x, const int y);
    void multiInputEvent(const int x, const int y);
    void drawRubberBand(const int minX,const int minY,const int maxX,const int maxY);
    void panByMiddleButton(const QPoint& thePoint);
signals:
    void selectionChanged(void);
public slots:
    void pan(void);
    void fitAll(void);
    void reset(void);
    void zoom(void);
    void rotate(void);
private:
    Handle(V3d_Viewer)  myViewer;   //occ viewer

    Handle(V3d_View) myView;    //occ view

    Handle(AIS_InteractiveContext) myContext;   //occ context

    //save the mouse position
    Standard_Integer myXmin;
    Standard_Integer myYmin;
    Standard_Integer myXmax;
    Standard_Integer myYmax;

    CurrentAction3d myCurrentMode;  //the mouse current mode
    Standard_Boolean myDegenerateModeIsOn;

    QRubberBand* myRectBand;
};

#endif // OCCVIEW_HXX
