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
    TCHAR cmdLine[] = L"D:\\EndoScope\\EndoControl\\thirdpart\\qpid-cpp\\bin\\release\\qpidd.exe";

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

        BSInterface.authorize();
        BSInterface.queryAllParamters();
        LoadOSDImages();
        
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
    BSInterface.changeParameter(Parameters::fluo_mode, "1");
    //BSInterface.pressCameraHeadButton(1, false); 
}

void endoControl::onFireflyMode(int mode)
{
    switch (mode)
    {
    case 1:
        BSInterface.changeParameter(Parameters::fluo_display_mode, "1");
        break;
    case 2:
        BSInterface.changeParameter(Parameters::fluo_display_mode, "2");
        break;
    case 3:
        BSInterface.changeParameter(Parameters::fluo_display_mode, "3");
        break;
    }

    //BSInterface.pressCameraHeadButton(2, false);
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

void endoControl::setBrightness(float brightness)//0~40
{
    int realValue = brightness * 40 + 0.5;
    std::string str = std::to_string(realValue);
    BSInterface.changeParameter(Parameters::brightness, str);
}

void endoControl::setContrast(float contrast)//0~20
{
    int realValue = contrast * 20 + 0.5;
    std::string str = std::to_string(realValue);
    BSInterface.changeParameter(Parameters::contrast, str);
}

std::map<std::string, std::string> endoControl::GetInfo()
{
    std::list<std::string> content = BSInterface.LogInfo(); 
    std::map<std::string, std::string> paraMap;
    for (const auto& info : content) {
        std::pair<std::string, std::string> sspair = parseInitString(info);
        if (!sspair.first.empty() && !sspair.second.empty()) {
            paraMap.insert({ sspair.first, sspair.second });
        }
    }
    //BSInterface.ClearLogInfo();
    return paraMap;
}

std::pair<std::string, std::string> endoControl::parseInitString(const std::string& input) {
    size_t colonPos = input.find(":");
    if (colonPos == std::string::npos) return { "", "" };


    size_t equalPos = input.find('=', colonPos + 1);
    if (equalPos == std::string::npos) return { "", "" };
    std::string b = input.substr(colonPos + 1, equalPos - colonPos - 1);

    std::string c = input.substr(equalPos + 1);
    std::string d = "";

    if (0 == c.compare("false"))
        d = "0";
    else if (0 == c.compare("true"))
        d = "1";
    else
        d = c;

    return { b, d };
}

void endoControl::onShutdown()
{
    BSInterface.shutdown();
}

void endoControl::LoadOSDImages()
{
    //Blank : 未安装或未分配
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Background_B.png", "Background_B");
    //Selected  : 分配给左手或右手，控制
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Background_S.png", "Background_S");
    //Unselected : 可切换/暂停/未被控制
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Background_U.png", "Background_U");
    //Alert : 异常
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Background_A.png", "Background_A");

    //None
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm1Num_N.png", "Arm1Num_N");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm2Num_N.png", "Arm2Num_N");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm3Num_N.png", "Arm3Num_N");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm4Num_N.png", "Arm4Num_N");

    //Left hand UNCONTROLLED
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm1Num_LU.png", "Arm1Num_LU");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm2Num_LU.png", "Arm2Num_LU");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm3Num_LU.png", "Arm3Num_LU");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm4Num_LU.png", "Arm4Num_LU");

    //Right hand UNCONTROLLED
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm1Num_RU.png", "Arm1Num_RU");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm2Num_RU.png", "Arm2Num_RU");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm3Num_RU.png", "Arm3Num_RU");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm4Num_RU.png", "Arm4Num_RU");

    //Left hand Switchable
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm1Num_LS.png", "Arm1Num_LS");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm2Num_LS.png", "Arm2Num_LS");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm3Num_LS.png", "Arm3Num_LS");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm4Num_LS.png", "Arm4Num_LS");

    //Right hand Switchable
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm1Num_RS.png", "Arm1Num_RS");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm2Num_RS.png", "Arm2Num_RS");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm3Num_RS.png", "Arm3Num_RS");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm4Num_RS.png", "Arm4Num_RS");

    //Left hand Pause
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm1Num_LP.png", "Arm1Num_LP");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm2Num_LP.png", "Arm2Num_LP");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm3Num_LP.png", "Arm3Num_LP");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm4Num_LP.png", "Arm4Num_LP");

    //Left hand Controlled
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm1Num_LC.png", "Arm1Num_LC");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm2Num_LC.png", "Arm2Num_LC");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm3Num_LC.png", "Arm3Num_LC");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm4Num_LC.png", "Arm4Num_LC");

    //Left hand BREAKDOWN
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm1Num_LB.png", "Arm1Num_LB");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm2Num_LB.png", "Arm2Num_LB");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm3Num_LB.png", "Arm3Num_LB");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm4Num_LB.png", "Arm4Num_LB");

    //Right hand Pause
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm1Num_RP.png", "Arm1Num_RP");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm2Num_RP.png", "Arm2Num_RP");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm3Num_RP.png", "Arm3Num_RP");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm4Num_RP.png", "Arm4Num_RP");

    //Right hand Controlled
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm1Num_RC.png", "Arm1Num_RC");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm2Num_RC.png", "Arm2Num_RC");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm3Num_RC.png", "Arm3Num_RC");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm4Num_RC.png", "Arm4Num_RC");

    //Right hand BREAKDOWN
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm1Num_RB.png", "Arm1Num_RB");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm2Num_RB.png", "Arm2Num_RB");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm3Num_RB.png", "Arm3Num_RB");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Arm4Num_RB.png", "Arm4Num_RB");

    //Energy
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Energy_Cut.png", "Energy_Cut");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Energy_Coag.png", "Energy_Coag");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Energy_Biopolar.png", 
        "Energy_Biopolar");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Energy_Empty.png", "Energy_Empty");

    //Endo
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Endo_0.png", "Endo_0");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Endo_30Up.png", "Endo_30Up");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Endo_30Down.png", "Endo_30Down");
    //BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Endo_Empty.png", "Energy_Empty");

    //Side Energy
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Energy_ActiveF.png", 
        "Energy_ActiveF");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Energy_Deactive.png", 
        "Energy_Deactive");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Energy_ActiveS.png", 
        "Energy_ActiveS");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Energy_Hover.png", 
        "Energy_Hover");

    //Top Message
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Message_Back.png", "Message_Back");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Message_Show.png", "Message_Show");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Message_Hide.png", "Message_Hide");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Message_Info.png", "Message_Info");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Message_Prompt.png",
        "Message_Prompt");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Message_Warning.png", 
        "Message_Warning");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Message_Fault.png", 
        "Message_Fault");

    //Popup Message
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_BgInfo.png",    "Popup_BgInfo");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_BgPrompt.png",  "Popup_BgPrompt");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_BgWarning.png", "Popup_BgWarning");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_BgFault.png",   "Popup_BgFault");

    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_InstInfo.png",    "Popup_InstInfo");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_InstPrompt.png",  "Popup_InstPrompt");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_InstWarning.png", "Popup_InstWarning");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_InstFault.png",   "Popup_InstFault");

    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_SlaveInfo.png",    "Popup_SlaveInfo");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_SlavePrompt.png",  "Popup_SlavePrompt");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_SlaveWarning.png", "Popup_SlaveWarning");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_SlaveFault.png",   "Popup_SlaveFault");
    
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_MasterInfo.png",    "Popup_MasterInfo");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_MasterPrompt.png",  "Popup_MasterPrompt");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_MasterWarning.png", "Popup_MasterWarning");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_MasterFault.png",   "Popup_MasterFault");

    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_CannulaInfo.png",    "Popup_CannulaInfo");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_CannulaPrompt.png",  "Popup_CannulaPrompt");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_CannulaWarning.png", "Popup_CannulaWarning");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Popup_CannulaFault.png",   "Popup_CannulaFault");

    //Blank 
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Inst1Indicator.png", 
        "InstIndicator1");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Inst2Indicator.png", 
        "InstIndicator2");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Inst3Indicator.png", 
        "InstIndicator3");
    BSInterface.uploadImage("D:\\EndoScope\\BoshengTest\\BoShengTest\\Images\\Inst4Indicator.png", 
        "InstIndicator4");

    BSInterface.drawRectangle(1, Rect(1560, 0, 360, 1080),120, 100, 100, 1.0);
}
    
