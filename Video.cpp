#include "Video.h"
#include <stdio.h>
#include <math.h>

double TetrisGrabList::grabXMin = -190;
double TetrisGrabList::grabXMax = 190;
double TetrisGrabList::grabYMin = -180;
double TetrisGrabList::grabYMax = 180;
double TetrisGrabList::grabYWarn = 120;

Video::Video()
{
    isCameraOpen = false;
    grabList.clear();
    for (int i = 0; i < TYPE_COUNT; i++)
    {
        char path[100] = { 0 };
        sprintf_s(path, "./models/model%d.shm", i + 1);
        ReadShapeModel(path, &hv_ModelID[i]);
    }
}


Video::~Video()
{
}

void Video::run()
{
    HObject  ho_Map, ho_Image, ho_ImageMapped, ho_Region, ho_ImageReduce;
    HTuple  hv_CameraParameters, hv_CameraPose, hv_CamParamOut;

    hv_CameraParameters.Clear();
    hv_CameraParameters[0] = "area_scan_division";
    hv_CameraParameters[1] = 0.00247494;
    hv_CameraParameters[2] = -6886.65;
    hv_CameraParameters[3] = 8.29834e-006;
    hv_CameraParameters[4] = 8.3e-006;
    hv_CameraParameters[5] = 319.672;
    hv_CameraParameters[6] = 225.157;
    hv_CameraParameters[7] = 640;
    hv_CameraParameters[8] = 480;
    hv_CameraPose.Clear();
    hv_CameraPose[0] = -0.0165625;
    hv_CameraPose[1] = 0.028063;
    hv_CameraPose[2] = 0.165684;
    hv_CameraPose[3] = 359.884;
    hv_CameraPose[4] = 0.145967;
    hv_CameraPose[5] = 0.123582;
    hv_CameraPose[6] = 0;

    ChangeRadialDistortionCamPar("adaptive", hv_CameraParameters, 0, &hv_CamParamOut);
    GenRadialDistortionMap(&ho_Map, hv_CameraParameters, hv_CamParamOut, "bilinear");
    GenRectangle1(&ho_Region, 80, 0, 480, 640);

    wait(1000);
    while (isCameraOpen)
    {
        GrabImage(&ho_Image, hv_AcqHandle);
        GetImageSize(ho_Image, &hl_width, &hl_height);
        MapImage(ho_Image, ho_Map, &ho_ImageMapped);
        SetPart(hv_WindowHandle, 0, 0, hl_height, hl_width);
        
        DispObj(ho_ImageMapped, hv_WindowHandle);
        ReduceDomain(ho_ImageMapped, ho_Region, &ho_ImageReduce);
        grabList.Lock();
        Distinguish(ho_ImageReduce);
        grabList.Unlock();
        DrawCoor();
        wait(200);
    }
    CloseFramegrabber(hv_AcqHandle);
    CloseWindow(hv_WindowHandle);
}


bool Video::OpenCamera(char * index, QWidget * widget)
{
    if (isCameraOpen)
    {
        
        return false;
    }
    HWND hWnd = (HWND)widget->winId();
    OpenWindow(0, 0, 400, 300, (Hlong)hWnd, "visible", "", &hv_WindowHandle);
    try
    {
        OpenFramegrabber("DirectShow", 1, 1, 0, 0, 0, 0, "default", 8, "gray", -1, "false",
            "default", index, 0, -1, &hv_AcqHandle);
        GrabImageStart(hv_AcqHandle, -1);
    }
    catch (...)
    {
        return false;
    }
    isCameraOpen = true;
    start();
    return true;
}

void Video::CloseCamera()
{
    msleep(1000);
    isCameraOpen = false;
}

bool Video::IsCameraOpen()
{
    return isCameraOpen;
}

