#ifndef APP_H
#define APP_H

#include <QThread>
#include <QMessageBox>
#include <QList>

#include "MachineController.h"
#include "Video.h"
#include "Server.h"

class App: public QThread
{
    Q_OBJECT

public:
    App();
    ~App();

protected:
    virtual void run() override;

public:
    MachineController mc;
    Video v;
    Server s;
    bool IsAppStart() { return isStart; }
    void StartApp();
    void StopApp();

private:
    bool isStart;

signals:
    void grabTetris();
    void appStart();
    void appStop();
};

#endif // APP_H
