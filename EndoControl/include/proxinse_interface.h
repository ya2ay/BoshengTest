#ifndef PROXINSE_INTERFACE_H
#define PROXINSE_INTERFACE_H

#include <list>
#include <qpid/messaging/Connection.h>
#include <qpid/messaging/Message.h>
#include <qpid/messaging/Sender.h>
#include <qpid/messaging/Session.h>
#include <qpid/messaging/Receiver.h>

using namespace qpid::types;
struct Point {
	int _x = 0;
	int _y = 0;

	Point(int x, int y) : _x(x), _y(y) {}

	int x() const { return _x; }
	int y() const { return _y; }
};

struct Rect {
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;

	Rect(int x_, int y_, int w_, int h_) : x(x_), y(y_), width(w_), height(h_) {}

	int left()   const { return x; }
	int top()    const { return y; }
	int right()  const { return x + width - 1; }
	int bottom() const { return y + height - 1; }
};

/**
 * @brief The ProxinseInterface class
 * interface class to communicate with PROXINSE machine
 */
class ProxinseInterface
{
public:
    ProxinseInterface();
    virtual ~ProxinseInterface();

    // connect with PROXINSE Machine
    bool connect(std::string address);
    void authorize();
    bool isProxinseStarted() const { return mProxinseStarted; }

    //
    // control operations
    //
    void shutdown();
    void requestVersions();
    void pressCameraHeadButton(int index, bool longPress);
    void changeParameter(std::string name);
    void changeParameter(std::string name, std::string value);

    void queryAllParamters();
    void queryCameraHeadSN();
    //
    //  OSD operations
    //
    void uploadImage(const std::string &localFilePath, const std::string &filename);
    void uploadImage(const char *data, int width, int height, int bpp, const std::string &filename);

    void drawImage(int id, const std::string &filename, const Rect &rect);
    void updateImage(const std::string &filename);
    void clearImage(int id);

    void drawRectangle(int id, const Rect &rect, int red, int green, int blue, double alpha);
    void clearRectangle(int id);

    void drawLine(int id, const Point &start, const Point &end, int lineWidth, int red, int green, int blue, double alpha);
    void clearLine(int id);

    void setTextColor(int id, int red, int green, int blue, double alpha);
    void drawText(int id, const std::string &text, const Rect &rect, double fontScale);
    void clearText(int id);
    std::list<std::string> LogInfo();
    void ClearLogInfo();

public:
    // listen received message from PROXINSE
    // NOTE: should call this function from an independent worker thread, OR
    //       change this class to thread class (such as inheriting from QThread)
    void listen();

public:
    void stopListen() { mKeepListening = false; }

protected:
    // override this function to handle the received in your way
    void handleReceivedMessage(std::string message);

    // send message to PROXINSE
    bool sendMessage(std::string message);

    // send parameter message to PROXINSE with name-value pair
    bool sendOsdMessage(std::string name, std::string value);
    bool sendOsdMessage(std::string name, std::list<std::string> value);

    bool sendImageData(std::string filename, const char *data, int sizeInBytes);

private:
    qpid::messaging::Connection *mConnection = nullptr;
    qpid::messaging::Session    mSession;
    qpid::messaging::Sender     mSender;
    qpid::messaging::Receiver   mReceiver;

    bool    mKeepListening = true;
    bool    mProxinseStarted = false;
    std::list<std::string> listLogs;
};


#endif // PROXINSE_INTERFACE_H
