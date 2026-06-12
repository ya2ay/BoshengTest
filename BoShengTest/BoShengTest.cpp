#include "BoShengTest.h"
#include <map>

BoShengTest::BoShengTest(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.btnConnect, SIGNAL(clicked()), this, SLOT(onConnect()));
    connect(ui.btnInit, SIGNAL(clicked()), this, SLOT(onInitialize()));
    connect(ui.btn2D, SIGNAL(clicked()), this, SLOT(on3Dto2D()));
    connect(ui.btn3DCalib, SIGNAL(clicked()), this, SLOT(on3DCalib()));
    connect(ui.btnFirefly, SIGNAL(clicked()), this, SLOT(onFirefly()));
    connect(ui.btnLight, SIGNAL(clicked()), this, SLOT(onLight()));
    connect(ui.btnSmoking, SIGNAL(clicked()), this, SLOT(onSmoking()));
    connect(ui.btnSnap, SIGNAL(clicked()), this, SLOT(onSnap()));
    connect(ui.btnVideo, SIGNAL(clicked()), this, SLOT(onVideo()));
    connect(ui.btnLog, SIGNAL(clicked()), this, SLOT(onLog()));
    connect(ui.btnShutdown, SIGNAL(clicked()), this, SLOT(onShutdown()));

    connect(ui.sldFirefly, &QSlider::sliderReleased, this, &BoShengTest::onFireflySenstive);
    connect(ui.sldLight, &QSlider::sliderReleased, this, &BoShengTest::onLightLevel);
    connect(ui.sldZoom, &QSlider::sliderReleased, this, &BoShengTest::onZoomRate);

    connect(ui.rdbScope, &QRadioButton::toggled, this, &BoShengTest::onScopeRotate);

    //OSD
    connect(ui.btnUpdate_Arm1, SIGNAL(clicked()), this, SLOT(onUpdateInst()));
    connect(ui.btnUpdate_Endo, SIGNAL(clicked()), this, SLOT(onUpdateEndo()));
    connect(ui.btnSendMsg, SIGNAL(clicked()), this, SLOT(onUpdateMsg()));
    connect(ui.btnSendPopMsg, SIGNAL(clicked()), this, SLOT(onUpdatePopMsg()));

    ui.sldFirefly->setDisabled(true);
    ui.sldLight->setDisabled(true);
    ui.lblStatus->setStyleSheet("background-color: rgb(200, 10, 10);");
}

BoShengTest::~BoShengTest()
{
    scope.ClearAllContent();
}

//Function
void BoShengTest::onConnect()
{
    std::string ipAddress = ui.lineEdit->text().toStdString();
    if (!ipAddress.empty())
        bConnected = scope.Connect(ipAddress);
    else
        bConnected = scope.Connect();

    if(bConnected)
        ui.lblStatus->setStyleSheet("background-color: rgb(10, 200, 10);");
    else
        MessageBox(NULL, L"内窥镜主机连接失败！", L"系统提示", MB_OK);
}

