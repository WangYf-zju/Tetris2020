#ifndef VIDEO_H
#define VIDEO_H

#include <QWidget>
#include <QThread>
#include <QTime>
#include <QList>
#include <QDebug>
#include <QMutex>
#include <Windows.h>
#include "HalconCpp.h"
using namespace HalconCpp;

#define TYPE_COUNT 7

#define SPEED 0.0463
#define YMAX 200

#define PXSCALEX 0.5531
#define PXSCALEY 0.5361
#define OFFSETX (-278.4)
#define OFFSETY (-426.1+3)

const double CenterPointOffsetX[TYPE_COUNT] = { 4, -8, 0, -1, -4, -6, -9 };
const double CenterPointOffsetY[TYPE_COUNT] = { 3, 1, 2, 1, 1, -5, -5 };
const double GrabPointOffset[TYPE_COUNT] = { 9, 9, 0, 0, 0, 0, 9 };
const double TestOffsetX[TYPE_COUNT] = { 0 };
const double TestOffsetY[TYPE_COUNT] = { 0 };

class TetrisInfo
{
public:
    TetrisInfo(int type, HTuple hv_Row, HTuple hv_Column, HTuple hv_Angle, QTime t)
    {
        double angle = hv_Angle.D();
        double centerx = hv_Column.D() + cos(angle) * CenterPointOffsetX[type] +
            sin(angle) * CenterPointOffsetY[type];
        double centery = hv_Row.D() - sin(angle) * CenterPointOffsetX[type] +
            cos(angle) * CenterPointOffsetY[type];
        this->type = type;
        this->angle = 270 - hv_Angle.TupleDeg().D();
        if (this->angle > 180) this->angle -= 360;
        //this->x = centerx;
        //this->y = centery;
        this->x = centery * PXSCALEX + OFFSETX + 
            sin(HTuple(-this->angle).TupleRad().D()) * GrabPointOffset[type] + TestOffsetX[type];
        this->y = centerx * PXSCALEY + OFFSETY -
            cos(HTuple(-this->angle).TupleRad().D()) * GrabPointOffset[type] + TestOffsetY[type];
        this->t = t;
        this->valid = true;
    }
    TetrisInfo(int type, double x, double y, double angle, QTime t)
    {
        this->type = type;
        this->x = x;
        this->y = y;
        this->angle = angle;
        this->t = t;
        this->valid = true;
    }
    bool UpdateInfo(QTime t)
    {
        double dt = t.msecsTo(this->t);
        y -= SPEED * (t.msecsTo(this->t));
        this->t = t;
        if (y > YMAX) return false;
        return true;
    }
    int type;
    double x, y, angle;
    QTime t;
    bool valid;
};

class Video : public QThread
{
public:
    Video();
    ~Video();

protected:
    void run();

public:
    bool OpenCamera(char *index, QWidget *widget);
    void CloseCamera();
    bool IsCameraOpen();
    QList<TetrisInfo> grabList;
    QList<TetrisInfo> viewList;
    QMutex lock;

private:
    void Distinguish(HObject &img);
    void DrawContour(int type, HTuple hv_Row, HTuple hv_Column, 
        HTuple hv_Angle, HTuple hv_Scale, const char * color);
    void AddTetrisToGrabList(TetrisInfo &ti, QTime time);
    void AddTetrisToViewList(TetrisInfo &ti);
    bool isCameraOpen;
    HTuple hv_ModelID[TYPE_COUNT];
    HTuple hv_WindowHandle, hv_AcqHandle;
    HTuple hl_width, hl_height;
};

#endif // VIDEO_H
