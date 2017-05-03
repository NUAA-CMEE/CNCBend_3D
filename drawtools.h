#ifndef DRAWTOOLS_H
#define DRAWTOOLS_H

#include <QWidget>
#include <QPainter>
#include "sysglobalvars.h"

class DrawTools : public QWidget
{
    Q_OBJECT

public:
    explicit DrawTools(QWidget *parent = 0);
    ~DrawTools();

    void resizeWindow(int width,int height);
    void refreshGraph();

protected:
    void paintEvent(QPaintEvent *e);
    QPixmap *drawPix();

private:
    int iWidth;
    int iHeight;

};

#endif // DRAWTOOLS_H
