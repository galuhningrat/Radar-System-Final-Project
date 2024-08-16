#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>
#include <QSerialPortInfo>
#include <QDebug>
#include <QtWidgets>
#include <QtGui>
#include <QtMath>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateServo(QString command);
    void readSerial();
    void processRadarData(const QString &data);
    void on_button0_clicked();
    void on_button45_clicked();
    void on_button90_clicked();
    void on_button135_clicked();
    void on_button180_clicked();
    void on_verticalSlider_valueChanged(int value);
    void on_button_auto_clicked();
    void updateServoAuto();
    void clearOldDetectionPoints();
    void updateDetectionPoint(float angle, float distance);
    void handleLaserActivation();
    void deactivateLaser();
    void resumeOperation();
    void updateLaserStatus(const QString &status);
    void setSliderEnabled(bool enabled);
    void handleLaserStatus(const QString &status);

private:
    Ui::MainWindow *ui;
    QSerialPort *arduino;
    QString serialBuffer;

    QGraphicsScene *scene;
    QPixmap pix;
    QGraphicsItem *rect;
    float currAngle;
    const float r;
    const float angleOffset;
    float t_up;
    float t_lo;
    QPolygonF triangle;
    QGraphicsPolygonItem* needle;
    static const quint16 arduino_uno_vendorID = 9025;
    static const quint16 arduino_uno_productID = 67;
    QString radarSerial;
    bool arduino_is_available;
    QByteArray serialData;
    QString servoSetting;
    QList<QGraphicsRectItem*> detectionPoints;
    QTimer *autoTimer;
    bool laserActive;
    QTimer *laserTimer;
    QTimer *resumeTimer;
    bool autoMode;
    bool previousAutoMode;
    bool previousSliderState;
    const float MAX_DETECTION_RANGE = 200.0;
    const float LASER_ACTIVATION_MIN_RANGE = 1.0;
    const float LASER_ACTIVATION_MAX_RANGE = 50.0;

    void updateHeightDisplay(float height);
};

#endif // MAINWINDOW_H
