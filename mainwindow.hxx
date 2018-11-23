#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <AIS_InteractiveContext.hxx>

class OccView;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void makeCylindricalHelix(void);

    //! make conical helix.
    void makeConicalHelix(void);

    //! make toroidal helix.
    void makeToroidalHelix(void);

    TopoDS_Shape reCreateBottle(const double width, const double height, const double thick);
private slots:
    //创建元素
    void makeBox(void);
    void makeCone(void);
    void makeSphere(void);
    void makeCylinder(void);
    void makeTorus(void);
    //修改
    void makeFillet(void);
    void makeChamfer(void);
    void makeExtrude(void);
    void makeRevol(void);
    void makeLoft(void);
    //布尔
    void testCut(void);
    void testFuse(void);
    void testCommon(void);
    //螺旋线
    void testHelix(void);
    //test
    void makeCircle(void);
private:
    Ui::MainWindow *ui;
    OccView* myOccView;
};

#endif // MAINWINDOW_H
