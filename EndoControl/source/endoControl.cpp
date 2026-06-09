#include "endoControl.h"

using namespace Proxinse;
using namespace Proxinse::CSR;
using namespace Proxinse::SZR;

endoControl::endoControl()
{

}

endoControl::~endoControl()
{

}

bool endoControl::Connect(std::string ipAddress)
{
    TCHAR cmdLine[] = L"D:\\EndoControl\\thirdpart\\qpid-cpp\\bin\\release\\qpidd.exe";

    // 2. 调用API创建进程
    BOOL bSuccess = CreateProcess(
        NULL,                // 应用程序名（为NULL时从命令行提取）
        cmdLine,             // 命令行字符串（必须是可修改的）
        NULL,                // 进程安全属性
        NULL,                // 线程安全属性
        FALSE,               // 是否继承句柄
        CREATE_NO_WINDOW,    // 创建标志（例如不显示窗口，可根据需要调整）
        NULL,                // 环境变量
        NULL,                // 当前工作目录
        &si,                 // 启动信息
        &pi                  // [输出] 用于接收新进程信息的结构体
    );

    // 3. 检查进程启动状态
    if (!bSuccess) {
        std::cerr << "CreateProcess失败，错误码: " << GetLastError() << std::endl;
        return 0;
    }
    else
    {
        std::cout << "外部程序已启动 (PID: " << pi.dwProcessId << ")" << std::endl;
        int retry = 3;

        do {
            std::cout << "Connecting to " << ipAddress << std::endl;
            bConnected = BSInterface.connect(ipAddress);
        } while (!bConnected && retry--);

        if(!bConnected)
            return 0;

        std::thread listener(&ProxinseInterface::listen, &BSInterface);
        listener.detach();

        // wait for PROXINSE started
        //while (BSInterface.isProxinseStarted() == false) 
        //{
        //   msleep(1);
        //	std::cout << "waiting for PROXINSE started";
        //}
        //std::cout << "PROXINSE is started, Ready to Go!" << std::endl;

        BSInterface.authorize();
        BSInterface.queryAllParamters();
        
        return 1;
    }
}

void endoControl::quitQpid()
{
    // 4. 在需要控制进程时（例如：强制结束、检查状态）
     if (TerminateProcess(pi.hProcess, 1)) {
         std::cout << "外部程序已强制结束" << std::endl;
     }
    
    // ⭐ 检查进程是否仍在运行，以及获取退出码
    DWORD exitCode = 0;
    if (GetExitCodeProcess(pi.hProcess, &exitCode)) {
        if (exitCode == STILL_ACTIVE) {
            std::cout << "进程仍在运行中..." << std::endl;
        }
        else {
            std::cout << "进程已结束，退出代码: " << exitCode << std::endl;
        }
    }
    else {
        std::cerr << "获取进程退出代码失败，错误码: " << GetLastError() << std::endl;
    }
    
    // 5. 清理资源：关闭进程和主线程的句柄
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}



//未开放API，通过按钮实现
void endoControl::on3DCalib()
{
    BSInterface.pressCameraHeadButton(0, false);
}

void endoControl::onFirefly()
{
    BSInterface.pressCameraHeadButton(1, false); 
}

void endoControl::onFireflyMode()
{
    BSInterface.pressCameraHeadButton(2, false);
}

//开放API
void endoControl::onSnap()
{
    BSInterface.changeParameter(Parameters::screen_shot);
}

void endoControl::on3Dto2D(bool type)
{
    if (type)
        BSInterface.changeParameter(Parameters::ThreeDTo2D, "1");
    else
        BSInterface.changeParameter(Parameters::ThreeDTo2D, "0");
}

void endoControl::onLight(bool type)
{
    if (type)
        BSInterface.changeParameter(Parameters::light, "5");
    else
        BSInterface.changeParameter(Parameters::light, "0");
}

void endoControl::onVideo(bool type)
{
    if (type)
        BSInterface.changeParameter(Parameters::record, "1");
    else
        BSInterface.changeParameter(Parameters::record, "0");
}

void endoControl::onRotate180(bool type)
{
    if (type)
        BSInterface.changeParameter(Parameters::rotate_180, "1");
    else
        BSInterface.changeParameter(Parameters::rotate_180, "0");
}

void endoControl::onSmoking(bool type)
{
    if (type)
        BSInterface.changeParameter(Parameters::dehaze, "1");
    else
        BSInterface.changeParameter(Parameters::dehaze, "0");
}

void endoControl::onFireflyGain(float gain)
{
    float realValue = gain * 10;
    std::string str = std::to_string(realValue);
    BSInterface.changeParameter(Parameters::ir_gain, str);
}

void endoControl::onFireflySenstive(float Sense)
{
    int realValue = Sense * 100 - 1;
    std::string str = std::to_string(realValue);
    BSInterface.changeParameter(Parameters::ir_sensitivity, str);
}

void endoControl::onLightLevel(float Level)
{
    int realValue = Level * 10 + 0.5;
    std::string str = std::to_string(realValue);
    BSInterface.changeParameter(Parameters::light, str);
}

void endoControl::onZoomRate(float Zoom)
{
    int value = Zoom * 10 + 0.5 + 5;
    float realValue = value * 2 / 10.0;
    std::string str = std::to_string(realValue);
    size_t dot_pos = str.find('.');
    if (dot_pos != std::string::npos) {
        str = str.substr(0, dot_pos + 2);  // 保留小数点后1位
    }

    BSInterface.changeParameter(Parameters::zoom, str);
}

std::list<std::string> endoControl::GetInfo()
{
    std::list<std::string> content = BSInterface.LogInfo();
    //BSInterface.ClearLogInfo();
    return content;
}

void endoControl::onShutdown()
{
    BSInterface.shutdown();
}