void BoShengTest::onInitialize()
{
    if (bConnected)
    {
        std::map<std::string, std::string> paraMap = scope.GetInfo();

        if (8 < paraMap.size())
        {
            if (paraMap.find("rotate_180") != paraMap.end()) {
                if (std::stoi(paraMap["rotate_180"]))
                    ui.rdbScope->setChecked(true);
                else
                    ui.rdbScope->setChecked(false);
            }

            // firefly
            if (paraMap.find("fluo_mode") != paraMap.end()) {
                if (std::stoi(paraMap["fluo_mode"]))
                {
                    ui.btnFirefly->setChecked(true);
                    ui.sldFirefly->setDisabled(false);
                }
                else
                {
                    ui.btnFirefly->setChecked(false);
                    ui.sldFirefly->setDisabled(true);
                }
            }

            if (paraMap.find("ir_sensitivity") != paraMap.end()) {
                int irValue = std::stoi(paraMap["ir_sensitivity"]);
                ui.sldFirefly->setValue(irValue);
                ui.sldFirefly->setDisabled(false);
            }

            if (paraMap.find("light") != paraMap.end()) {
                int lightValue = std::stoi(paraMap["light"]);
                if (lightValue)
                {
                    ui.btnLight->setChecked(true);
                    ui.sldLight->setValue(lightValue * 10);
                    ui.sldLight->setDisabled(false);
                }     
                else
                {
                    ui.btnLight->setChecked(false);
                    ui.sldLight->setValue(0);
                    ui.sldLight->setDisabled(true);
                }
            }

            if (paraMap.find("zoom") != paraMap.end()) {
                float mastervalue = std::stof(paraMap["zoom"]);
                float zoomValue = mastervalue * 10 / 2 - 5;
                ui.sldZoom->setValue(zoomValue * 10);
            }

            if (paraMap.find("record") != paraMap.end()) {
                if (std::stoi(paraMap["record"]))
                    ui.btnVideo->setChecked(true);
                else
                    ui.btnVideo->setChecked(false);
            }

            if (paraMap.find("dehaze") != paraMap.end()) {
                if (std::stoi(paraMap["dehaze"]))
                    ui.btnSmoking->setChecked(true);
                else
                    ui.btnSmoking->setChecked(false);
            }

            if (paraMap.find("ThreeDTo2D") != paraMap.end()) {
                if (std::stoi(paraMap["ThreeDTo2D"]))
                    ui.btn2D->setChecked(true);
                else
                    ui.btn2D->setChecked(false);
            }
        }
        else
            MessageBox(NULL, L"初始化失败，请获取日志查看！", L"系统提示", MB_OK);
    }
    else
    {
        MessageBox(NULL, L"请先连接内窥镜主机！", L"系统提示", MB_OK);
    }
}

void BoShengTest::on3Dto2D()
{
    if(ui.btn2D->isChecked())
        scope.on3Dto2D(true);
    else
        scope.on3Dto2D(false);
}

void BoShengTest::on3DCalib()
{
    scope.on3DCalib();
}

void BoShengTest::onFirefly()
{
    scope.onFirefly();
    bFireflyOn = !bFireflyOn;
    if(bFireflyOn)
        ui.sldFirefly->setDisabled(false);
    else
        ui.sldFirefly->setDisabled(true);
}

void BoShengTest::onLight()
{
    if (ui.btnLight->isChecked())
    {
        scope.onLight(true);
        ui.sldLight->setDisabled(false);
    }
    else
    {
        scope.onLight(false);
        ui.sldLight->setDisabled(true);
    }
}

void BoShengTest::onSmoking()
{
    if (ui.btnSmoking->isChecked())
        scope.onSmoking(true);
    else
        scope.onSmoking(false);
}

void BoShengTest::onSnap()
{
    scope.onSnap();
}

void BoShengTest::onVideo()
{
    if (ui.btnVideo->isChecked())
        scope.onVideo(true);
    else
        scope.onVideo(false);
}

void BoShengTest::onFireflySenstive()
{
    float sense = ui.sldFirefly->value();
    scope.onFireflySenstive((sense + 1) / 100);
}

void BoShengTest::onLightLevel()
{
    float level = ui.sldLight->value();
    scope.onLightLevel((level + 1) / 100);
}

void BoShengTest::onZoomRate()
{
    float zoom = ui.sldZoom->value();
    scope.onZoomRate((zoom + 1) / 100);
}

void BoShengTest::onScopeRotate(bool checked) 
{
    scope.onRotate180(checked);
}

void BoShengTest::onLog()
{
    std::map<std::string, std::string> paraMap = scope.GetInfo();
    auto it = paraMap.begin();
    for (int i = 0; i < paraMap.size(); ++i) {
        std::string strFirst = (*it++).first;
        std::string strSecond = (*it++).second;
        ui.txtScopeInfo->append(QString::fromStdString(strFirst + strSecond));
        it++;
    }
}

