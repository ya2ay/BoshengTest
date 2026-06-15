#include "endoControl.h"
#include "UIConstants.h"

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
    TCHAR cmdLine[] = L"qpidd.exe";

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

void endoControl::onFirefly(int type)
{
    switch (type)
    {
    case 0:
        BSInterface.changeParameter(Parameters::fluo_mode, "0");
        break;
    case 1:
        BSInterface.changeParameter(Parameters::fluo_mode, "1");
        break;
    }

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
    BSInterface.uploadImage("..\\..\\Images\\Background_B.png", "Background_B");
    //Selected  : 分配给左手或右手，控制
    BSInterface.uploadImage("..\\..\\Images\\Background_S.png", "Background_S");
    //Unselected : 可切换/暂停/未被控制
    BSInterface.uploadImage("..\\..\\Images\\Background_U.png", "Background_U");
    //Alert : 异常
    BSInterface.uploadImage("..\\..\\Images\\Background_A.png", "Background_A");

    //None
    BSInterface.uploadImage("..\\..\\Images\\Arm1Num_N.png", "Arm1Num_N");
    BSInterface.uploadImage("..\\..\\Images\\Arm2Num_N.png", "Arm2Num_N");
    BSInterface.uploadImage("..\\..\\Images\\Arm3Num_N.png", "Arm3Num_N");
    BSInterface.uploadImage("..\\..\\Images\\Arm4Num_N.png", "Arm4Num_N");

    //Left hand UNCONTROLLED
    BSInterface.uploadImage("..\\..\\Images\\Arm1Num_LU.png", "Arm1Num_LU");
    BSInterface.uploadImage("..\\..\\Images\\Arm2Num_LU.png", "Arm2Num_LU");
    BSInterface.uploadImage("..\\..\\Images\\Arm3Num_LU.png", "Arm3Num_LU");
    BSInterface.uploadImage("..\\..\\Images\\Arm4Num_LU.png", "Arm4Num_LU");

    //Right hand UNCONTROLLED
    BSInterface.uploadImage("..\\..\\Images\\Arm1Num_RU.png", "Arm1Num_RU");
    BSInterface.uploadImage("..\\..\\Images\\Arm2Num_RU.png", "Arm2Num_RU");
    BSInterface.uploadImage("..\\..\\Images\\Arm3Num_RU.png", "Arm3Num_RU");
    BSInterface.uploadImage("..\\..\\Images\\Arm4Num_RU.png", "Arm4Num_RU");

    //Left hand Switchable
    BSInterface.uploadImage("..\\..\\Images\\Arm1Num_LS.png", "Arm1Num_LS");
    BSInterface.uploadImage("..\\..\\Images\\Arm2Num_LS.png", "Arm2Num_LS");
    BSInterface.uploadImage("..\\..\\Images\\Arm3Num_LS.png", "Arm3Num_LS");
    BSInterface.uploadImage("..\\..\\Images\\Arm4Num_LS.png", "Arm4Num_LS");

    //Right hand Switchable
    BSInterface.uploadImage("..\\..\\Images\\Arm1Num_RS.png", "Arm1Num_RS");
    BSInterface.uploadImage("..\\..\\Images\\Arm2Num_RS.png", "Arm2Num_RS");
    BSInterface.uploadImage("..\\..\\Images\\Arm3Num_RS.png", "Arm3Num_RS");
    BSInterface.uploadImage("..\\..\\Images\\Arm4Num_RS.png", "Arm4Num_RS");

    //Left hand Pause
    BSInterface.uploadImage("..\\..\\Images\\Arm1Num_LP.png", "Arm1Num_LP");
    BSInterface.uploadImage("..\\..\\Images\\Arm2Num_LP.png", "Arm2Num_LP");
    BSInterface.uploadImage("..\\..\\Images\\Arm3Num_LP.png", "Arm3Num_LP");
    BSInterface.uploadImage("..\\..\\Images\\Arm4Num_LP.png", "Arm4Num_LP");

    //Left hand Controlled
    BSInterface.uploadImage("..\\..\\Images\\Arm1Num_LC.png", "Arm1Num_LC");
    BSInterface.uploadImage("..\\..\\Images\\Arm2Num_LC.png", "Arm2Num_LC");
    BSInterface.uploadImage("..\\..\\Images\\Arm3Num_LC.png", "Arm3Num_LC");
    BSInterface.uploadImage("..\\..\\Images\\Arm4Num_LC.png", "Arm4Num_LC");

    //Left hand BREAKDOWN
    BSInterface.uploadImage("..\\..\\Images\\Arm1Num_LB.png", "Arm1Num_LB");
    BSInterface.uploadImage("..\\..\\Images\\Arm2Num_LB.png", "Arm2Num_LB");
    BSInterface.uploadImage("..\\..\\Images\\Arm3Num_LB.png", "Arm3Num_LB");
    BSInterface.uploadImage("..\\..\\Images\\Arm4Num_LB.png", "Arm4Num_LB");

    //Right hand Pause
    BSInterface.uploadImage("..\\..\\Images\\Arm1Num_RP.png", "Arm1Num_RP");
    BSInterface.uploadImage("..\\..\\Images\\Arm2Num_RP.png", "Arm2Num_RP");
    BSInterface.uploadImage("..\\..\\Images\\Arm3Num_RP.png", "Arm3Num_RP");
    BSInterface.uploadImage("..\\..\\Images\\Arm4Num_RP.png", "Arm4Num_RP");

    //Right hand Controlled
    BSInterface.uploadImage("..\\..\\Images\\Arm1Num_RC.png", "Arm1Num_RC");
    BSInterface.uploadImage("..\\..\\Images\\Arm2Num_RC.png", "Arm2Num_RC");
    BSInterface.uploadImage("..\\..\\Images\\Arm3Num_RC.png", "Arm3Num_RC");
    BSInterface.uploadImage("..\\..\\Images\\Arm4Num_RC.png", "Arm4Num_RC");

    //Right hand BREAKDOWN
    BSInterface.uploadImage("..\\..\\Images\\Arm1Num_RB.png", "Arm1Num_RB");
    BSInterface.uploadImage("..\\..\\Images\\Arm2Num_RB.png", "Arm2Num_RB");
    BSInterface.uploadImage("..\\..\\Images\\Arm3Num_RB.png", "Arm3Num_RB");
    BSInterface.uploadImage("..\\..\\Images\\Arm4Num_RB.png", "Arm4Num_RB");

    //None
    BSInterface.uploadImage("..\\..\\Images\\Arm1Num_N.png", "Arm1Num_N");
    BSInterface.uploadImage("..\\..\\Images\\Arm2Num_N.png", "Arm2Num_N");
    BSInterface.uploadImage("..\\..\\Images\\Arm3Num_N.png", "Arm3Num_N");
    BSInterface.uploadImage("..\\..\\Images\\Arm4Num_N.png", "Arm4Num_N");

    //Energy
    BSInterface.uploadImage("..\\..\\Images\\Energy_Cut.png", "Energy_Cut");
    BSInterface.uploadImage("..\\..\\Images\\Energy_Coag.png", "Energy_Coag");
    BSInterface.uploadImage("..\\..\\Images\\Energy_Biopolar.png", "Energy_Biopolar");
    BSInterface.uploadImage("..\\..\\Images\\Energy_Empty.png", "Energy_Empty");

    //Endo
    BSInterface.uploadImage("..\\..\\Images\\Endo_0.png", "Endo_0");
    BSInterface.uploadImage("..\\..\\Images\\Endo_30Up.png", "Endo_30Up");
    BSInterface.uploadImage("..\\..\\Images\\Endo_30Down.png", "Endo_30Down");
    //BSInterface.uploadImage("..\\..\\Images\\Endo_Empty.png", "Energy_Empty");

    //Side Energy
    BSInterface.uploadImage("..\\..\\Images\\Energy_ActiveF.png", "Energy_ActiveF");
    BSInterface.uploadImage("..\\..\\Images\\Energy_Deactive.png", "Energy_Deactive");
    BSInterface.uploadImage("..\\..\\Images\\Energy_ActiveS.png", "Energy_ActiveS");
    BSInterface.uploadImage("..\\..\\Images\\Energy_Hover.png", "Energy_Hover");

    //Top Message
    BSInterface.uploadImage("..\\..\\Images\\Message_Back.png", "Message_Back");
    BSInterface.uploadImage("..\\..\\Images\\Message_Show.png", "Message_Show");
    BSInterface.uploadImage("..\\..\\Images\\Message_Hide.png", "Message_Hide");
    BSInterface.uploadImage("..\\..\\Images\\Message_Info.png", "Message_Info");
    BSInterface.uploadImage("..\\..\\Images\\Message_Prompt.png","Message_Prompt");
    BSInterface.uploadImage("..\\..\\Images\\Message_Warning.png", "Message_Warning");
    BSInterface.uploadImage("..\\..\\Images\\Message_Fault.png", "Message_Fault");

    //Popup Message
    BSInterface.uploadImage("..\\..\\Images\\Popup_BgInfo.png",    "Popup_BgInfo");
    BSInterface.uploadImage("..\\..\\Images\\Popup_BgPrompt.png",  "Popup_BgPrompt");
    BSInterface.uploadImage("..\\..\\Images\\Popup_BgWarning.png", "Popup_BgWarning");
    BSInterface.uploadImage("..\\..\\Images\\Popup_BgFault.png",   "Popup_BgFault");

    BSInterface.uploadImage("..\\..\\Images\\Popup_InstInfo.png",    "Popup_InstInfo");
    BSInterface.uploadImage("..\\..\\Images\\Popup_InstPrompt.png",  "Popup_InstPrompt");
    BSInterface.uploadImage("..\\..\\Images\\Popup_InstWarning.png", "Popup_InstWarning");
    BSInterface.uploadImage("..\\..\\Images\\Popup_InstFault.png",   "Popup_InstFault");

    BSInterface.uploadImage("..\\..\\Images\\Popup_SlaveInfo.png",    "Popup_SlaveInfo");
    BSInterface.uploadImage("..\\..\\Images\\Popup_SlavePrompt.png",  "Popup_SlavePrompt");
    BSInterface.uploadImage("..\\..\\Images\\Popup_SlaveWarning.png", "Popup_SlaveWarning");
    BSInterface.uploadImage("..\\..\\Images\\Popup_SlaveFault.png",   "Popup_SlaveFault");
    
    BSInterface.uploadImage("..\\..\\Images\\Popup_MasterInfo.png",    "Popup_MasterInfo");
    BSInterface.uploadImage("..\\..\\Images\\Popup_MasterPrompt.png",  "Popup_MasterPrompt");
    BSInterface.uploadImage("..\\..\\Images\\Popup_MasterWarning.png", "Popup_MasterWarning");
    BSInterface.uploadImage("..\\..\\Images\\Popup_MasterFault.png",   "Popup_MasterFault");

    BSInterface.uploadImage("..\\..\\Images\\Popup_CannulaInfo.png",    "Popup_CannulaInfo");
    BSInterface.uploadImage("..\\..\\Images\\Popup_CannulaPrompt.png",  "Popup_CannulaPrompt");
    BSInterface.uploadImage("..\\..\\Images\\Popup_CannulaWarning.png", "Popup_CannulaWarning");
    BSInterface.uploadImage("..\\..\\Images\\Popup_CannulaFault.png",   "Popup_CannulaFault");

    //Blank 
    BSInterface.uploadImage("..\\..\\Images\\InstIndicator1.png", "InstIndicator1");
    BSInterface.uploadImage("..\\..\\Images\\InstIndicator2.png", "InstIndicator2");
    BSInterface.uploadImage("..\\..\\Images\\InstIndicator3.png", "InstIndicator3");
    BSInterface.uploadImage("..\\..\\Images\\InstIndicator4.png", "InstIndicator4");

    BSInterface.drawRectangle(100, Rect(1560, 0, 360, 1080),120, 100, 100, 0.8);
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
        //BSInterface.drawImage(1, msgShowHide, Rect(
        //    UIConstants::UI_MARGIN_H, 
        //    UIConstants::UI_SCREENHIGHT - UIConstants::UI_MARGIN_V - UIConstants::UI_ICONHIGHT,
        //    UIConstants::UI_ICONWIDTH, 
        //    UIConstants::UI_ICONHIGHT));
        BSInterface.drawRectangle(1, Rect(
            UIConstants::UI_MARGIN_H,
            UIConstants::UI_SCREENHIGHT - UIConstants::UI_MARGIN_V - UIConstants::UI_ICONHIGHT,
            UIConstants::UI_ICONWIDTH,
            UIConstants::UI_ICONHIGHT), 100, 50, 50, 0.5);
    }
    else
    {
        BSInterface.clearImage(1);
        BSInterface.drawRectangle(1, Rect(
            UIConstants::UI_MARGIN_H, 
            1080 - UIConstants::UI_MARGIN_V - UIConstants::UI_ICONHIGHT, 
            UIConstants::UI_ICONWIDTH, 
            UIConstants::UI_ICONHIGHT), 100, 40, 40, 0.5);

        //BSInterface.drawImage(1, msgShowHide, Rect(
        //    UIConstants::UI_MARGIN_H,
        //    1080 - UIConstants::UI_MARGIN_V - UIConstants::UI_ICONHIGHT,
        //    UIConstants::UI_ICONWIDTH,
        //    UIConstants::UI_ICONHIGHT));

        BSInterface.drawRectangle(3, Rect(
            UIConstants::UI_MARGIN_H + UIConstants::UI_ICONWIDTH,
            UIConstants::UI_SCREENHIGHT - UIConstants::UI_MARGIN_V - UIConstants::UI_ICONHIGHT,
            UIConstants::UI_SCREENWIDTH - UIConstants::UI_RECTWIDTH - (UIConstants::UI_MARGIN_H * 2 + UIConstants::UI_ICONWIDTH),
            UIConstants::UI_MSGRECTHIGHT), 80, 50, 50, 0.5);

        //BSInterface.drawImage(3, "Message_Back", Rect(
        //    UIConstants::UI_MARGIN_H + UIConstants::UI_ICONWIDTH,
        //    UIConstants::UI_SCREENHIGHT - UIConstants::UI_MARGIN_V - UIConstants::UI_ICONHIGHT,
        //    UIConstants::UI_SCREENWIDTH - UIConstants::UI_RECTWIDTH - (UIConstants::UI_MARGIN_H * 2 + UIConstants::UI_ICONWIDTH),
        //    UIConstants::UI_MSGRECTHIGHT));

        BSInterface.drawRectangle(2, Rect(
            UIConstants::UI_MARGIN_H + UIConstants::UI_ICONWIDTH + UIConstants::UI_MINIICONMARGIN_H,
            UIConstants::UI_SCREENHIGHT - UIConstants::UI_MARGIN_V - UIConstants::UI_MINIICONMARGIN_V - UIConstants::UI_MINIICONHIGHT,
            UIConstants::UI_MINIICONWIDTH,
            UIConstants::UI_MINIICONHIGHT), 80, 40, 40, 0.5);

        //BSInterface.drawImage(2, msgLevel, Rect(
        //    UIConstants::UI_MARGIN_H + UIConstants::UI_ICONWIDTH + UIConstants::UI_MINIICONMARGIN_H,
        //    UIConstants::UI_SCREENHIGHT - UIConstants::UI_MARGIN_V - UIConstants::UI_MINIICONMARGIN_V - UIConstants::UI_MINIICONHIGHT,
        //    UIConstants::UI_MINIICONWIDTH,
        //    UIConstants::UI_MINIICONHIGHT));

        BSInterface.drawText(1, info.eContent, Rect(
            UIConstants::UI_MARGIN_H + UIConstants::UI_ICONWIDTH + UIConstants::UI_MINIICONMARGIN_H * 2 + UIConstants::UI_MINIICONWIDTH,
            UIConstants::UI_SCREENHIGHT - UIConstants::UI_MARGIN_V - UIConstants::UI_MINIICONMARGIN_V - UIConstants::UI_MINIICONHIGHT,
            UIConstants::UI_SCREENWIDTH - UIConstants::UI_RECTWIDTH - (UIConstants::UI_MARGIN_H * 2 + UIConstants::UI_ICONWIDTH + UIConstants::UI_MINIICONMARGIN_H * 2 + UIConstants::UI_MINIICONWIDTH),
            UIConstants::UI_MSGHIGHT), 0.7);
    }
}

