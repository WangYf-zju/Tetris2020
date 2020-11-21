#ifndef APP_H
#define APP_H

#include <QThread>
#include <QMessageBox>
#include <QList>

#include "MachineController.h"
#include "Video.h"


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
