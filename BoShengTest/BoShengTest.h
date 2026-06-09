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

    void Initialize();
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
private:
    endoControl scope;
    bool bConnected = false;
    bool bIs2D = false;
    bool bFireflyOn = false;
    bool bLightOn = false;
    bool bIsSmoking = false;
    bool bVideoStart = false;

    std::pair<std::string, std::string> parseInitString(const std::string& input);
};