std::string endoControl::SlaveTypeToNumStatus(SlaveNum c) {
    switch (c) {
    case SlaveNum::ARM1:
        return "Arm1Num_";
    case SlaveNum::ARM2:
        return "Arm2Num_";
    case SlaveNum::ARM3:
        return "Arm3Num_";
    case SlaveNum::ARM4:
        return "Arm4Num_";
    default:
        return "";
    }
}

std::string endoControl::SlaveStatusToNumStatus(InstStatus_Master m, InstStatus_Control c) {

    std::string result = "";

    switch (m) {
    case InstStatus_Master::LEFTHAND:
        result = "L";
        break;
    case InstStatus_Master::RIGHTHAND:
        result = "R";
        break;
    case InstStatus_Master::EMPTY:
        return "N";
    }

    switch (c) {

    case InstStatus_Control::CONTROLLED:
        result += "C";
        break;
    case InstStatus_Control::BREAKDOWN:
        result += "B";
        break;
    case InstStatus_Control::SWITCHABLE:
        result += "S";
        break;
    case InstStatus_Control::UNCONTROLLED:
        result += "U";
        break;
    case InstStatus_Control::PAUSE:
        result += "P";
        break;
    case InstStatus_Control::NONE_CONTROL:
        return "N";
    }
    
    return result;
}

