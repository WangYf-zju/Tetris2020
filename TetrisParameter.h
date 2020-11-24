#ifndef PARAMETER_H
#define PARAMETER_H

#include <QString>
#include <QFileInfo>
#include <QMap>
#include <QSettings>

#define DEFAULT_CONVERY_SPEED 0.0450
#define DEFAULT_IMAGE_SCALEX 0.5744
#define DEFAULT_IMAGE_SCALEY 0.5625
#define DEFAULT_IMAGE_OFFSETX (-283.7)
#define DEFAULT_IMAGE_OFFSETY (-425.8+3)
#define DEFAULT_PLACE_GRIDSIZEX 17.95
#define DEFAULT_PLACE_GRIDSIZEY 17.7
#define DEFAULT_PLACE_OFFSETX 35
#define DEFAULT_PLACE_OFFSETY 20
#define DEFAULT_GRABZ -430
#define DEFAULT_PLACEZ -398
#define DEFAULT_MAX_GRABX 190.0
#define DEFAULT_MIN_GRABX -190.0
#define DEFAULT_MAX_GRABY 180.0
#define DEFAULT_MIN_GRABY -180.0
#define DEFAULT_WARN_GRABY 120

#define DEFAULT_CameraParameters0 "area_scan_division"
#define DEFAULT_CameraParameters1 0.00247494
#define DEFAULT_CameraParameters2 -6886.65
#define DEFAULT_CameraParameters3 8.29834e-006
#define DEFAULT_CameraParameters4 8.3e-006
#define DEFAULT_CameraParameters5 319.672
#define DEFAULT_CameraParameters6 225.157
#define DEFAULT_CameraParameters7 640.0
#define DEFAULT_CameraParameters8 480.0
#define DEFAULT_CameraPose0 -0.0165625
#define DEFAULT_CameraPose1 0.028063
#define DEFAULT_CameraPose2 0.165684
#define DEFAULT_CameraPose3 359.884
#define DEFAULT_CameraPose4 0.145967
#define DEFAULT_CameraPose5 0.123582
#define DEFAULT_CameraPose6 0.0

class TetrisParameter
{
public:
    static void ImportParameters(QString filename)
    {
        QFileInfo fileinfo(filename);
        if (!fileinfo.isFile())
        {
            throw "File not exist!";
            return;
        }
        QSettings * settings = new QSettings(filename, QSettings::IniFormat);
        if (settings)
        {
            CONVERY_SPEED = settings->value("Machine/CONVERY_SPEED", DEFAULT_CONVERY_SPEED).toDouble();
            IMAGE_SCALEX = settings->value("Machine/IMAGE_SCALEX", DEFAULT_IMAGE_SCALEX).toDouble();
            IMAGE_SCALEY = settings->value("Machine/IMAGE_SCALEY", DEFAULT_IMAGE_SCALEY).toDouble();
            IMAGE_OFFSETX = settings->value("Machine/IMAGE_OFFSETX", DEFAULT_IMAGE_OFFSETX).toDouble();
            IMAGE_OFFSETY = settings->value("Machine/IMAGE_OFFSETY", DEFAULT_IMAGE_OFFSETY).toDouble();
            PLACE_GRIDSIZEX = settings->value("Machine/PLACE_GRIDSIZEX", DEFAULT_PLACE_GRIDSIZEX).toDouble();
            PLACE_GRIDSIZEY = settings->value("Machine/PLACE_GRIDSIZEY", DEFAULT_PLACE_GRIDSIZEY).toDouble();
            PLACE_OFFSETX = settings->value("Machine/PLACE_OFFSETX", DEFAULT_PLACE_OFFSETX).toDouble();
            PLACE_OFFSETY = settings->value("Machine/PLACE_OFFSETY", DEFAULT_PLACE_OFFSETY).toDouble();
            MAX_GRABX = settings->value("Machine/MAX_GRABX", DEFAULT_MAX_GRABX).toDouble();
            MIN_GRABX = settings->value("Machine/MIN_GRABX", DEFAULT_MIN_GRABX).toDouble();
            MAX_GRABY = settings->value("Machine/MAX_GRABY", DEFAULT_MAX_GRABY).toDouble();
            MIN_GRABY = settings->value("Machine/MIN_GRABY", DEFAULT_MIN_GRABY).toDouble();
            WARN_GRABY = settings->value("Machine/WARN_GRABY", DEFAULT_WARN_GRABY).toDouble();
            
            CameraParameters1 = settings->value("Camera/CameraParameters1", DEFAULT_CameraParameters1).toDouble();
            CameraParameters2 = settings->value("Camera/CameraParameters2", DEFAULT_CameraParameters2).toDouble();
            CameraParameters3 = settings->value("Camera/CameraParameters3", DEFAULT_CameraParameters3).toDouble();
            CameraParameters4 = settings->value("Camera/CameraParameters4", DEFAULT_CameraParameters4).toDouble();
            CameraParameters5 = settings->value("Camera/CameraParameters5", DEFAULT_CameraParameters5).toDouble();
            CameraParameters6 = settings->value("Camera/CameraParameters6", DEFAULT_CameraParameters6).toDouble();
            CameraParameters7 = settings->value("Camera/CameraParameters7", DEFAULT_CameraParameters7).toDouble();
            CameraParameters8 = settings->value("Camera/CameraParameters8", DEFAULT_CameraParameters8).toDouble();
            CameraPose0 = settings->value("Camera/CameraPose0", DEFAULT_CameraPose0).toDouble();
            CameraPose1 = settings->value("Camera/CameraPose1", DEFAULT_CameraPose1).toDouble();
            CameraPose2 = settings->value("Camera/CameraPose2", DEFAULT_CameraPose2).toDouble();
            CameraPose3 = settings->value("Camera/CameraPose3", DEFAULT_CameraPose3).toDouble();
            CameraPose4 = settings->value("Camera/CameraPose4", DEFAULT_CameraPose4).toDouble();
            CameraPose5 = settings->value("Camera/CameraPose5", DEFAULT_CameraPose5).toDouble();
            CameraPose6 = settings->value("Camera/CameraPose6", DEFAULT_CameraPose6).toDouble();
        }
    }

