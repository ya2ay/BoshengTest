#include "proxinse_interface.h"
#include "protocol.h"
#include <sstream>
#include <iostream>
#include <string>
#include <fcntl.h>


using namespace qpid::messaging;
using namespace qpid::types;
using namespace Proxinse;
using namespace Proxinse::CSR;
using namespace Proxinse::SZR;

ProxinseInterface::ProxinseInterface()
{

}

ProxinseInterface::~ProxinseInterface()
{
    if (mConnection) {
        mConnection->close();
        delete mConnection;
    }
}

bool ProxinseInterface::connect(std::string address)
{
    if (mConnection) {
        mConnection->close();
        delete mConnection;
    }

    mConnection = new Connection(address.c_str(), "");
    if (!mConnection) {
        return false;
    }

    mConnection->setOption("reconnect", true);
    mConnection->setOption("reconnect_limit", 2);

    try {
        mConnection->open();
        mSession  = mConnection->createSession();
        mSender   = mSession.createSender("csr-to-px; {create: always, node: {type: queue, durable: True}}");
        mReceiver = mSession.createReceiver("px-to-csr; {create: always}");
    }
    catch (...)
    {
        mConnection->close();
        return false;
    }

    return true;
}

void ProxinseInterface::authorize()
{
    // authorization
    sendMessage("Proxinse=India!#)^");
}

bool ProxinseInterface::sendOsdMessage(std::string name, std::string value)
{
    if (!mConnection)           return false;
    if (!mConnection->isOpen()) return false;

    std::string msg = name + "=" + value;
    try {
        mSender.send(Message(msg.c_str()), true);
    }  catch (...) {
        listLogs.push_back("error happens when sendOsdMessage1");
    }

    return true;
}

bool ProxinseInterface::sendOsdMessage(std::string name, std::list<std::string> value)
{
    if (!mConnection)           return false;
    if (!mConnection->isOpen()) return false;

    std::string msg = name;

    for ( const std::string &v : value) {
        msg += "=";
        msg += v.c_str();
    }

    try {
        mSender.send(Message(msg.c_str()), true);
    }  catch (...) {
        listLogs.push_back("error happens when sendOsdMessage2");
    }

    return true;
}

bool ProxinseInterface::sendMessage(std::string message)
{
    if (!mConnection)           return false;
    if (!mConnection->isOpen()) return false;

    try {
        mSender.send(Message(message.c_str()), true);
    } catch (...) {
        listLogs.push_back("error happens when sendMessage");
    }

    return true;
}

void ProxinseInterface::shutdown()
{
    sendMessage(CCU::shutdown);
}

void ProxinseInterface::requestVersions()
{
    sendMessage(Parameters::query_version);
}

void ProxinseInterface::pressCameraHeadButton(int index, bool longPress)
{
	std::ostringstream stream;
	stream << CCU::button << "=" << index << "=" << (longPress ? "1" : "0");
	std::string message = stream.str();

    sendMessage(message);
    listLogs.push_back(message);
}

void ProxinseInterface::queryAllParamters()
{
    sendMessage(CCU::query_parameter);
}

void ProxinseInterface::queryCameraHeadSN()
{
    sendMessage(CCU::cameara_head_SN);
}

void ProxinseInterface::changeParameter(std::string name)
{
    sendMessage(name);
}

void ProxinseInterface::changeParameter(std::string name, std::string value)
{
    std::string message = CCU::parameter + "=" + name + "=" + value;
    sendMessage(message);
}

void ProxinseInterface::uploadImage(const std::string &localFilePath, const std::string &filename)
{
    FILE* in_file;
    errno_t err = fopen_s(&in_file, localFilePath.c_str(), "rb");
	if (!err != 0) {
		std::string info =  "Error 1: Could not open file " + localFilePath;
        listLogs.push_back(info);
		return;
	}

	struct stat sb;
	if (stat(localFilePath.c_str(), &sb) == -1) {
        std::string info = "Error 2: Could not open file " + localFilePath;
        listLogs.push_back(info);
		return;
	}

	auto size = sb.st_size;
	void *data = malloc(size);
	fread(data, size, 1, in_file);

	sendImageData(filename, static_cast<char*>(data), size);

	free(data);
	fclose(in_file);
}

