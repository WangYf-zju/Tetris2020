#include "App.h"

App::App()
{
    isStart = false;
}

App::~App()
{
}

void App::run()
{
    emit appStart();   
    msleep(1000);
    MachineController m;
    m.OpenSerial(portname);
    bool grab = false;
    v.grabList.clear();
    m.GoToReferPoint();
    while (isStart)
    {
        int sup_index = v.grabList.GetSupremeTetrisIndex();
        if (sup_index >= 0)
        {
            v.grabList.Lock();
            TetrisInfo * sup_tetris = v.grabList + sup_index;
            MachineControlInfo mi(sup_tetris, true);
            sup_tetris->valid = false;
            m.Grab(mi.from_x, mi.from_y, mi.from_z, 
                mi.to_x, mi.to_y, mi.to_z, mi.d_angle);
            grab = true;
            v.grabList.Unlock();
        }
        if (grab)
        {
            emit grabTetris();
            msleep(2000);
            grab = false;
        }
    }
    m.CloseSerial();
    emit appStop();
}

void App::StartApp()
{
    if (!v.IsCameraOpen() || !mc.IsSerialOpen())
    {
        isStart = false;
        return;
    }
    isStart = true;
    portname = mc.SerialName();
    mc.CloseSerial();
    start();
}

void App::StopApp()
{
    isStart = false;
    msleep(1000);
}