void BoShengTest::onShutdown()
{
    scope.quitQpid();
    scope.onShutdown();
}

//OSD
void BoShengTest::onUpdateInst()
{
    int armIndex = ui.cbbArm1SlaveIndex->currentText().toInt();

    InstData armData;
    switch (armIndex)
    {
         case 1: {
             armData.slaveType = SlaveNum::ARM1;
             break;
         }
         case 2: {
             armData.slaveType = SlaveNum::ARM2;
             break;
         }
         case 3: {
             armData.slaveType = SlaveNum::ARM3;
             break;
         }
         case 4: {
             armData.slaveType = SlaveNum::ARM4;
             break;
         }
         default:
             armData.slaveType = SlaveNum::UNKNOW_SLAVE;
    }

    QString arm1name = ui.edtArm1Name->text();
    armData.name = arm1name.toStdString();

    int RMTimes = ui.spbArm1Times->value();
    armData.remainTimes = RMTimes;

    if (ui.rdbArm1Empty->isChecked())
        armData.masterType = InstStatus_Master::EMPTY;
    else if(ui.rdbArm1Left->isChecked())
        armData.masterType = InstStatus_Master::LEFTHAND;
    else if (ui.rdbArm1Right->isChecked())
        armData.masterType = InstStatus_Master::RIGHTHAND;

    else
        armData.masterType = InstStatus_Master::UNKNOW_MASTER;

    if (ui.rdbArm1Uncontrol->isChecked())
        armData.controlType = InstStatus_Control::UNCONTROLLED;
    else if (ui.rdbArm1Control->isChecked())
        armData.controlType = InstStatus_Control::CONTROLLED;
    else if (ui.rdbArm1Pause->isChecked())
        armData.controlType = InstStatus_Control::PAUSE;
    else if (ui.rdbArm1Switch->isChecked())
        armData.controlType = InstStatus_Control::SWITCHABLE;
    else if (ui.rdbArm1Break->isChecked())
        armData.controlType = InstStatus_Control::BREAKDOWN;
    else
        armData.controlType = InstStatus_Control::UNKNOWED_CONTROL;

    if (ui.rdbArm1EnergyNone->isChecked()) {
        armData.energyType = InstStatus_Energy::NONE_ENERGY;
    }
    else if (ui.rdbArm1Cutable->isChecked()) {
        armData.energyType = InstStatus_Energy::CUTABLE;
    }
    else if (ui.rdbArm1Coagable->isChecked()) {
        armData.energyType = InstStatus_Energy::COAGABLE;
    }
    else if (ui.rdbArm1Biop->isChecked()) {
        armData.energyType = InstStatus_Energy::BIOPOLAR;
    }
    else if (ui.rdbArm1CCable->isChecked()) {
        armData.energyType = InstStatus_Energy::CUTCOAG;
    }
    else {
        armData.energyType = InstStatus_Energy::UNKNOWED_ENERGY;
    }

    if (ui.rdbArm1Disconn->isChecked()) {
        armData.energyStatus = InstStatus_EnergyStatus::DISCONNECTED;
    }
    else if (ui.rdbArm1Conn->isChecked()) {
        armData.energyStatus = InstStatus_EnergyStatus::CONNECTED;
    }
    else if (ui.rdbArm1Hover->isChecked()) {
        armData.energyStatus = InstStatus_EnergyStatus::HOVERING;
    }
    else if (ui.rdbArm1Cutting->isChecked()) {
        armData.energyStatus = InstStatus_EnergyStatus::CUTTING;
    }
    else if (ui.rdbArm1Coaging->isChecked()) {
        armData.energyStatus = InstStatus_EnergyStatus::COAGING;
    }
    else if (ui.rdbArm1Bioping->isChecked()) {
        armData.energyStatus = InstStatus_EnergyStatus::BIOPOLARING;
    }
    else {
        armData.energyStatus = InstStatus_EnergyStatus::UNKNOW_ESTATUS;
    }

    InstPosition pos{ ui.lndtInst1_X->text().toInt(), ui.lndtInst1_Y->text().toInt(), ui.lndtInst1_Z->text().toInt() };
    armData.position = pos;

    scope.UpdateInstStatus(armData);
}

