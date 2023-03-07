# include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QGraphicsPixmapItem>


MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //初始化
    leftScene = new QGraphicsScene;//场景,用于管理图形界面
    leftPixmapItem = new QGraphicsPixmapItem();//存图片
    rightScene = new QGraphicsScene;
    rightPixmapItem = new QGraphicsPixmapItem();

    ConstImage="";
    flag=false;//用于退出确认

    //初始化界面，设置场景
    leftScene->setBackgroundBrush(QColor::fromRgb(224,224,224));
    ui->leftgraphicsView->setScene(leftScene);
    rightScene->setBackgroundBrush(QColor::fromRgb(224,224,224));
    ui->rightgraphicsView->setScene(rightScene);

    createToolBar();
}

void MainWindow::createToolBar()
{
    ui->file->addAction(ui->doOpen);
    ui->file->addAction(ui->doSave);
    ui->file->addAction(ui->doSaveAs);
    ui->file->addAction(ui->doRestore);
    ui->file->addAction(ui->doExit);

    ui->handle->addAction(ui->doSimple);
    ui->handle->addAction(ui->doLaplace);
    ui->handle->addAction(ui->Binaryzation);

    ui->hue->addAction(ui->doCool);
    ui->hue->addAction(ui->doWarm);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/************************打开*********************************/
/*先判断界面中是否有图片被更改，再通过用户选取图片路径，再用QPixmap声明一个对象，在界面中添加该对象后再进行调整。*/
void MainWindow::on_doOpen_triggered()
{
    if(flag){
        if(QMessageBox::Yes==QMessageBox::question(this,tr("尚未保存"),tr("是否返回操作？"),//依次代表组件、title、消息框、按钮、默认按钮
                                                   QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes)){
            return;
        }
    }
    if(ConstImage==""){
        ConstImage=getUserPath();//获取用户的桌面目录
    }
    //依次代表组件、title、路径、filter
    QString imagePath = QFileDialog::getOpenFileName(this, tr("Open image"), ConstImage,tr("Images (*.bpm *.jpg *.jpeg *.png"
                                                                                           " *.tif *.pcx *.tga *.svg *.ico)"));
    ConstImage=imagePath;
    //如果这个目录存在
    if (!imagePath.isEmpty())//地址非空
    {
        //将之前的图片清除掉
        cleanImage();
        //左框显示图片
        QPixmap leftPixmap(imagePath);//图片地址
        leftPixmapItem = leftScene->addPixmap(leftPixmap);//设置图片
        leftScene->setSceneRect(QRectF(leftPixmap.rect()));//更新坐标为使图片不会偏离界面
        //右框显示图片
        QPixmap rightPixmap(imagePath);
        rightPixmapItem = rightScene->addPixmap(rightPixmap);
        rightScene->setSceneRect(QRectF(rightPixmap.rect()));
        if(!changedImage.empty())//如果容器非空，则将容器清空后添加新图片；容器为空，则直接向容器中添加新图片
        {
            changedImage.clear();
        }

        changedImage.push_back(rightPixmap);
        //调整使得图片适应边框
        do_adjust();
        flag=false;
    }
}

/************************获得当前用户的桌面目录*********************************/
QString MainWindow::getUserPath()
{
    QString userPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);//默认桌面路径
    return userPath;
}

/************************保存*********************************/
/*将pixmap()返回的像素图保存，在界面中通过ConstImage的路径把保存后的图片打开，最后进行调整。*/
void MainWindow::on_doSave_triggered()
{
    //首先将右边图与本地图片进行替换
    rightPixmapItem->pixmap().save(ConstImage);//保存pixmap()返回的像素图
    //清除左边框图片
    leftScene->clear();
    ui->leftgraphicsView->resetTransform();
    //左框重新显示图片
    QPixmap leftPixmap(ConstImage);
    leftPixmapItem = leftScene->addPixmap(leftPixmap);
    leftScene->setSceneRect(QRectF(leftPixmap.rect()));
    //仅调整左边图片适应窗口
    adjust_left();
    flag=false;
    if(!changedImage.empty())//清空容器
    {
        changedImage.clear();
    }
    changedImage.push_back(rightPixmapItem->pixmap());
}

/************************另存为*********************************/
/*调用getSaveFileName()方法返回用户选择的文件名给QString定义的新路径，该文件不必存在是该方法的关键属性，在检测路径后保存该文件。*/
void MainWindow::on_doSaveAs_triggered()
{
    //直接用原始图片作为地址和名字
    QString newPath = QFileDialog::getSaveFileName(this, tr("Save image"), ConstImage,
                                                   tr("All files (*);;"
                                                      "Image BPM (*.bpm);;"
                                                      "Image JPG (*.jpg);;"
                                                      "Image JPEG (*.jpeg);;"
                                                      "Image PNG (*.png);;" 
                                                      "Image TIF (*.tif);;"
                                                      "Image PCX (*.pcx);;"
                                                      "Image TGA (*.tga);;"
                                                      "Image SVG (*.svg);;"
                                                      "Image ICO (*.ico);;"));
    //检测路径并保存图片
    if (!newPath.isEmpty()) {
        QFile file(newPath);
        rightPixmapItem->pixmap().save(newPath);
        flag=false;
    }
}

