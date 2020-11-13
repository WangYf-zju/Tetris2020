#include "MachineController.h"
#include <QDebug>

MachineController::MachineController()
{
}


MachineController::~MachineController()
{
}


bool MachineController::OpenSerial(QString name)
{
    if (serial.isOpen())
        serial.close();
    serial.setPortName(name);
    serial.setBaudRate(QSerialPort::Baud9600);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::EvenParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    return serial.open(QIODevice::ReadWrite);
}

void MachineController::CloseSerial()
{
    if (serial.isOpen())
        serial.close();
}

QString MachineController::SerialName()
{
    if (IsSerialOpen())
    {
        return serial.portName();
    }
    return QString("");
}

bool MachineController::IsSerialOpen()
{
    return serial.isOpen();
}

void MachineController::SetState(double x, double y, double z, 
    double angle, int8_t speed, bool valve)
{
    int16_t sx = (int16_t)x * 10;
    int16_t sy = (int16_t)y * 10;
    int16_t sz = (int16_t)z * 10;
    int16_t sa = (int16_t)angle * 10;
    QByteArray data = QByteArray::fromHex("0110000800050A");
    data.append(uchar((sx >> 8) & 0xff));
    data.append(uchar(sx & 0xff));
    data.append(uchar((sy >> 8) & 0xff));
    data.append(uchar(sy & 0xff));
    data.append(uchar((sz >> 8) & 0xff));
    data.append(uchar(sz & 0xff));
    data.append(uchar((sa >> 8) & 0xff));
    data.append(uchar(sa & 0xff));
    data.append(uchar(speed));
    if (valve) data.append(0x01);
    else data.append(char(0x00));
    uint16_t crc = CalculateCRC((uint8_t*)data.data(), data.length());
    data.append(uchar(crc & 0xff));
    data.append(uchar((crc >> 8) & 0xff));
    qDebug() << data.toHex().toUpper();
    if (IsSerialOpen())
    {
        serial.write(data);
        serial.waitForBytesWritten(1000);
        cx = x;
        cy = y;
        cz = z;
        ca = angle;
    }
}

void MachineController::Grab7Points(double * x, double * y, double * z,
    double * angle, int8_t * speed, bool * valve)
{
    QByteArray data = QByteArray::fromHex("01100064002346");
    for (int i = 0; i < 7; i++)
    {
        int16_t sx = (int16_t)x[i] * 10;
        int16_t sy = (int16_t)y[i] * 10;
        int16_t sz = (int16_t)z[i] * 10;
        int16_t sa = (int16_t)angle[i] * 10;
        data.append(uchar((sx >> 8) & 0xff));
        data.append(uchar(sx & 0xff));
        data.append(uchar((sy >> 8) & 0xff));
        data.append(uchar(sy & 0xff));
        data.append(uchar((sz >> 8) & 0xff));
        data.append(uchar(sz & 0xff));
        data.append(uchar((sa >> 8) & 0xff));
        data.append(uchar(sa & 0xff));
        data.append(uchar(speed[i]));
        if (valve[i]) data.append(0x01);
        else data.append(char(0x00));
    }
    uint16_t crc = CalculateCRC((uint8_t*)data.data(), data.length());
    data.append(uchar(crc & 0xff));
    data.append(uchar((crc >> 8) & 0xff));
    qDebug() << data.toHex().toUpper();
    if (IsSerialOpen())
    {
        serial.write(data);
        serial.waitForBytesWritten(1000);
        cx = x[6];
        cy = y[6];
        cz = z[6];
        ca = angle[6];
    }
}

void MachineController::Grab(double from_x, double from_y, double from_z, double to_x, double to_y, double to_z, double d_angle)
{
    double x[] = { from_x,from_x,from_x,to_x,to_x,to_x,to_x };
    double y[] = { from_y,from_y,from_x,to_y,to_y,to_y,to_y };
    double z[] = { (double)MOVEZ,from_z,(double)MOVEZ,(double)MOVEZ,to_z+5,to_z,to_z+10 };
    double sa = -90;
    if (d_angle < 0) sa = 90;
    double a[] = { 0,sa,sa,(sa + d_angle),(sa + d_angle),(sa + d_angle),(sa + d_angle) };
    int8_t s[] = { 1,4,1,6,8,4 };
    bool v[] = { 0,1,1,1,1,0,0 };
    Grab7Points(x, y, z, a, s, v);
}

void MachineController::GoToReferPoint()
{
    SetState(0, 0, -320, 0, 1, 0);
}


uint16_t MachineController::CalculateCRC(uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xffff;
    for (int n = 0; n < len; n++)
    {
        crc ^= data[n];
        for (int i = 0; i < 8; i++)
        {
            if ((crc & 1) == 1)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
                crc >>= 1;
        }
    }
    return crc;
}