void ProxinseInterface::uploadImage(const char *data, int width, int height, int bpp, const std::string &filename)
{
    int len = width * height * bpp;
    if (data && len > 0 && bpp == 4) {
		std::ostringstream stream;
		stream << filename << "|" << width << "|" << height << "|" << bpp;
		std::string tmp = stream.str();
        sendImageData(tmp, data, len);
        listLogs.push_back(tmp);
    } else {
        std::string info = "uploadRawImage: invalid input";
        listLogs.push_back(info);
    }
}

void ProxinseInterface::drawImage(int id, const std::string &filename, const Rect &rect)
{
    std::list<std::string> list;
    list.push_back(std::to_string(id));
    list.push_back(filename);
    list.push_back(std::to_string(rect.left()));
    list.push_back(std::to_string(rect.top()));
    list.push_back(std::to_string(rect.right()));
    list.push_back(std::to_string(rect.bottom()));

    sendOsdMessage(OSD::image, list);
}

void ProxinseInterface::updateImage(const std::string &filename)
{
    std::list<std::string> list;
    list.push_back(filename);

    sendOsdMessage(OSD::image, list);
}

void ProxinseInterface::clearImage(int id)
{
    sendOsdMessage(OSD::image, std::to_string(id));
}

void ProxinseInterface::drawRectangle(int id, const Rect &rect, int red, int green, int blue, double alpha)
{
	std::list<std::string> list;

    list.push_back(std::to_string(id));
    list.push_back(std::to_string(rect.left()));
    list.push_back(std::to_string(rect.top()));
    list.push_back(std::to_string(rect.right()));
    list.push_back(std::to_string(rect.bottom()));
    list.push_back(std::to_string(red));
    list.push_back(std::to_string(green));
    list.push_back(std::to_string(blue));
    list.push_back(std::to_string(alpha));

    sendOsdMessage(OSD::rectangle, list);
}

void ProxinseInterface::clearRectangle(int id)
{
    sendOsdMessage(OSD::rectangle, std::to_string(id));
}

void ProxinseInterface::drawLine(int id, const Point &start, const Point &end, int lineWidth, int red, int green, int blue, double alpha)
{
	std::list<std::string> list;

    list.push_back(std::to_string(id));
    list.push_back(std::to_string(start.x()));
    list.push_back(std::to_string(start.y()));
    list.push_back(std::to_string(end.x()));
    list.push_back(std::to_string(end.y()));
    list.push_back(std::to_string(lineWidth));
    list.push_back(std::to_string(red));
    list.push_back(std::to_string(green));
    list.push_back(std::to_string(blue));
    list.push_back(std::to_string(alpha));

    sendOsdMessage(OSD::line, list);
}

void ProxinseInterface::clearLine(int id)
{
    sendOsdMessage(OSD::line, std::to_string(id));
}

void ProxinseInterface::setTextColor(int id, int red, int green, int blue, double alpha)
{
	std::list<std::string> list;

    list.push_back(std::to_string(id));
    list.push_back(std::to_string(red));
    list.push_back(std::to_string(green));
    list.push_back(std::to_string(blue));
    list.push_back(std::to_string(alpha));

    sendOsdMessage(OSD::text_color, list);
}

void ProxinseInterface::drawText(int id, const std::string &text, const Rect &rect, double fontScale)
{
	std::list<std::string> list;

    list.push_back(std::to_string(id));
    list.push_back(text);
    list.push_back(std::to_string(rect.left()));
    list.push_back(std::to_string(rect.top()));
    list.push_back(std::to_string(rect.right()));
    list.push_back(std::to_string(rect.bottom()));
    list.push_back(std::to_string(fontScale));

    sendOsdMessage(OSD::text, list);
}

void ProxinseInterface::clearText(int id)
{
    sendOsdMessage(OSD::text, std::to_string(id));
}

