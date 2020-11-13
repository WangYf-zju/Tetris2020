#ifndef MACHINECONTROLLER_H
#define MACHINECONTROLLER_H

#include <stdint.h>
#include <QSerialPort>

#define MOVEZ -350

class MachineController
{
public:
    MachineController();
    ~MachineController();

public:
    bool OpenSerial(QString name);
    void CloseSerial();
    QString SerialName();
    bool IsSerialOpen();
    void SetState(double x, double y, double z, double angle, int8_t speed, bool valve);
    void Grab7Points(double *x, double *y, double *z, double *angle, int8_t *speed, bool *valve);
    void Grab(double from_x, double from_y, double from_z,
        double to_x, double to_y, double to_z, double d_angle);
    void GoToReferPoint();

private:
    QSerialPort serial;
    uint16_t CalculateCRC(uint8_t *data, uint16_t len);
    double cx, cy, cz, ca;
};

#endif // !MACHINECONTROLLER_H