/************************退出*********************************/
/*检查是否有更改，如果图片更改过则跳出消息框确认继续或返回，再通过将界面清理并重置，通过全局变量qApp调用的quit()方法退出程序。*/
void MainWindow::on_doExit_triggered()
{
    //检查是否更改
    if(flag){
        if(QMessageBox::Yes==QMessageBox::question(this,tr("尚未保存"),tr("是否返回操作再退出？"),
                                                   QMessageBox::Yes | QMessageBox::No,QMessageBox::Yes)){
            return;
        }
    }
    cleanImage();
    if(!changedImage.empty())//清空容器
    {
        changedImage.clear();
    }
    qApp->quit();//通过全局变量qApp调用QCoreApplication的quit()方法退出程序
}

/************************简单模糊*********************************/
/*将QGraphicsPixmapItem定义的对象转换为QPixmap再转换为QImage传入定义的算法当中,用滤波器进行计算并返回,用新的图像替换就图像后更新右侧图片*/
void MainWindow::on_doSimple_triggered()
{
    QPixmap rightImage = rightPixmapItem->pixmap();
    //调用Algorithm::SimpleSmooth()
    QImage newImage = algorithm->SimpleSmooth(rightImage.toImage());//像素图转化为QImage
    rightImage.convertFromImage(newImage);
    updateRightImage(rightImage);
    flag=true;
    changedImage.push_back(rightImage);//向容器中添加修改后的图片信息
}

/************************拉普拉斯锐化*********************************/
void MainWindow::on_doLaplace_triggered()
{
    QPixmap rightImage = rightPixmapItem->pixmap();
    //调用Algorithm::LaplaceSharpen()
    QImage newImage = algorithm->LaplaceSharpen(rightImage.toImage());
    rightImage.convertFromImage(newImage);
    updateRightImage(rightImage);
    flag=true;
    changedImage.push_back(rightImage);
}

/************************撤销*********************************/
/*先清除容器中图像的当前状态，再取出容器中的最后一个元素，在右侧的界面中进行显示，最后对图片进行调整。*/
void MainWindow::on_doRestore_triggered()
{
    //获取左侧图片
    QPixmap leftImage = leftPixmapItem->pixmap();
    if(!changedImage.empty())
    {
        changedImage.pop_back();
    }
    QPixmap changeImage=changedImage.back();
    //用左侧图片更新右侧
    updateRightImage(changeImage);
    //调整图像自适应
    adjust_right();
    flag=false;
}

/************************更新右边图片*********************************/
void MainWindow::updateRightImage(QPixmap &pixmap)
{
    //刷新右边图片
    rightPixmapItem->setPixmap(pixmap);
    rightScene->setSceneRect(QRectF(pixmap.rect()));
}

/************************清理图片*********************************/
void MainWindow::cleanImage()
{
    //左边框清理
    leftScene->clear();
    //重置
    ui->leftgraphicsView->resetTransform();
    //右边
    rightScene->clear();
    ui->leftgraphicsView->resetTransform();

}

/************************调整图片适应界面*********************************/
void MainWindow::do_adjust()
{
    adjust_left();
    adjust_right();
}

/************************调整左边图片适应*********************************/
void MainWindow::adjust_left()
{
    //恢复
    ui->leftgraphicsView->resetTransform();

    //左图
    int height = leftPixmapItem->pixmap().height();
    int width = leftPixmapItem->pixmap().width();
    int max_height = ui->leftgraphicsView->height();
    int max_width = ui->leftgraphicsView->width();

    //计算
    int pic_size = qMax(width,height);
    int max_size = qMin(max_width,max_height)-5;
    double val = (max_size*1.0)/pic_size;

    ui->leftgraphicsView->scale(val,val);
}

/************************调整右边图片适应*********************************/
void MainWindow::adjust_right()
{
    //恢复
    ui->rightgraphicsView->resetTransform();

    // 右图
    int height = rightPixmapItem->pixmap().height();
    int width = rightPixmapItem->pixmap().width();
    int max_height = ui->rightgraphicsView->height();
    int max_width = ui->rightgraphicsView->width();

    //计算
    int pic_size = qMax(width,height);
    int max_size = qMin(max_width,max_height)-5;
    double val = (max_size*1.0)/pic_size;

    ui->rightgraphicsView->scale(val,val);
}

/************************二值化*********************************/
void MainWindow::on_Binaryzation_triggered()
{
    QPixmap rightImage = rightPixmapItem->pixmap();
    //调用Algorithm::Binaryzation()
    QImage newImage = algorithm->Binaryzation(rightImage.toImage());
    rightImage.convertFromImage(newImage);
    updateRightImage(rightImage);
    flag=true;
    changedImage.push_back(rightImage);
}

/************************冷色调*********************************/
void MainWindow::on_doCool_triggered()
{
    QPixmap rightImage = rightPixmapItem->pixmap();
    //调用Algorithm::Cool()
    QImage newImage = algorithm->Cool(10, rightImage.toImage());
    rightImage.convertFromImage(newImage);
    updateRightImage(rightImage);
    flag=true;
    changedImage.push_back(rightImage);
}

/************************暖色调*********************************/
void MainWindow::on_doWarm_triggered()
{
    QPixmap rightImage = rightPixmapItem->pixmap();
    //调用Algorithm::Warm()
    QImage newImage = algorithm->Warm(10, rightImage.toImage());
    rightImage.convertFromImage(newImage);
    updateRightImage(rightImage);
    flag=true;
    changedImage.push_back(rightImage);
}