std::string endoControl::SlaveStatusToBackground(InstStatus_Control c) {
    switch (c) {
    case InstStatus_Control::NONE_CONTROL:
        return "B";
    case InstStatus_Control::CONTROLLED:
        return "S";
    case InstStatus_Control::BREAKDOWN:
        return "A";
    case InstStatus_Control::UNCONTROLLED:
    case InstStatus_Control::SWITCHABLE:
    case InstStatus_Control::PAUSE:
        return "U";
    }
;
    return "";
}

std::string  endoControl::EnergyStatusToString(InstStatus_EnergyStatus s)
{
    switch (s) {
    case InstStatus_EnergyStatus::DISCONNECTED:
    case InstStatus_EnergyStatus::CONNECTED:
        return "Energy_Deactive";
    case InstStatus_EnergyStatus::CUTTING:
        return "Energy_ActiveF";
    case InstStatus_EnergyStatus::HOVERING:
        return "Energy_Hover";
    case InstStatus_EnergyStatus::COAGING:
    case InstStatus_EnergyStatus::BIOPOLARING:
        return "Energy_ActiveS";
    }
    ;
    return "";
}

void endoControl::UpdateInstStatus(InstData data)
{
    int marginH = 2;
    int marginV = 2;

    int tabSpacing = 4;

    int tabWidth = 386;
    int tabHight = 80;

    int numWidth = 80;
    int numHight = 80;

    int energyWidth = 80;
    int energyHight = 36;
    int energySpacing = 4;
    int energyMarginV = 2;
    int energyRMargin = 10;

    int textWidth = 270;
    int textHight = 36;
    int textSpacing = 4;
    int textLSpacing = 16;
    int textMarginV = 2;

    std::string backgroundName = "Background_";
    std::string numName = "";
    std::string cutName = "";
    std::string coagName = "";

    std::string strBg = SlaveStatusToBackground(data.controlType);
    backgroundName += strBg;

    int rate = data.slaveType;
    std::string strNumber = SlaveTypeToNumStatus(data.slaveType);
    std::string strStatus = SlaveStatusToNumStatus(data.masterType, data.controlType);
    numName = strNumber + strStatus;

    if (InstStatus_Energy::CUTCOAG == data.energyType)
    {
        cutName = "Energy_Cut";
        coagName = "Energy_Coag";
    }
    else if (InstStatus_Energy::CUTABLE == data.energyType)
        cutName = "Energy_Cut";
    else if (InstStatus_Energy::COAGABLE == data.energyType)
        coagName = "Energy_Coag";
    else if(InstStatus_Energy::BIOPOLAR == data.energyType)
        coagName = "Energy_Biopolar";

    BSInterface.clearImage(4);
    BSInterface.clearImage(5);
    BSInterface.clearImage(14 + (rate - 1) * 4);
    BSInterface.clearImage(15 + (rate - 1) * 4);
    BSInterface.clearImage(16 + (rate - 1) * 4);
    BSInterface.clearImage(17 + (rate - 1) * 4);
    BSInterface.clearText(6 + (rate - 1) * 2);

    BSInterface.drawImage(15 + (rate - 1) * 4, backgroundName,
        Rect((rate - 1) * (tabWidth + tabSpacing) + marginH, 
            marginV, 
            tabWidth, 
            tabHight));

    BSInterface.drawImage(14 + (rate - 1) * 4, numName,
        Rect((rate - 1) * (tabWidth + tabSpacing) + marginH, 
            marginV, 
            numWidth, 
            numHight));

    BSInterface.drawImage(16 + (rate - 1) * 4, cutName,
        Rect((rate - 1) * (tabWidth + tabSpacing) + marginH + (tabWidth - energyWidth - energyRMargin),
            marginV + energyMarginV + energyHight + energySpacing, 
            energyWidth, 
            energyHight));

    BSInterface.drawImage(17 + (rate - 1) * 4, coagName,
        Rect((rate - 1) * (tabWidth + tabSpacing) + marginH + (tabWidth - energyWidth - energyRMargin),
            marginV + energyMarginV, 
            energyWidth, 
            energyHight));

    std::string energyStatusName = EnergyStatusToString(data.energyStatus);

    if(InstStatus_Master::LEFTHAND == data.masterType)
        BSInterface.drawImage(4, energyStatusName,Rect(2, 325, 20, 540));
    else if (InstStatus_Master::RIGHTHAND == data.masterType)
        BSInterface.drawImage(5, energyStatusName, Rect(1898, 325, 20, 540));

    BSInterface.drawText(6 + (rate - 1) * 2, data.name, 
        Rect((rate - 1) * (tabWidth + tabSpacing) + marginH + numWidth + textLSpacing, 
            marginV + textMarginV + textHight + textSpacing,
            textWidth,
            textHight), 0.5);
}

