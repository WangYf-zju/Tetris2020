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
    if (!v.IsCameraOpen() || !mc.IsSerialOpen())
    {
        isStart = false;
        return;
    }
    emit appStart();
    QString name = mc.SerialName();
    mc.CloseSerial();
    msleep(1000);
    MachineController m;
    m.OpenSerial(name);
    bool grab = false;
    v.grabList.clear();
    m.GoToReferPoint();
    while (isStart)
    {
        int sup_index = v.grabList.GetSupremeTetrisIndex();
        if (sup_index >= 0)
        {
            v.grabList.Lock();
            TetrisInfo sup_tetris = v.grabList[sup_index];
            MachineControlInfo mi(sup_tetris, true);
            m.Grab(mi.from_x, mi.from_y, mi.from_z, 
                mi.to_x, mi.to_y, mi.to_z, mi.d_angle);
            sup_tetris.valid = false;
            grab = true;
            v.grabList.Unlock();
        }
        if (grab)
        {
            emit updateBoard();
            msleep(2000);
            grab = false;
        }
    }
    emit appStop();
    m.CloseSerial();
}

void App::StartApp()
{
    isStart = true;
    start();
}

void App::StopApp()
{
    isStart = false;
    wait(1000);
}
