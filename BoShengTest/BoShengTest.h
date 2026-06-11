#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_BoShengTest.h"

#include "endoControl.h"

class BoShengTest : public QMainWindow
{
    Q_OBJECT

public:
    BoShengTest(QWidget *parent = nullptr);
    ~BoShengTest();

private:
    Ui::BoShengTestClass ui;

private slots:
    void on3Dto2D();
    void on3DCalib();
    void onFirefly();
    void onLight();
    void onSmoking();
    void onSnap();
    void onVideo();
    void onFireflySenstive();
    void onLightLevel();
    void onZoomRate();
    void onScopeRotate(bool checked);
    void onLog();
    void onShutdown();
    void onInitialize();
    void onConnect();

    void onUpdateEndo();
    void onUpdateInst();
    void onUpdateMsg();
    void onUpdatePopMsg();

private:
    endoControl scope;
    bool bConnected = false;
    bool bFireflyOn = false;
};

