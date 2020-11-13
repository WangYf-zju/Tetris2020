#include "App.h"


const double PlaceGridOffsetX[TYPE_COUNT][4] = {
    {1,1,1,1},{1,1,1,1},{1,0.5,1,0.5},{1,1.5,1,1.5},{0.5,0.5,0.5,0.5},{1.5,1,1.5,1},{1,1,1,1}
};
const double PlaceGridOffsetY[TYPE_COUNT][4] = {
    {1,1,1,1},{1,1,1,1},{0.5,1,0.5,1},{0.5,1,0.5,1},{0.5,0.5,0.5,0.5},{1,1.5,1,1.5},{1,1,1,1}
};

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
    QString name = mc.SerialName();
    mc.CloseSerial();
    wait(1000);
    MachineController m;
    m.OpenSerial(name);
    bool grab = false;
    v.lock.lock();
    v.grabList.clear();
    v.lock.unlock();
    m.GoToReferPoint();
    while (isStart)
    {
        QTime time = QTime::currentTime();
        int best_index = -1;
        int supremerank = -1000000;
        v.lock.lock();
        for (int i = 0; i < v.grabList.length(); i++)
        {
            v.grabList[i].UpdateInfo(time);
            if (v.grabList[i].valid && v.grabList[i].y > -180 && v.grabList[i].y < 180)
            {
                TetrisAI ta(v.grabList[i].type);
                double rank = ta.GetSupremeRank();
                if (rank > supremerank || 
                    (fabs(rank - supremerank) < 1 && v.grabList[i].y > v.grabList[best_index].y) ||
                    (v.grabList[i].y > v.grabList[best_index].y && v.grabList[i].y > 120))
                {
                    best_index = i;
                }
            }
        }
        if (best_index >= 0)
        {
            time = QTime::currentTime();
            v.grabList[best_index].UpdateInfo(time);
            int type = v.grabList[best_index].type;
            TetrisAI ta(type);
            TetrisPosition * pos = ta.GetSupremePos();
            if (pos == nullptr)
            {
                QMessageBox::warning(nullptr, "Tetris", "Cannot place tetris now!", QMessageBox::Ok);
                continue;
            }
            double to_x = pos->x * GRID_SIZE + PLACE_OFFSET_X + PlaceGridOffsetX[type][pos->r] * GRID_SIZE;
            double to_y = -pos->y * GRID_SIZE + PLACE_OFFSET_Y - PlaceGridOffsetY[type][pos->r] * GRID_SIZE;
            double d_angle = -pos->r * 90 + v.grabList[best_index].angle;
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
            m.Grab(v.grabList[best_index].x, v.grabList[best_index].y + 20, (double)GRAB_Z,
                to_x, to_y, (double)PLACE_Z, (double)d_angle);
            v.grabList[best_index].valid = false;
            TetrisAI::PlaceTetris(type, pos);
            best_index = -1;
            grab = true;
        }
        v.lock.unlock();
        if (grab)
        {
            emit updateBoard();
            msleep(2000);
            grab = false;
        }
    }
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