    static void ExportParameters(QString filename)
    {
        QSettings * settings = new QSettings(filename, QSettings::IniFormat);
        if (settings)
        {
            settings->setValue("Machine/CONVERY_SPEED", CONVERY_SPEED);
            settings->setValue("Machine/IMAGE_SCALEX", IMAGE_SCALEX);
            settings->setValue("Machine/IMAGE_SCALEY", IMAGE_SCALEY);
            settings->setValue("Machine/IMAGE_OFFSETX", IMAGE_OFFSETX);
            settings->setValue("Machine/IMAGE_OFFSETY", IMAGE_OFFSETY);
            settings->setValue("Machine/PLACE_GRIDSIZEX", PLACE_GRIDSIZEX);
            settings->setValue("Machine/PLACE_GRIDSIZEY", PLACE_GRIDSIZEY);
            settings->setValue("Machine/PLACE_OFFSETX", PLACE_OFFSETX);
            settings->setValue("Machine/PLACE_OFFSETY", PLACE_OFFSETY);
            settings->setValue("Machine/MAX_GRABX", MAX_GRABX);
            settings->setValue("Machine/MIN_GRABX", MIN_GRABX);
            settings->setValue("Machine/MAX_GRABY", MAX_GRABY);
            settings->setValue("Machine/MIN_GRABY", MIN_GRABY);
            settings->setValue("Machine/WARN_GRABY", WARN_GRABY);

            settings->setValue("Camera/CameraParameters1", CameraParameters1);
            settings->setValue("Camera/CameraParameters2", CameraParameters2);
            settings->setValue("Camera/CameraParameters3", CameraParameters3);
            settings->setValue("Camera/CameraParameters4", CameraParameters4);
            settings->setValue("Camera/CameraParameters5", CameraParameters5);
            settings->setValue("Camera/CameraParameters6", CameraParameters6);
            settings->setValue("Camera/CameraParameters7", CameraParameters7);
            settings->setValue("Camera/CameraParameters8", CameraParameters8);
            settings->setValue("Camera/CameraPose0", CameraPose0);
            settings->setValue("Camera/CameraPose1", CameraPose1);
            settings->setValue("Camera/CameraPose2", CameraPose2);
            settings->setValue("Camera/CameraPose3", CameraPose3);
            settings->setValue("Camera/CameraPose4", CameraPose4);
            settings->setValue("Camera/CameraPose5", CameraPose5);
            settings->setValue("Camera/CameraPose6", CameraPose6);
        }
    }

    /* Machine Parameters */

    static double CONVERY_SPEED;
    static double IMAGE_SCALEX;
    static double IMAGE_SCALEY;
    static double IMAGE_OFFSETX;
    static double IMAGE_OFFSETY;
    static double PLACE_GRIDSIZEX;
    static double PLACE_GRIDSIZEY;
    static double PLACE_OFFSETX;
    static double PLACE_OFFSETY;
    static const double GRABZ;
    static const double PLACEZ;
    static double MAX_GRABX;
    static double MIN_GRABX;
    static double MAX_GRABY;
    static double MIN_GRABY;
    static double WARN_GRABY;

    /* Camera Parameters */

    static const char * CameraParameters0;
    static double CameraParameters1, CameraParameters2, CameraParameters3,
        CameraParameters4, CameraParameters5, CameraParameters6,
        CameraParameters7, CameraParameters8, CameraPose0, CameraPose1,
        CameraPose2, CameraPose3, CameraPose4, CameraPose5, CameraPose6;
};


#endif // PARAMETER_H
