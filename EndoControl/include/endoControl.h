#pragma once
#include "define.h"

#include "proxinse_interface.h"
#include "protocol.h"
#include "dataStruct.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <thread>
#include <windows.h>
#include <map>

class DLL_EXPORTS endoControl
{
public:
    endoControl();
    ~endoControl();

    bool Connect(std::string ipAddress = "192.168.1.222");

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

    std::map<std::string, std::string> GetInfo();

    //调整亮度
    //调整对比度 
    //荧光模式

    //状态查询功能
    //连接超时处理

    //OSD
    void LoadOSDImages();
    void UpdateInstStatus(InstData);
    void UpdateEndoStatus(EndoData);
    void UpdateMsg(CustomErrorInfo);
    void UpdatePopMsg(PopupInfo);

private:
    STARTUPINFO si = { sizeof(si) };         // 控制窗口等启动信息
    PROCESS_INFORMATION pi = { 0 };            // 用于接收新进程ID和句柄
    ProxinseInterface BSInterface;
    bool bConnected = false;

    std::string SlaveTypeToNumStatus(SlaveNum c);
    std::string SlaveStatusToNumStatus(InstStatus_Master m, InstStatus_Control c);
    std::string SlaveStatusToBackground(InstStatus_Control c);
    std::pair<std::string, std::string> parseInitString(const std::string& input);
    std::string EnergyStatusToString(InstStatus_EnergyStatus s);

};

