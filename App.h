#ifndef APP_H
#define APP_H

#include <QThread>
#include <QMessageBox>
#include <QList>

#include "MachineController.h"
#include "Video.h"
#include "TetrisAI.h"

#define PLACE_OFFSET_X 33
#define PLACE_OFFSET_Y 20
#define PLACE_Z -398 
#define GRID_SIZE 17.7
#define GRAB_Z -430

class App: public QThread
{
    Q_OBJECT

public:
    App();
    ~App();

protected:
    void run();

public:
    MachineController mc;
    Video v;
    bool IsAppStart() { return isStart; }
    void StartApp();
    void StopApp();

private:
    bool isStart;

signals:
    void updateBoard();
};

#endif // APP_H
