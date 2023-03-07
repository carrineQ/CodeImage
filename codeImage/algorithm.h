#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <QPixmap>
#include <QDebug>
#include <QtMath>

class  Algorithm
{

public:
    Algorithm();
    ~Algorithm();
    QImage SimpleSmooth(const QImage &origin);//简单平滑处理
    QImage LaplaceSharpen(const QImage &origin);//拉普拉斯锐化
    QImage Binaryzation(const QImage &origin);//二值化
    QImage Warm(int delta, QImage origin);//暖色调
    QImage Cool(int delta, QImage origin);//冷色调
};

#endif // ALGORITHM_H