void ProxinseInterface::listen()
{
    if (!mConnection) return;
    if (!mConnection->isOpen()) return;
    if (!mSession.isValid()) return;
    if (!mReceiver.isValid()) return;
    if (mReceiver.isClosed()) return;

    Message message;

    try {
        while (mReceiver.fetch(message, Duration::FOREVER))
        {
            mSession.acknowledge(true);

            // there must be a way to break the loop
            if (mKeepListening == false) break;

            handleReceivedMessage(message.getContent());
         
        }
    }  catch (...) {
        std::string info = "error happens when listen";
        listLogs.push_back(info);
    }

}

std::list<std::string> split(const std::string& str, char delimiter) {
	std::list<std::string> tokens;
	std::string token;
	std::stringstream ss(str);

	while (std::getline(ss, token, delimiter)) {
		tokens.push_back(token);
	}

	return tokens;
}

//
// TODO: overrides this function to integrate with your application
//
void ProxinseInterface::handleReceivedMessage(std::string message)
{
    // execute associated function according to the received message
	std::list<std::string> list = split(message, '=');
    std::string name = list.front();
    std::string value;
	if (list.size() > 1) {
		auto it = list.begin();
		value = *++it;
	}

    if (name == CCU::heartbeat) {
        std::cout << "CCU heartbeat" << std::endl;
        mProxinseStarted = true;
    }
    else if (name == CCU::version_firmware) {
        std::string info = "Firmware Versoin: " + value;
        listLogs.push_back(info);
    }
    else if (name == CCU::version_ccu) {
        std::string info = "CCU Versoin: " + value;
        listLogs.push_back(info);
    }
    else if (name == CCU::version_ui) {
        std::string info = "UI Versoin: " + value;
        listLogs.push_back(info);
    }
    else if (name == CCU::status) {
        std::string info = "status: " + value;
        listLogs.push_back(info);
        if ("PROXINSE started" == value) {
            mProxinseStarted = true;
        }
        else if ("PROXINSE closed" == value) {
            mProxinseStarted = false;
        }
    }
    else if (name == CCU::parameter) {
        if (list.size() == 3) {
            auto it = list.begin();
            std::string parameter = *++it;
            std::string info = "parameter:" + parameter;
            std::string value = *++it;

            std::string log = "received parameter changed-" + info + "=" + value;
            listLogs.push_back(log);
            //qDebug() << "received parameter changed:" << list.at(1) << "=" << list.at(2);
        }
        else if (list.size() == 4) {
            auto it = list.begin();
            std::string parameter = *++it;
            int min = std::stoi(*++it);
            int max = std::stoi(*++it);
            std::string info = "parameter:" + parameter + " " + std::to_string(min) + " " + std::to_string(max);
            listLogs.push_back(info);
        }
        else if (list.size() > 4) {
            auto it = list.begin();
            std::string parameter = *++it;
            std::string info = "parameter:" + parameter;
            std::string paraList = "value list:";
            for (int i = 2; i < list.size(); ++i) {
                paraList += " \\ " + *++it;
            }

            listLogs.push_back(info);
            listLogs.push_back(paraList);
        }
    }
    else if (name == CCU::button_pressed) {
        std::string info = "button_pressed" + value;
        listLogs.push_back(info);
    }
    else if (name == CCU::cameara_head_SN) {
        std::string info = "camera_head_SN: " + value;
        listLogs.push_back(info);
    }
    else
        std::cout << name << std::endl;

    // TODO: handling other messages ...
}

bool ProxinseInterface::sendImageData(std::string filename, const char *data, int sizeInBytes)
{
    if (!mConnection)           return false;
    if (!mConnection->isOpen()) return false;

    try {
        Message msg;
        msg.setContentType(filename);
        msg.setContent(data, sizeInBytes);
        mSender.send(msg, true);
    } catch (...) {
        std::string info = "error happens when sendImageData";
        listLogs.push_back(info);
    }

    return true;
}


std::list<std::string> ProxinseInterface::LogInfo()
{
    return listLogs;
}

void ProxinseInterface::ClearLogInfo()
{
    listLogs.clear();
}