void Video::Distinguish(HObject & img)
{
    QTime time = QTime::currentTime();
    viewList.clear();

    HTuple ScaleMin = 0.9;
    HTuple ScaleMax = 1.25;
    HTuple MinScore[] = { 0.80,0.80,0.85,0.85,0.85,0.85,0.85 };
    HTuple NumMatches = 16;
    HTuple MaxOverlap = 0.5;
    HTuple Greediness = 0.9;

    for (int i = 0; i < TYPE_COUNT; i++)
    {
        HTuple hv_Row, hv_Column, hv_Angle, hv_Score;
        FindShapeModel(img, hv_ModelID[i], HTuple(0).TupleRad(), HTuple(360).TupleRad(),
            MinScore[i], NumMatches, MaxOverlap,
            "least_squares", (HTuple(5).Append(1)), Greediness,
            &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
        int count = hv_Row.Length();
        if (count <= 0) continue;
        if (count == 1)
        {
            TetrisInfo ti(i, hv_Row, hv_Column, hv_Angle, time);
            AddTetrisToViewList(ti);
            if (hv_Column.D() >= 400 && hv_Column.D() <= 460)
            {
                AddTetrisToGrabList(ti, time);
                DrawContour(i, hv_Row, hv_Column, hv_Angle, 1, "red");
            }
            else
            {
                DrawContour(i, hv_Row, hv_Column, hv_Angle, 1, "green");
            }
        }
        else
        {
            for (int j = 0; j < count; j++)
            {
                TetrisInfo ti(i, hv_Row[j].ToTuple(), hv_Column[j].ToTuple(),
                    hv_Angle[j].ToTuple(), time);
                AddTetrisToViewList(ti);
                if (hv_Column[j].D() >= 400 && hv_Column[j].D() <= 460)
                {
                    AddTetrisToGrabList(ti, time);
                    DrawContour(i, hv_Row[j], hv_Column[j], hv_Angle[j], 1, "red");
                }
                else
                {
                    DrawContour(i, hv_Row[j], hv_Column[j], hv_Angle[j], 1, "green");
                }
            }
        }
    }
}

void Video::DrawContour(int type, HTuple hv_Row, HTuple hv_Column, HTuple hv_Angle, 
    HTuple hv_Scale, const char * color)
{
    HTuple  hv_RefColumn, hv_HomMat2D, hv_TestImages, hv_T;
    HObject  ho_TemplateImage, ho_ModelContours, ho_TransContours;

    GetShapeModelContours(&ho_ModelContours, hv_ModelID[type], 1);

    HomMat2dIdentity(&hv_HomMat2D);
    HomMat2dScale(hv_HomMat2D, (hv_Scale), (hv_Scale),
        0, 0, &hv_HomMat2D);
    HomMat2dRotate(hv_HomMat2D, (hv_Angle), 0, 0, &hv_HomMat2D);
    HomMat2dTranslate(hv_HomMat2D, (hv_Row), (hv_Column),
        &hv_HomMat2D);
    AffineTransContourXld(ho_ModelContours, &ho_TransContours, hv_HomMat2D);

    SetColor(hv_WindowHandle, color);
    SetLineWidth(hv_WindowHandle, 5);

    DispObj(ho_TransContours, hv_WindowHandle);
}

void Video::DrawCoor()
{
    HObject HCoordinate;
    SetColor(hv_WindowHandle, "white");
    SetLineWidth(hv_WindowHandle, 10);
    GenRegionLine(&HCoordinate, HTuple(30), HTuple(315), HTuple(200), HTuple(315));
    DispObj(HCoordinate, hv_WindowHandle);
    GenRegionLine(&HCoordinate, HTuple(30), HTuple(315), HTuple(30), HTuple(500));
    DispObj(HCoordinate, hv_WindowHandle);

}


void Video::AddTetrisToGrabList(TetrisInfo & ti, QTime time)
{
    const double error = 20;
    for (int i = 0; i < grabList.length(); )
    {
        TetrisInfo * temp_ti = grabList + i;
        if (!temp_ti->UpdateInfo(time))
        {
            grabList.removeAt(i);
            continue;
        }
        if (temp_ti->type == ti.type && (fabs(temp_ti->x - ti.x) <= error &&
            fabs(temp_ti->y - ti.y) <= error))
        {
            temp_ti->x = ti.x;
            temp_ti->y = ti.y;
            temp_ti->angle = ti.angle;
            temp_ti->t = ti.t;
            return;
        }
        i++;
    }
    grabList.append(ti);
}

void Video::AddTetrisToViewList(TetrisInfo & ti)
{
    viewList.append(ti);
}

