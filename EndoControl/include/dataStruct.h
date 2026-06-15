#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <string>

enum SlaveNum {
	UNKNOW_SLAVE,
	ARM1,
	ARM2,
	ARM3,
	ARM4
};

enum InstStatus_Master {
	UNKNOW_MASTER,
	EMPTY,
	LEFTHAND,
	RIGHTHAND
};

enum InstStatus_Control {
	UNKNOWED_CONTROL,
	NONE_CONTROL,
	UNCONTROLLED,
	PAUSE,
	CONTROLLED,
	BREAKDOWN,
	SWITCHABLE
};

enum InstStatus_Energy {
	UNKNOWED_ENERGY,
	NONE_ENERGY,
	CUTABLE,
	COAGABLE,
	BIOPOLAR,
	CUTCOAG
};

enum InstStatus_EnergyStatus {
	UNKNOW_ESTATUS,
	DISCONNECTED,
	CONNECTED,
	HOVERING,
	CUTTING,
	COAGING,
	BIOPOLARING
};

struct InstPosition {
	int x;
	int y;
	int z;
};

struct EnergyData
{
	InstStatus_Master masterType;
	InstStatus_EnergyStatus energyStatus;
};

struct ControlData
{
	SlaveNum slaveType;
	InstStatus_Master masterType;
	InstStatus_Control controlType;
};

struct InstData
{
	std::string name;
	short remainTimes;
	InstStatus_Energy energyType;

	SlaveNum slaveType;
	InstStatus_Master masterType;
	InstStatus_Control controlType;

	InstStatus_EnergyStatus energyStatus;
	InstPosition position;
};

struct EndoAttitude {
	double yaw;
	double roll;
	double pitch;
};

struct ZoomData
{
	int slaveIndex;
	float zoom;
};

struct FluroData
{
	int slaveIndex;
	bool isFireflyOn;
};

struct EndoTypeData
{
	int slaveIndex;
	bool is30Degree;
	bool isSocpeUp;
};

struct EndoData
{
	std::string name;
	int slaveIndex;
	//EndoStatus_Control status;
	bool is30Degree;
	bool isSocpeUp;
	float zoom;
	bool isFireflyOn;
	EndoAttitude att;
};

enum ErrorLevel {
	UNKNOW_LEVEL,
	INFO,
	PROMPT,
	WARNING,
	FAULT
};

struct CustomErrorInfo
{
	bool isHide;
	ErrorLevel eLevel;
	std::string eContent;
};

enum PopupOwner{
	UNKNOW_OWNER,
	INST,
	SLAVE,
	MASTER,
	CANNULA
};

struct PopupInfo
{
	bool isHide;
	SlaveNum popSlave;
	ErrorLevel popLevel;
	PopupOwner popOwner;
	std::string popContent;
};

struct IndicatorInfo
{
	bool isHide;
	SlaveNum slaveType;
	InstPosition pos;
};

//enum SlaveStatus_GuideTip {
//	UNKNOWED,
//	CLUTCH,
//	GUIDE,
//	INCANNULA
//};
//
//enum MasterStatus_GuideTip {
//	UNKNOWED,
//	SQUEEZE,
//	ROTATE,
//	RELAX
//};

struct SlaveArmStatus
{
	bool isSterile;
	bool isStow;
};

#endif