#pragma once
#include "define.h"

#include "proxinse_interface.h"
#include "protocol.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <thread>
#include <windows.h>

class DLL_EXPORTS endoControl
{
public:
    endoControl();
    ~endoControl();

    bool Connect(std::string ipAddress = "192.168.1.222");
    bool Initialize();

    void on3DCalib();
    void onFirefly();
    void onFireflyMode();

    void on3Dto2D(bool type);
    void onLight(bool type);
    void onSnap();
    void onVideo(bool type);
    void onRotate180(bool type);
    void onSmoking(bool type);

    void onFireflyGain(float gain);
    void onFireflySenstive(float);
    void onLightLevel(float);
    void onZoomRate(float);

    void quitQpid();
    void onShutdown();

    std::list<std::string> GetInfo();

    //调整亮度
    //调整对比度 
    //荧光模式
    //控制消息提示可见性

private:
    STARTUPINFO si = { sizeof(si) };         // 控制窗口等启动信息
    PROCESS_INFORMATION pi = { 0 };            // 用于接收新进程ID和句柄
    ProxinseInterface BSInterface;
    bool bConnected = false;


};