void BoShengTest::onUpdateEndo()
{
    EndoData endoData;

    QString endoName = ui.edtEndoName->text();
    endoData.name = endoName.toStdString();

    int endoIndex = ui.cbbEndoSlaveIndex->currentText().toInt();
    endoData.slaveIndex = endoIndex;
    
    int endoDegree = ui.cbbEndoType->currentText().toInt();

    if (ui.rbdScopeUp->isChecked())
        endoData.isSocpeUp = true;
    else
        endoData.isSocpeUp = false;

    if (30 == endoDegree)
        endoData.is30Degree = true;
    else
        endoData.is30Degree = false;

    EndoAttitude att{ ui.lndtEndo_Y->text().toDouble(), ui.lndtEndo_R->text().toDouble(), ui.lndtEndo_P->text().toDouble() };
    endoData.att = att;

    scope.UpdateEndoStatus(endoData);
}

void BoShengTest::onUpdateMsg()
{
    CustomErrorInfo info;
    QString msg = ui.edtErrorContent->text();
    info.eContent = msg.toStdString();

    int level = ui.cbbErrorLevel->currentIndex();

    switch (level)
    {
    case 0: {
        info.eLevel = ErrorLevel::INFO;
        break;
    }
    case 1: {
        info.eLevel = ErrorLevel::PROMPT;
        break;
    }
    case 2: {
        info.eLevel = ErrorLevel::WARNING;
        break;
    }
    case 3: {
        info.eLevel = ErrorLevel::FAULT;
        break;
    }
    default:
        info.eLevel = ErrorLevel::UNKNOW_LEVEL;
    }

    info.isHide = ui.btnShowHideMsg->isChecked();

    scope.UpdateMsg(info);
}

void BoShengTest::onUpdatePopMsg()
{
    PopupInfo info;

    int armIndex = ui.cbbPopArm->currentText().toInt();
    switch (armIndex)
    {
    case 1: {
        info.popSlave = SlaveNum::ARM1;
        break;
    }
    case 2: {
        info.popSlave = SlaveNum::ARM2;
        break;
    }
    case 3: {
        info.popSlave = SlaveNum::ARM3;
        break;
    }
    case 4: {
        info.popSlave = SlaveNum::ARM4;
        break;
    }
    default:
        info.popSlave = SlaveNum::UNKNOW_SLAVE;
    }

    int level = ui.cbbPopErrorLevel->currentIndex();
    switch (level)
    {
    case 0: {
        info.popLevel = ErrorLevel::INFO;
        break;
    }
    case 1: {
        info.popLevel = ErrorLevel::PROMPT;
        break;
    }
    case 2: {
        info.popLevel = ErrorLevel::WARNING;
        break;
    }
    case 3: {
        info.popLevel = ErrorLevel::FAULT;
        break;
    }
    default:
        info.popLevel = ErrorLevel::UNKNOW_LEVEL;
    }

    int owner = ui.cbbPopErrorOwner->currentIndex();
    switch (owner)
    {
    case 0: {
        info.popOwner = PopupOwner::INST;
        break;
    }
    case 1: {
        info.popOwner = PopupOwner::SLAVE;
        break;
    }
    case 2: {
        info.popOwner = PopupOwner::MASTER;
        break;
    }
    case 3: {
        info.popOwner = PopupOwner::CANNULA;
        break;
    }
    default:
        info.popOwner = PopupOwner::UNKNOW_OWNER;
    }

    QString msg = ui.edtPopErrorMsg->text();
    info.popContent = msg.toStdString();

    scope.UpdatePopMsg(info);
}

