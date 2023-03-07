#ifndef OPERATION_H
#define OPERATION_H

#include <QPixmap>
#include <iostream>
using namespace std;

class operation
{
public:
    virtual void updateRightImage(QPixmap &pixmap) = 0;//更新右边图片
    virtual void cleanImage() = 0;//清理图片
    virtual void do_adjust() = 0;//调整图片适应界面
    virtual void adjust_left() = 0;
    virtual void adjust_right() = 0;
};


#endif // OPERATION_H
