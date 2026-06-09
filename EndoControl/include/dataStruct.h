#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <string>

enum InstStatus_Slave {
	UNKNOW,
	Arm1,
	Arm2,
	Arm3,
	Arm4
};

enum InstStatus_Master {
	UNKNOW,
	LeftHand,
	RightHand,
	Switchable
};

enum InstStatus_Control {
	UNKNOW,
	UNCONTROLLED,
	PAUSE,
	CONTROLLED,
	BREAKDOWN
};

enum InstStatus_Energy {
	UNKNOW,
	CUTABLE,
	COAGABLE,
	BIOPOLAR
};

enum InstStatus_EnergyStatus {
	UNKNOW,
	CONNECTED,
	CUTTING,
	COAGING,
	BIOPOLARING
};

struct InstPosition {
	int x;
	int y;
	int z;
};

enum ErrorLevel {
	UNKNOW,
	INFO,
	PROMPT,
	WARNING,
	FAULT
};

enum ErrorOwner{
	UNKNOW,
	INST,
	MASTER,
	SLAVE,
	CANNULA
};

struct InstAttitude {
	int yaw;
	int roll;
	int pitch;
};

//enum SlaveStatus_Sterile {
//	UNKNOW,
//	STERILE,
//	UNSTERILE
//};
//
//enum SlaveStatus_Stow {
//	UNKNOW,
//	STOW,
//	UNSTOW
//};

//enum SlaveStatus_GuideTip {
//	UNKNOW,
//	CLUTCH,
//	GUIDE,
//	INCANNULA
//};
//
//enum MasterStatus_GuideTip {
//	UNKNOW,
//	SQUEEZE,
//	ROTATE,
//	RELAX
//};

struct InstData
{
	std::string name;
	short remainTimes;
	InstStatus_Energy energyType;

	InstStatus_Slave slaveType;
	InstStatus_Master masterType;
	InstStatus_Control controlType;

	InstStatus_EnergyStatus energyStatus;
	InstPosition position;

	PopupInfo popInfo;
};

struct EndoData
{
	bool is30Degree;
	bool isSocpeUp;
	InstAttitude att;
};

struct PopupInfo
{
	ErrorLevel eLevel;
	ErrorOwner eOwner;
	std::string eContent;
};

struct CustomErrorInfo
{
	ErrorLevel eLevel;
	std::string eContent;
};

struct SlaveArmStatus
{
	bool isSterile;
	bool isStow;
};
#endif