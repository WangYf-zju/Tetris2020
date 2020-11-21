#ifndef VIDEO_H
#define VIDEO_H

#include <QWidget>
#include <QThread>
#include <QTime>
#include <QList>
#include <QDebug>
#include <QMutex>
#include <Windows.h>
#include "TetrisAI.h"
#include "HalconCpp.h"
using namespace HalconCpp;

#define TYPE_COUNT 7

#define SPEED 0.0463
#define YMAX 200

#define PXSCALEX 0.5531
#define PXSCALEY 0.5361
#define OFFSETX (-278.4)
#define OFFSETY (-426.1+3)

#define PLACE_OFFSET_X 33
#define PLACE_OFFSET_Y 20
#define PLACE_Z -398 
#define GRID_SIZE 17.7
#define GRAB_Z -430

const double CenterPointOffsetX[TYPE_COUNT] = { 4, -8, 0, -1, -4, -6, -9 };
const double CenterPointOffsetY[TYPE_COUNT] = { 3, 1, 2, 1, 1, -5, -5 };
const double GrabPointOffset[TYPE_COUNT] = { 9, 9, 0, 0, 0, 0, 9 };
const double TestOffsetX[TYPE_COUNT] = { 0 };
const double TestOffsetY[TYPE_COUNT] = { 0 };

const double PlaceGridOffsetX[TYPE_COUNT][4] = {
    {1,1,1,1},{1,1,1,1},{1,0.5,1,0.5},{1,1.5,1,1.5},{0.5,0.5,0.5,0.5},{1.5,1,1.5,1},{1,1,1,1}
};
const double PlaceGridOffsetY[TYPE_COUNT][4] = {
    {1,1,1,1},{1,1,1,1},{0.5,1,0.5,1},{0.5,1,0.5,1},{0.5,0.5,0.5,0.5},{1,1.5,1,1.5},{1,1,1,1}
};

class TetrisInfo
{
public:
    TetrisInfo(int type, HTuple &hv_Row, HTuple &hv_Column, HTuple &hv_Angle, QTime &t)
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
    TetrisInfo(int type, double x, double y, double angle, QTime &t)
    {
        this->type = type;
        this->x = x;
        this->y = y;
        this->angle = angle;
        this->t = t;
        this->valid = true;
    }
    bool UpdateInfo(QTime &t)
    {
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

class TetrisList
{
protected:
    QList<TetrisInfo> list;
    QMutex lock;
    void UpdateAllTetrisInfo()
    {
        Lock();
        QTime time = QTime::currentTime();
        for (int i = 0; i < list.length(); i++)
            list[i].UpdateInfo(time);
        Unlock();
    }
public:
    TetrisInfo operator[] (int i) { return list[i]; }
    void clear() { list.clear(); }
    void append(TetrisInfo & ti) { list.append(ti); }
    int length() { return list.length(); }
    void removeAt(int index) { list.removeAt(index); }
    void Lock() { lock.lock(); }
    void Unlock() { lock.unlock(); }
};

class TetrisGrabList: public TetrisList
{
public:  
    static double grabXMin;
    static double grabXMax;
    static double grabYMin;
    static double grabYMax;
    static double grabYWarn;

    bool isTetrisValid(int i)
    {
        if (list[i].valid && list[i].y >= grabYMin && list[i].y <= grabYMax
            && list[i].x >= grabXMin && list[i].x <= grabXMax)
        {
            return true;
        }
        return false;
    }

    int GetSupremeTetrisIndex()
    {
        Lock();
        int sup_index = -1;
        double sup_rank = -1e10;
        QTime time = QTime::currentTime();
        double rank;
        for (int i = 0; i < list.length(); i++)
        {
            list[i].UpdateInfo(time);
            if (isTetrisValid(i))
            {
                rank = TetrisAI::GetSupremeRank(list[i].type);
                if (rank > sup_rank ||
                    (sup_rank - rank < 1 && list[i].y > list[sup_index].y) ||
                    list[i].y >= grabYWarn)
                {
                    sup_index = i;
                }
            }
        }
        Unlock();
        return sup_index;
    }
};

class MachineControlInfo
{
private:
    int type;
    void ApplyAngleSymmetry()
    {
        if (type == 2 || type == 3 || type == 5)
        {
            d_angle = (int)d_angle % 180;
            if (d_angle > 90) d_angle -= 180;
            else if (d_angle < -90) d_angle += 180;
        }
        else if (type == 4)
        {
            d_angle = (int)d_angle % 90;
            if (d_angle > 45) d_angle -= 90;
            else if (d_angle < -45) d_angle += 90;
        }
        else
        {
            d_angle = (int)d_angle % 360;
            if (d_angle > 180) d_angle -= 360;
            if (d_angle < -180) d_angle += 360;
        }
    }
public:
    double from_x, from_y, from_z;
    double to_x, to_y, to_z;
    double d_angle;
    MachineControlInfo(TetrisInfo &ti, bool placeOnBoard = true)
    {
        type = ti.type;
        TetrisAI ta(type);
        TetrisPosition * sup_pos = ta.GetSupremePos();
        if (nullptr == sup_pos)
        {
            throw "MachineControlInfo: Cannot place the tetris on board.";
            return;
        }
        if (placeOnBoard)
            TetrisAI::PlaceTetris(type, sup_pos);
        to_x = sup_pos->x * GRID_SIZE + PLACE_OFFSET_X + PlaceGridOffsetX[type][sup_pos->r] * GRID_SIZE;
        to_y = -sup_pos->y * GRID_SIZE + PLACE_OFFSET_Y - PlaceGridOffsetY[type][sup_pos->r] * GRID_SIZE;
        to_z = PLACE_Z;
        d_angle = -sup_pos->r * 90 + ti.angle;
        ApplyAngleSymmetry();
        QTime time = QTime::currentTime();
        ti.UpdateInfo(time);
        from_x = ti.x;
        from_y = ti.y;
        from_z = GRAB_Z;
    }
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
    TetrisGrabList grabList;
    TetrisList viewList;
    //QMutex lock;

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
