#include <OpenGl_GraphicDriver.hxx>

#include "occview.hxx"
#include "qdebug.h"
#include <QStyleFactory>
#include <QMouseEvent>

#include <V3d_View.hxx>
#include <Aspect_Handle.hxx>
#include <Aspect_DisplayConnection.hxx>

#ifdef WNT
    #include <WNT_Window.hxx>
#else
    #include <Xw_Window.hxx>
#endif
static Handle(Graphic3d_GraphicDriver)& GetGraphicDriver()
{
    static Handle(Graphic3d_GraphicDriver) aGraphicDriver;
    return aGraphicDriver;
}

OccView::OccView(QWidget* parent)
    :QOpenGLWidget(parent),
      myXmin(0),
      myYmin(0),
      myXmax(0),
      myYmax(0),
      myCurrentMode(CurAction3d_DynamicRotation),
      myDegenerateModeIsOn(Standard_True)
      //myRectBand(NULL)
{
    this->setBackgroundRole(QPalette::NoRole);
    this->setMouseTracking(true);
}

void OccView::init()
{
    Handle(Aspect_DisplayConnection) aDisplayConnection=new Aspect_DisplayConnection();
    if(GetGraphicDriver().IsNull())
    {
        GetGraphicDriver()=new OpenGl_GraphicDriver(aDisplayConnection);
    }
    WId window_handle=(WId) winId();
#ifdef WNT
    Handle(WNT_Window) wind=new WNT_Window((Aspect_Handle) window_handle);
#else
    Handel(Xw_Window) wind=new Xw_Window(aDisplayConnection,(Window) window_handle);
#endif
    //crate v3dviewer and v3d_view
    myViewer=new V3d_Viewer(GetGraphicDriver(),Standard_ExtString("viewer3d"));
    myView=myViewer->CreateView();
    myView->SetWindow(wind);
    if(!wind->IsMapped()) wind->Map();
    myContext = new AIS_InteractiveContext(myViewer);

    // Set up lights etc
    myViewer->SetDefaultLights();
    myViewer->SetLightOn();

    myView->SetBackgroundColor(Quantity_NOC_BLACK);
    myView->MustBeResized();
    myView->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, 0.08, V3d_ZBUFFER);

    myContext->SetDisplayMode(AIS_Shaded, Standard_True);
}

const Handle(AIS_InteractiveContext)& OccView::getContext() const
{
    return myContext;
}

void OccView::paintEvent(QPaintEvent *e)
{
    if(myContext.IsNull())
    {
        init();
    }
    myView->Redraw();
}
//===========================修改大小窗口================
void OccView::resizeEvent(QResizeEvent *e)
{
    if(!myView.IsNull())
    {
        myView->MustBeResized();
    }
}

void OccView::reset(void)
{
    myView->Reset();
}

void OccView::pan(void)
{
    myCurrentMode=CurAction3d_DynamicPanning;
}

void OccView::zoom(void)
{
    myCurrentMode=CurAction3d_DynamicZooming;
}

void OccView::rotate(void)
{
    myCurrentMode=CurAction3d_DynamicRotation;
}

void OccView::fitAll(void)
{
    myView->FitAll();
    myView->ZFitAll();
    myView->Redraw();
}

void OccView::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        onLButtonDown(event->buttons()|event->modifiers(),event->pos());
    }else if (event->button()==Qt::MidButton)
    {
        onMButtonDown((event->buttons()|event->modifiers()),event->pos());
    }else if(event->button()==Qt::RightButton)
    {
        onRButtonDown((event->buttons()|event->modifiers()),event->pos());
    }
}

void OccView::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        onLButtonDown(event->buttons()|event->modifiers(),event->pos());
    }else if(event->button()==Qt::MidButton)
    {
        onMButtonDown(event->buttons()|event->modifiers(),event->pos());
    }else if(event->button()==Qt::RightButton)
    {
        onRButtonDown(event->buttons()|event->modifiers(),event->pos());
    }
}

void OccView::mouseMoveEvent(QMouseEvent *event)
{
    onMouseMove(event->buttons(),event->pos());
}

void OccView::wheelEvent(QWheelEvent *event)
{
    onMouseWheel(event->buttons(),event->delta(),event->pos());
}

void OccView::onLButtonDown(const int /*theFlags*/, const QPoint thePoint)
{
    myXmin=thePoint.x();
    myYmin=thePoint.y();
    myXmax=thePoint.x();
    myYmax=thePoint.y();
}

void OccView::onMButtonDown(const int theFlags, const QPoint thePoint)
{
    myXmin=thePoint.x();
    myYmin=thePoint.y();
    myXmax=thePoint.x();
    myYmax=thePoint.y();

    if(myCurrentMode==CurAction3d_DynamicRotation)
    {
        myView->StartRotation(thePoint.x(),thePoint.y());
    }
}