void endoControl::UpdateEndoStatus(EndoData data)
{
    int marginH = 2;
    int marginV = 2;

    int tabSpacing = 4;

    int tabWidth = 386;
    int tabHight = 80;

    int numWidth = 80;
    int numHight = 80;

    int energyWidth = 80;
    int energyHight = 36;
    int energySpacing = 4;
    int energyMarginV = 2;
    int energyMarginHR = 10;

    std::string backgroundName = "Background_S";
    std::string numName = "";
    std::string typeName = "";
    std::string zoomName = "";
    std::string fireflyName = "";

    int rate = data.slaveIndex;
    switch (rate) {
    case 1:
        numName = "Arm1Num_N";
        break;
    case 2:
        numName = "Arm2Num_N";
        break;
    case 3:
        numName = "Arm3Num_N";
        break;
    case 4:
        numName = "Arm4Num_N";
        break;
    default:
        numName = "";
    }

    if (data.is30Degree)
    {
        if (data.isSocpeUp)
            typeName = "Endo_30Up";
        else
            typeName = "Endo_30Down";
    }
    else
        typeName = "Endo_0";

    std::list<std::string> content = BSInterface.LogInfo();
    std::map<std::string, std::string> paraMap;
    for (const auto& info : content) {
        std::pair<std::string, std::string> sspair = parseInitString(info);
        if (!sspair.first.empty() && !sspair.second.empty()) {
            paraMap.insert({ sspair.first, sspair.second });
        }
    }

    if (paraMap.find("zoom") != paraMap.end()) {
        zoomName = paraMap["zoom"] + "x";
    }
    if (paraMap.find("fluo_mode") != paraMap.end()) {
        if (std::stoi(paraMap["fluo_mode"])){
            fireflyName = "Firefly On";
        }
        else{
            fireflyName = "Firefly Off";
        }
    }

    BSInterface.clearImage(14 + (rate - 1) * 4);
    BSInterface.clearImage(15 + (rate - 1) * 4);
    BSInterface.clearText(14);
    BSInterface.clearText(15);

    BSInterface.drawImage(15 + (rate - 1) * 4, backgroundName, Rect(
        (rate - 1) * (tabWidth + tabSpacing) + marginH, 
        marginV, 
        tabWidth, 
        tabHight));

    BSInterface.drawImage(14 + (rate - 1) * 4, numName,Rect(
        (rate - 1) * (tabWidth + tabSpacing) + marginH, 
        marginV, 
        numWidth, 
        numHight));

    BSInterface.drawText(14, fireflyName,Rect(
        (rate - 1) * (tabWidth + tabSpacing) + marginH + (tabWidth - energyMarginHR - energyWidth),
            marginV + energyMarginV + energyHight + energySpacing, 
        energyWidth, 
        energyHight),0.2);
    
    BSInterface.drawText(15, zoomName, Rect(
        (rate - 1) * (tabWidth + tabSpacing) + marginH + (tabWidth - energyMarginHR - energyWidth),
            marginV + energyMarginV, 
        energyWidth, 
        energyHight), 0.2);
}