//Arm-Level Status Update
void endoControl::UpdateInstStatus(InstData data)
{
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
    else if (InstStatus_Energy::BIOPOLAR == data.energyType)
        coagName = "Energy_Biopolar";

    BSInterface.clearImage(14 + (rate - 1) * 4);
    BSInterface.clearImage(15 + (rate - 1) * 4);
    BSInterface.clearImage(16 + (rate - 1) * 4);
    BSInterface.clearImage(17 + (rate - 1) * 4);
    BSInterface.clearText(6 + (rate - 1) * 2);

    //BSInterface.drawImage(15 + (rate - 1) * 4, backgroundName,
    //    Rect((rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H,
    //        UIConstants::UI_MARGIN_V,
    //        UIConstants::UI_TABWIDTH,
    //        UIConstants::UI_TABHIGHT ));

    BSInterface.drawRectangle(15 + (rate - 1) * 4,
        Rect((rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H,
            UIConstants::UI_MARGIN_V,
            UIConstants::UI_TABWIDTH,
            UIConstants::UI_TABHIGHT), 40, 80, 40, 0.8);

    //BSInterface.drawImage(14 + (rate - 1) * 4, numName,
    //    Rect((rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H,
    //        UIConstants::UI_MARGIN_V,
    //        UIConstants::UI_NUMWIDTH,
    //        UIConstants::UI_NUMHIGHT));

    BSInterface.drawRectangle(14 + (rate - 1) * 4,
        Rect((rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H,
            UIConstants::UI_MARGIN_V,
            UIConstants::UI_NUMWIDTH,
            UIConstants::UI_NUMHIGHT), 80, 80, 40, 0.8);

    //BSInterface.drawImage(16 + (rate - 1) * 4, cutName,
    //    Rect((rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H + (UIConstants::UI_TABWIDTH - UIConstants::UI_ENERGYWIDTH - UIConstants::UI_ENERGYMARGIN_R),
    //        UIConstants::UI_MARGIN_V + UIConstants::UI_ENERGYMARGIN_V + UIConstants::UI_ENERGYHIGHT + UIConstants::UI_ENERGYSPACING_V,
    //        UIConstants::UI_ENERGYWIDTH,
    //        UIConstants::UI_ENERGYHIGHT));

    BSInterface.drawRectangle(16 + (rate - 1) * 4,
        Rect((rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H + (UIConstants::UI_TABWIDTH - UIConstants::UI_ENERGYWIDTH - UIConstants::UI_ENERGYMARGIN_R),
            UIConstants::UI_MARGIN_V + UIConstants::UI_ENERGYMARGIN_V + UIConstants::UI_ENERGYHIGHT + UIConstants::UI_ENERGYSPACING_V,
            UIConstants::UI_ENERGYWIDTH,
            UIConstants::UI_ENERGYHIGHT), 40, 80, 80, 0.8);

    //BSInterface.drawImage(17 + (rate - 1) * 4, coagName,
    //    Rect((rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H + (UIConstants::UI_TABWIDTH - UIConstants::UI_ENERGYWIDTH - UIConstants::UI_ENERGYMARGIN_R),
    //        UIConstants::UI_MARGIN_V + UIConstants::UI_ENERGYMARGIN_V,
    //        UIConstants::UI_ENERGYWIDTH,
    //        UIConstants::UI_ENERGYHIGHT));

    BSInterface.drawRectangle(17 + (rate - 1) * 4, 
        Rect((rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H + (UIConstants::UI_TABWIDTH - UIConstants::UI_ENERGYWIDTH - UIConstants::UI_ENERGYMARGIN_R),
            UIConstants::UI_MARGIN_V + UIConstants::UI_ENERGYMARGIN_V,
            UIConstants::UI_ENERGYWIDTH,
            UIConstants::UI_ENERGYHIGHT), 40, 150, 40, 0.8);

    std::string energyStatusName = EnergyStatusToString(data.energyStatus);

    if (InstStatus_Master::LEFTHAND == data.masterType) {
        BSInterface.clearImage(4);
        /*BSInterface.drawImage(4, energyStatusName, Rect(
        UIConstants::UI_MARGIN_H, 
        325, 
        UIConstants::UI_SIDEENERGYWIDTH, 
        UIConstants::UI_SIDEENERGYHIGHT));*/

        BSInterface.drawRectangle(4, Rect(
            UIConstants::UI_MARGIN_H, 
            325, 
            UIConstants::UI_SIDEENERGYWIDTH, 
            UIConstants::UI_SIDEENERGYHIGHT), 40, 150, 150, 0.2);
    }
    else if (InstStatus_Master::RIGHTHAND == data.masterType){
        BSInterface.clearImage(5);
        //BSInterface.drawImage(5, energyStatusName, Rect(
        // UIConstants::UI_SCREENWIDTH - UIConstants::UI_RECTWIDTH - UIConstants::UI_SIDEENERGYWIDTH - UIConstants::UI_MARGIN_H,
        // 325, 
        // UIConstants::UI_SIDEENERGYWIDTH, 
        // UIConstants::UI_SIDEENERGYHIGHT));

        BSInterface.drawRectangle(5, Rect(
            UIConstants::UI_SCREENWIDTH - UIConstants::UI_RECTWIDTH - UIConstants::UI_SIDEENERGYWIDTH - UIConstants::UI_MARGIN_H,
            325, 
            UIConstants::UI_SIDEENERGYWIDTH, 
            UIConstants::UI_SIDEENERGYHIGHT), 150, 150, 10, 0.2);
    }

    BSInterface.drawText(6 + (rate - 1) * 2, data.name, 
        Rect((rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H + UIConstants::UI_NUMWIDTH + UIConstants::UI_TEXTSPACING_L, 
            UIConstants::UI_MARGIN_V + UIConstants::UI_TEXTMARGIN_B + UIConstants::UI_TEXTHIGHT + UIConstants::UI_TEXTSPACING_V,
            UIConstants::UI_TEXTWIDTH,
            UIConstants::UI_TEXTHIGHT), 0.3);
}

void endoControl::UpdateEndoStatus(EndoData data)
{
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

    zoomName = std::to_string(data.zoom) + "x";
    fireflyName = data.isFireflyOn ? "Firefly On" : "Firefly Off";

    BSInterface.clearImage(14 + (rate - 1) * 4);
    BSInterface.clearImage(15 + (rate - 1) * 4);
    BSInterface.clearImage(32);
    BSInterface.clearText(14);
    BSInterface.clearText(15);

    //BSInterface.drawImage(15 + (rate - 1) * 4, backgroundName, Rect(
    //    (rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H, 
    //    UIConstants::UI_MARGIN_V, 
    //    UIConstants::UI_TABWIDTH, 
    //    UIConstants::UI_TABHIGHT ));

    BSInterface.drawRectangle(15 + (rate - 1) * 4, Rect(
        (rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H,
        UIConstants::UI_MARGIN_V,
        UIConstants::UI_TABWIDTH,
        UIConstants::UI_TABHIGHT), 22, 44, 66, 0.3);

    //BSInterface.drawImage(14 + (rate - 1) * 4, numName,Rect(
    //    (rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H, 
    //    UIConstants::UI_MARGIN_V, 
    //    UIConstants::UI_NUMWIDTH, 
    //    UIConstants::UI_NUMHIGHT));

    BSInterface.drawRectangle(14 + (rate - 1) * 4, Rect(
        (rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H,
        UIConstants::UI_MARGIN_V,
        UIConstants::UI_NUMWIDTH,
        UIConstants::UI_NUMHIGHT), 66, 22, 44, 0.3);

    //BSInterface.drawImage(32, typeName, Rect(
    //    (rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H + (UIConstants::UI_TABWIDTH - UIConstants::UI_ENERGYMARGIN_R - UIConstants::UI_ENERGYWIDTH - UIConstants::UI_ENERGYMARGIN_L - UIConstants::UI_ENDOTYPEWIDTH),
    //    UIConstants::UI_MARGIN_V
    //    UIConstants::UI_ENDOTYPEWIDTH,
    //    UIConstants::UI_ENDOTYPEHIGHT));

    BSInterface.drawRectangle(32, Rect(
        (rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H + (UIConstants::UI_TABWIDTH - UIConstants::UI_ENERGYMARGIN_R - UIConstants::UI_ENERGYWIDTH - UIConstants::UI_ENERGYMARGIN_L - UIConstants::UI_ENDOTYPEWIDTH),
        UIConstants::UI_MARGIN_V,
        UIConstants::UI_ENDOTYPEWIDTH,
        UIConstants::UI_ENDOTYPEHIGHT), 88, 44, 66, 0.3);

    BSInterface.drawText(14, fireflyName,Rect(
        (rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H + (UIConstants::UI_TABWIDTH - UIConstants::UI_ENERGYMARGIN_R - UIConstants::UI_ENERGYWIDTH),
        UIConstants::UI_MARGIN_V + UIConstants::UI_ENERGYMARGIN_V + UIConstants::UI_ENERGYHIGHT + UIConstants::UI_ENERGYSPACING_V, 
        UIConstants::UI_ENERGYWIDTH, 
        UIConstants::UI_ENERGYHIGHT),0.3);
    
    BSInterface.drawText(15, zoomName, Rect(
        (rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H + (UIConstants::UI_TABWIDTH - UIConstants::UI_ENERGYMARGIN_R - UIConstants::UI_ENERGYWIDTH),
        UIConstants::UI_MARGIN_V + UIConstants::UI_ENERGYMARGIN_V, 
        UIConstants::UI_ENERGYWIDTH, 
        UIConstants::UI_ENERGYHIGHT), 0.3);
}

void endoControl::UpdatePopMsg(PopupInfo info)
{
    int rate = info.popSlave;

    BSInterface.clearImage(6 + (rate - 1) * 2);
    BSInterface.clearImage(7 + (rate - 1) * 2);
    BSInterface.clearText(2 + (rate - 1) * 1);

    if (info.isHide)
        return;

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

    BSInterface.clearImage(6 + (rate - 1) * 2);
    BSInterface.clearImage(7 + (rate - 1) * 2);
    BSInterface.clearText(2 + (rate - 1) * 1);

    //BSInterface.drawImage(6 + (rate - 1) * 2, backgroundName,
    //    Rect((rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H,
    //        UIConstants::UI_MARGIN_V + UIConstants::UI_TABHIGHT  + UIConstants::UI_TABPOPSPACING,
    //        UIConstants::UI_POPWIDTH,
    //        UIConstants::UI_POPHIGHT));

    BSInterface.drawRectangle(6 + (rate - 1) * 2, Rect(
        (rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H,
        UIConstants::UI_MARGIN_V + UIConstants::UI_TABHIGHT + UIConstants::UI_TABPOPSPACING,
        UIConstants::UI_POPWIDTH,
        UIConstants::UI_POPHIGHT), 22, 44, 166, 0.3);

    //BSInterface.drawImage(7 + (rate - 1) * 2, iconName,
    //    Rect(rate * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H - UIConstants::UI_TABSPACING - UIConstants::UI_ICONMARGIN_H - UIConstants::UI_ICONWIDTH,
    //        UIConstants::UI_MARGIN_V + UIConstants::UI_TABHIGHT  + UIConstants::UI_TABPOPSPACING + UIConstants::UI_ICONMARGIN_V,
    //        UIConstants::UI_ICONWIDTH,
    //        UIConstants::UI_ICONHIGHT));

    BSInterface.drawRectangle(7 + (rate - 1) * 2, Rect(
        rate * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H - UIConstants::UI_TABSPACING - UIConstants::UI_ICONMARGIN_H - UIConstants::UI_ICONWIDTH,
        UIConstants::UI_MARGIN_V + UIConstants::UI_TABHIGHT + UIConstants::UI_TABPOPSPACING + UIConstants::UI_ICONMARGIN_V,
        UIConstants::UI_ICONWIDTH,
        UIConstants::UI_ICONHIGHT), 22, 144, 66, 0.3);

    BSInterface.drawText(2 + (rate - 1) * 1, info.popContent,Rect(
        (rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H + UIConstants::UI_POPMSGMARGIN_H,
        UIConstants::UI_MARGIN_V + UIConstants::UI_TABHIGHT  + UIConstants::UI_TABPOPSPACING + UIConstants::UI_ICONMARGIN_V,
        UIConstants::UI_TABWIDTH - UIConstants::UI_ICONWIDTH - UIConstants::UI_ICONMARGIN_H * 2 - UIConstants::UI_POPMSGMARGIN_H * 2,
        UIConstants::UI_POPMSGHIGHT), 0.5);
}

//Single Status Update
void endoControl::UpdateEnergyStatus(EnergyData data)
{
    std::string energyStatusName = EnergyStatusToString(data.energyStatus);

    if (InstStatus_Master::LEFTHAND == data.masterType) {
        BSInterface.clearImage(4);
        BSInterface.drawImage(4, energyStatusName, Rect(
        UIConstants::UI_MARGIN_H, 
        325, 
        UIConstants::UI_SIDEENERGYWIDTH, 
        UIConstants::UI_SIDEENERGYHIGHT));
    }
    else if (InstStatus_Master::RIGHTHAND == data.masterType) {
        BSInterface.clearImage(5);
        BSInterface.drawImage(5, energyStatusName, Rect(
         UIConstants::UI_SCREENWIDTH - UIConstants::UI_RECTWIDTH - UIConstants::UI_SIDEENERGYWIDTH - UIConstants::UI_MARGIN_H, 
         325, 
         UIConstants::UI_SIDEENERGYWIDTH, 
         UIConstants::UI_SIDEENERGYHIGHT));
    }
}

void endoControl::UpdateControlStatus(ControlData data)
{
    std::string numName = "";
    int rate = data.slaveType;
    std::string strNumber = SlaveTypeToNumStatus(data.slaveType);
    std::string strStatus = SlaveStatusToNumStatus(data.masterType, data.controlType);
    numName = strNumber + strStatus;

    BSInterface.clearImage(14 + (rate - 1) * 4);

    BSInterface.drawImage(14 + (rate - 1) * 4, numName,
        Rect((rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H,
            UIConstants::UI_MARGIN_V,
            UIConstants::UI_NUMWIDTH,
            UIConstants::UI_NUMHIGHT));
}

void endoControl::UpdateEndoZoom(ZoomData data)
{
    int rate = data.slaveIndex;

    std::string zoomName = "";
    zoomName = std::to_string(data.zoom) + "x";

    BSInterface.clearText(15);
    BSInterface.drawText(15, zoomName, Rect(
        (rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H + (UIConstants::UI_TABWIDTH - UIConstants::UI_ENERGYMARGIN_R - UIConstants::UI_ENERGYWIDTH),
        UIConstants::UI_MARGIN_V + UIConstants::UI_ENERGYMARGIN_V,
        UIConstants::UI_ENERGYWIDTH,
        UIConstants::UI_ENERGYHIGHT), 0.3);
}

void endoControl::UpdateEndoFluro(FluroData data)
{
    int rate = data.slaveIndex;

    std::string fireflyName = "";
    fireflyName = data.isFireflyOn ? "Firefly On" : "Firefly Off";

    BSInterface.clearText(14);
    BSInterface.drawText(14, fireflyName, Rect(
        (rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H + (UIConstants::UI_TABWIDTH - UIConstants::UI_ENERGYMARGIN_R - UIConstants::UI_ENERGYWIDTH),
        UIConstants::UI_MARGIN_V + UIConstants::UI_ENERGYMARGIN_V + UIConstants::UI_ENERGYHIGHT + UIConstants::UI_ENERGYSPACING_V,
        UIConstants::UI_ENERGYWIDTH,
        UIConstants::UI_ENERGYHIGHT), 0.3);
}

void endoControl::UpdateEndoType(EndoTypeData data)
{
    int rate = data.slaveIndex;
    std::string typeName = "";
    if (data.is30Degree)
    {
        if (data.isSocpeUp)
            typeName = "Endo_30Up";
        else
            typeName = "Endo_30Down";
    }
    else
        typeName = "Endo_0";

    BSInterface.clearImage(32);
    BSInterface.drawImage(32, typeName, Rect(
        (rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H + (UIConstants::UI_TABWIDTH - UIConstants::UI_ENERGYMARGIN_R - UIConstants::UI_ENERGYWIDTH - UIConstants::UI_ENERGYMARGIN_L - UIConstants::UI_ENDOTYPEWIDTH),
        UIConstants::UI_MARGIN_V,
        UIConstants::UI_ENDOTYPEWIDTH,
        UIConstants::UI_ENDOTYPEHIGHT));
}

void endoControl::UpdateIndicatorStatus(IndicatorInfo data)
{
    int index = data.slaveType;
    BSInterface.clearImage(32 + index);
    if (data.isHide)
    {
        return;
    }

    std::string indicatorName = "InstIndicator" + std::to_string(index);
    //BSInterface.drawImage(32 + index, indicatorName, Rect(
    //    (rate - 1) * (UIConstants::UI_TABWIDTH + UIConstants::UI_TABSPACING) + UIConstants::UI_MARGIN_H,
    //    UIConstants::UI_MARGIN_V,
    //    UIConstants::UI_NUMWIDTH,
    //    UIConstants::UI_NUMHIGHT));
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

    BSInterface.clearRectangle(1);
    BSInterface.clearRectangle(2);
    BSInterface.clearRectangle(3);
    BSInterface.clearRectangle(4);
    BSInterface.clearRectangle(5);
    BSInterface.clearRectangle(6);
    BSInterface.clearRectangle(7);
    BSInterface.clearRectangle(8);
    BSInterface.clearRectangle(9);
    BSInterface.clearRectangle(10);
    BSInterface.clearRectangle(11);
    BSInterface.clearRectangle(12);
    BSInterface.clearRectangle(13);
    BSInterface.clearRectangle(14);
    BSInterface.clearRectangle(15);
    BSInterface.clearRectangle(16);
    BSInterface.clearRectangle(17);
    BSInterface.clearRectangle(18);
    BSInterface.clearRectangle(19);
    BSInterface.clearRectangle(20);
    BSInterface.clearRectangle(21);
    BSInterface.clearRectangle(22);
    BSInterface.clearRectangle(23);
    BSInterface.clearRectangle(24);
    BSInterface.clearRectangle(25);
    BSInterface.clearRectangle(26);
    BSInterface.clearRectangle(27);
    BSInterface.clearRectangle(28);
    BSInterface.clearRectangle(29);
    BSInterface.clearRectangle(30);
    BSInterface.clearRectangle(31);
    BSInterface.clearRectangle(32);
    BSInterface.clearRectangle(100);

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
