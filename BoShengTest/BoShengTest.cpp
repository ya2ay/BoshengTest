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

    //connect(ui.sldFirefly, SIGNAL(valueChanged(int)), this, SLOT(onFireflySenstive(int)));
    //connect(ui.sldLight, SIGNAL(valueChanged(int)), this, SLOT(onLightLevel(int)));
    //connect(ui.sldZoom, SIGNAL(valueChanged(int)), this, SLOT(onZoomRate(int)));

    connect(ui.sldFirefly, &QSlider::sliderReleased, this, &BoShengTest::onFireflySenstive);
    connect(ui.sldLight, &QSlider::sliderReleased, this, &BoShengTest::onLightLevel);
    connect(ui.sldZoom, &QSlider::sliderReleased, this, &BoShengTest::onZoomRate);

    connect(ui.rdbScope, &QRadioButton::toggled, this, &BoShengTest::onScopeRotate);

    ui.sldFirefly->setDisabled(true);
    ui.sldLight->setDisabled(true);
    ui.lblStatus->setStyleSheet("background-color: rgb(200, 10, 10);");
}

BoShengTest::~BoShengTest()
{

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
        std::list<std::string> scopeInfo = scope.GetInfo();
        std::map<std::string, std::string> paraMap;

        for (const auto& info : scopeInfo) {
            std::pair<std::string, std::string> sspair = parseInitString(info);
            if (!sspair.first.empty() && !sspair.second.empty()) {
                paraMap.insert({ sspair.first, sspair.second });
            }
        }

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
    std::list<std::string> scopeInfo = scope.GetInfo();
    auto it = scopeInfo.begin();
    for (int i = 0; i < scopeInfo.size(); ++i) {
        ui.txtScopeInfo->append(QString::fromStdString(*it++));
    }
}

void BoShengTest::onShutdown()
{
    scope.quitQpid();
    scope.onShutdown();
}

std::pair<std::string, std::string> BoShengTest::parseInitString(const std::string& input) {
    // 1. 找到全角冒号 '：' 的位置
    size_t colonPos = input.find(":");
    if (colonPos == std::string::npos) return { "", "" }; // 没找到冒号，返回空

    // 2. 从冒号之后开始，找到等号 '=' 的位置
    size_t equalPos = input.find('=', colonPos + 1);
    if (equalPos == std::string::npos) return { "", "" }; // 没找到等号，返回空

    // 3. 截取 b (冒号和等号之间的部分)
    std::string b = input.substr(colonPos + 1, equalPos - colonPos - 1);

    // 4. 截取 c (等号之后的所有部分)
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
//OSD