void endoControl::UpdateMsg(CustomErrorInfo info)
{
    std::string msgShowHide = "";
    if (info.isHide)
        msgShowHide = "Message_Hide";
    else
        msgShowHide = "Message_Show";

    std::string msgLevel = "";

    switch (info.eLevel) {
    case ErrorLevel::INFO:
        msgLevel = "Message_Info";
        break;
    case ErrorLevel::PROMPT:
        msgLevel = "Message_Prompt";
        break;
    case ErrorLevel::WARNING:
        msgLevel = "Message_Warning";
        break;
    case ErrorLevel::FAULT:
        msgLevel = "Message_Fault";
        break;
    }

    if (info.isHide)
    {
        BSInterface.clearImage(1);
        BSInterface.clearImage(2);
        BSInterface.clearImage(3);
        BSInterface.clearText(1);
        BSInterface.drawImage(1, msgShowHide, Rect(2, 998, 80, 80));
    }
    else
    {
        BSInterface.clearImage(1);
        BSInterface.drawImage(1, msgShowHide, Rect(2, 998, 80, 80));
        BSInterface.drawImage(2, msgLevel, Rect(82, 1008, 60, 60));
        BSInterface.drawImage(3, "Message_Back", Rect(82, 998, 1480, 80));
        BSInterface.drawText(1, info.eContent, Rect(154, 998, 960, 80), 0.5);
    }
}

