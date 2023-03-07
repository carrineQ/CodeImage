#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>
#include <QMainWindow>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>
#include <QDebug>
#include <QStandardPaths>
#include <QTranslator>
#include <QGraphicsView>
#include <QWheelEvent>
#include "algorithm.h"     //这里体现了调用自己创建的动态库，在.pro文件中有LIBS += -L$$PWD/lib/ -limageDLL
#include "operation.h"

namespace Ui { class MainWindow; }


class MainWindow : public QMainWindow,operation
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateRightImage(QPixmap &pixmap) final;//更新右边图片
    void cleanImage() final;//清理图片
    void do_adjust() final;//调整图片适应界面
    void adjust_left() final;
    void adjust_right() final;
    void createToolBar();//创建工具栏

private slots://设计界面生成的槽函数
    void on_doOpen_triggered();//打开

    void on_doSave_triggered();//保存

    void on_doSaveAs_triggered();//另存为

    void on_doExit_triggered();//关闭

    void on_doSimple_triggered();//简单模糊

    void on_doLaplace_triggered();//拉普拉斯锐化

    void on_doRestore_triggered();//恢复

    void on_Binaryzation_triggered();//二值化

    void on_doCool_triggered();//冷色调

    void on_doWarm_triggered();//暖色调

private:
    Ui::MainWindow *ui;
    //左右两个图片
    QGraphicsScene  *leftScene;
    QGraphicsScene  *rightScene;
    QGraphicsPixmapItem *leftPixmapItem;
    QGraphicsPixmapItem *rightPixmapItem;
    //用vector存储改变的图片
    vector<QPixmap> changedImage;
    //定义算法类,用以调用算法
    Algorithm *algorithm;
    //用于存放图片地址（下次打开，保存，另存）
    QString ConstImage;
    //判定是否修改图片
    bool flag;
    //用于获取桌面位置
    QString getUserPath();
};
#endif // MAINWINDOW_H