void OccView::onRButtonDown(const int theFlags, const QPoint thePoint)
{
    Q_UNUSED(theFlags);
    Q_UNUSED(thePoint);
}

void OccView::onMouseWheel( const int /*theFlags*/, const int theDelta, const QPoint thePoint)
{
    Standard_Integer aFactor=16;

    Standard_Integer aX=thePoint.x();
    Standard_Integer aY=thePoint.y();

    if(theDelta>0)
    {
        aX+=aFactor;
        aY+=aFactor;
    }else{
        aX-=aFactor;
        aY-=aFactor;
    }
    myView->Zoom(thePoint.x(),thePoint.y(),aX,aY);
}

void OccView::popup( const int /*x*/, const int /*y*/ )
{
}

void OccView::onLButtonUp(const int theFlags, const QPoint thePoint)
{
    // Hide the QRubberBand
    if (myRectBand)
    {
        myRectBand->hide();
    }

    // Ctrl for multi selection.
    if (thePoint.x() == myXmin && thePoint.y() == myYmin)
    {
        if (theFlags & Qt::ControlModifier)
        {
            multiInputEvent(thePoint.x(), thePoint.y());
        }
        else
        {
            inputEvent(thePoint.x(), thePoint.y());
        }
    }
}

void OccView::onMButtonUp( const int /*theFlags*/, const QPoint thePoint )
{
    if (thePoint.x() == myXmin && thePoint.y() == myYmin)
    {
        panByMiddleButton(thePoint);
    }
}

void OccView::dragEvent(const int x, const int y)
{
    myContext->Select(myXmin, myYmin, x, y, myView, Standard_True);
}

void OccView::onRButtonUp( const int /*theFlags*/, const QPoint thePoint )
{
    popup(thePoint.x(), thePoint.y());
}

void OccView::multiDragEvent( const int x, const int y )
{
    myContext->ShiftSelect(myXmin, myYmin, x, y, myView, Standard_True);

    emit selectionChanged();

}

void OccView::inputEvent( const int x, const int y )
{
    Q_UNUSED(x);
    Q_UNUSED(y);

    myContext->Select(Standard_True);

    emit selectionChanged();
}

void OccView::multiInputEvent( const int x, const int y )
{
    Q_UNUSED(x);
    Q_UNUSED(y);

    myContext->ShiftSelect(Standard_True);

    emit selectionChanged();
}

void OccView::onMouseMove(const int theFlags, const QPoint thePoint)
{
    // Draw the rubber band.
    if (theFlags & Qt::LeftButton)
    {
        drawRubberBand(myXmin, myYmin, thePoint.x(), thePoint.y());

        dragEvent(thePoint.x(), thePoint.y());
    }

    // Ctrl for multi selection.
    if (theFlags & Qt::ControlModifier)
    {
        //multiMoveEvent(thePoint.x(), thePoint.y());
    }
    else
    {
        moveEvent(thePoint.x(), thePoint.y());
    }

    // Middle button.
    if (theFlags & Qt::MidButton)
    {
        switch (myCurrentMode)
        {
        case CurAction3d_DynamicRotation:
            myView->Rotation(thePoint.x(), thePoint.y());
            break;

        case CurAction3d_DynamicZooming:
            myView->Zoom(myXmin, myYmin, thePoint.x(), thePoint.y());
            break;

        case CurAction3d_DynamicPanning:
            myView->Pan(thePoint.x() - myXmax, myYmax - thePoint.y());
            myXmax = thePoint.x();
            myYmax = thePoint.y();
            break;

         default:
            break;
        }
    }

}

void OccView::drawRubberBand(const int minX, const int minY, const int maxX, const int maxY)
{
    QRect aRect;
    (minX<maxX)?(aRect.setX(minX)):(aRect.setX(maxX));
    (minY<maxY)?(aRect.setY(minY)):(aRect.setY(maxY));

    aRect.setWidth(abs(maxX - minX));
    aRect.setHeight(abs(maxY - minY));

    if (!myRectBand)
    {
        myRectBand = new QRubberBand(QRubberBand::Rectangle, this);

        // setStyle is important, set to windows style will just draw
        // rectangle frame, otherwise will draw a solid rectangle.
        myRectBand->setStyle(QStyleFactory::create("windows"));
    }

    myRectBand->setGeometry(aRect);
    myRectBand->show();
}

void OccView::panByMiddleButton( const QPoint& thePoint )
{
    Standard_Integer aCenterX = 0;
    Standard_Integer aCenterY = 0;

    QSize aSize = size();

    aCenterX = aSize.width() / 2;
    aCenterY = aSize.height() / 2;

    myView->Pan(aCenterX - thePoint.x(), thePoint.y() - aCenterY);
}

void OccView::moveEvent(const int x, const int y)
{
    myContext->MoveTo(x, y, myView, Standard_True);
}