void endoControl::UpdatePopMsg(PopupInfo info)
{
    int marginH = 2;
    int marginV = 2;

    int tabSpacing = 4;
    int tabPopSpacing = 4;

    int tabWidth = 386;
    int tabHight = 80;

    int popWidth = 476;
    int popHight = 100;

    int iconWidth = 80;
    int iconHight = 80;

    int iconRMargin = 10;
    int iconBMargin = 10;

    std::string backgroundName = "Popup_Bg";
    std::string iconName = "Popup_";

    switch (info.popOwner) {
    case PopupOwner::INST:
        iconName += "Inst";
        break;
    case PopupOwner::SLAVE:
        iconName += "Slave";
        break;
    case PopupOwner::MASTER:
        iconName += "Master";
        break;
    case PopupOwner::CANNULA:
        iconName += "Cannula";
        break;
    default:
        backgroundName = "";
        iconName += "";
        return;
    }

    switch (info.popLevel) {
    case ErrorLevel::INFO:
        backgroundName += "Info";
        iconName += "Info";
        break;
    case ErrorLevel::PROMPT:
        backgroundName += "Prompt";
        iconName += "Prompt";
        break;
    case ErrorLevel::WARNING:
        backgroundName += "Warning";
        iconName += "Warning";
        break;
    case ErrorLevel::FAULT:
        backgroundName += "Fault";
        iconName += "Fault";
        break;
    default:
        backgroundName = "";
        iconName += "";
        return;
    }

    int rate = info.popSlave;

    BSInterface.clearImage(6 + (rate - 1) * 2);
    BSInterface.clearImage(7 + (rate - 1) * 2);
    BSInterface.clearText(2 + (rate - 1) * 1);

    BSInterface.drawImage(6 + (rate - 1) * 2, backgroundName,
        Rect((rate - 1) * (tabWidth + tabSpacing) + marginH,
            marginV + tabHight + tabPopSpacing,
            popWidth,
            popHight));

    BSInterface.drawImage(7 + (rate - 1) * 2, iconName,
        Rect(rate * (tabWidth + tabSpacing) + marginH - tabSpacing - iconRMargin - iconWidth,
            marginV + tabHight + tabPopSpacing + iconBMargin,
            iconWidth,
            iconHight));

    BSInterface.drawText(2 + (rate - 1) * 1, info.popContent,
        Rect((rate - 1) * (tabWidth + tabSpacing) + marginH,
            marginV + tabHight + tabPopSpacing + iconBMargin,
            popWidth,
            popHight), 0.5);
}

void endoControl::ClearAllContent()
{
    BSInterface.clearImage(1);
    BSInterface.clearImage(2);
    BSInterface.clearImage(3);
    BSInterface.clearImage(4);
    BSInterface.clearImage(5);
    BSInterface.clearImage(6);
    BSInterface.clearImage(7);
    BSInterface.clearImage(8);
    BSInterface.clearImage(9);
    BSInterface.clearImage(10);
    BSInterface.clearImage(11);
    BSInterface.clearImage(12);
    BSInterface.clearImage(13);
    BSInterface.clearImage(14);
    BSInterface.clearImage(15);
    BSInterface.clearImage(16);
    BSInterface.clearImage(17);
    BSInterface.clearImage(18);
    BSInterface.clearImage(19);
    BSInterface.clearImage(20);
    BSInterface.clearImage(21);
    BSInterface.clearImage(22);
    BSInterface.clearImage(23);
    BSInterface.clearImage(24);
    BSInterface.clearImage(25);
    BSInterface.clearImage(26);
    BSInterface.clearImage(27);
    BSInterface.clearImage(28);
    BSInterface.clearImage(29);
    BSInterface.clearImage(30);
    BSInterface.clearImage(31);
    BSInterface.clearImage(32);

    BSInterface.clearText(1);
    BSInterface.clearText(2);
    BSInterface.clearText(3);
    BSInterface.clearText(4);
    BSInterface.clearText(5);
    BSInterface.clearText(6);
    BSInterface.clearText(7);
    BSInterface.clearText(8);
    BSInterface.clearText(9);
    BSInterface.clearText(10);
    BSInterface.clearText(11);
    BSInterface.clearText(12);
    BSInterface.clearText(13);
    BSInterface.clearText(14);
    BSInterface.clearText(15);
}
