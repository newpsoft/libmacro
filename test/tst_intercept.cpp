#include "test/tst_intercept.h"

#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#ifdef MCR_PLATFORM_WINDOWS
	const static int endKey = 'Q';
#else
	const static int endKey = KEY_Q;
	static QByteArrayList deviceFileList;
#endif

// options
static bool onlyHearKeysFlag = true;
static bool mangleYourKeysFlag = false;
// context
static mcr::Libmacro *libmacroPt = nullptr;
static mcr::Signal *mangleSignal = nullptr;
static mcr_Key *mangleKey = nullptr;

#ifndef MCR_PLATFORM_WINDOWS
static bool exists(const std::string &filename)
{
	return !std::ifstream(filename).fail();
}
static bool setInterceptList();
#endif
static bool setInterceptList(const QCommandLineParser &parser);

/* Return value:  If blocking is enabled and return true, the signal will not
 * be received by anything else (including X11 or Wayland). */
static bool receive(struct mcr_DispatchReceiver *receiver, struct mcr_Signal *dispatchSignal, unsigned int mods);

#ifdef MCR_PLATFORM_LINUX
static void sig_handler(int)
{
	qApp->quit();
}
#endif

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	app.setOrganizationName("New Paradigm Software");
	app.setApplicationName("Libmacro Intercept Test");
	/* Remove trailing version code on some platforms */
#ifdef VERSION
	app.setApplicationVersion(MCR_STR(VERSION));
#endif
	/* Add branch name to non-master builds */
#ifdef GIT_BRANCH
	if (QString(MCR_STR(GIT_BRANCH)) != "master") {
		app.setApplicationVersion(app.applicationVersion() + "-" MCR_STR(GIT_BRANCH));
	}
#endif

	QCommandLineParser parser;
	// default: off
	QCommandLineOption allSignalsOption({"a", "all"}, app.tr("Listen to all signals, not just keyboard."));
	// default: --mangle="off"
	QCommandLineOption mangleOption({"m", "mangle"},
									app.tr("Mangle keyboard keys. Also block keyboard signals in order to reinterpret them."));
	// default: -t 10
	QCommandLineOption timeoutOption({"t", "timeout"}, app.tr("Timeout for test application (seconds)."), "10");

	/* parser ctx */
	parser.setApplicationDescription(
		app.tr("Libmacro Intercept Test, press Q to quit"));
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addOption(allSignalsOption);
	parser.addOption(mangleOption);
	parser.addOption(timeoutOption);
	parser.addPositionalArgument("devices", app.tr("Devices or input files to read from, optionally."), "[devices...]");
	parser.process(app);

	if (parser.isSet(allSignalsOption))
		onlyHearKeysFlag = false;
	if (parser.isSet(mangleOption))
		mangleYourKeysFlag = true;

#ifdef MCR_PLATFORM_LINUX
	int i;
	for (i = SIGHUP; i <= SIGTERM; i++) {
		signal(i, sig_handler);
	}
#endif

	std::cout <<
			  "Libmacro intercept sample.  To end test, press Q from intercepted keyboard, or kill with Ctrl+C."
			  << std::endl;

	libmacroPt = new mcr::Libmacro(true);
	mangleSignal = new mcr::Signal(&libmacroPt->iKey);
	/* Confusing?  This will all be simplified in the future. */
	mcr::SignalBuilder(libmacroPt).build(&mangleSignal->self).mkdata();
	mangleKey = reinterpret_cast<mcr_Key *>(mcr_Signal_data(&mangleSignal->self));

	/* Listen for signals. There will be a C++ wrapper for this. */
	if (onlyHearKeysFlag) {
		/* The following to only listen to key signals. */
		mcr::Signal siggy(&libmacroPt->iKey);
		mcr_dispatch_add(&**libmacroPt, &*siggy, nullptr, receive);
	} else {
		mcr_dispatch_add_generic(&**libmacroPt, nullptr, nullptr, receive);
	}

	/* Set to true and all signals (keyboard keys) will be blocked.
	 * Remember press Q to exit. */
	if (mangleYourKeysFlag) {
		mcr_intercept_set_blockable(&**libmacroPt, true);
	} else {
		mcr_intercept_set_blockable(&**libmacroPt, false);
	}

	/* Detect intercept devices, or set from arguments */
	setInterceptList(parser);
	/* Enable proper intercept in set list */

	/* Execute with close timeout in case of blocking errors. */
	QTimer *timer = new QTimer(&app);
	Closer *closer = new Closer(&app);
	int timeout = 10000;
	if (parser.isSet(timeoutOption)) {
		bool bOk;
		timeout = parser.value(timeoutOption).toInt(&bOk);
		if (bOk)
			timeout *= 1000;
	}
	timer->singleShot(timeout, closer, &Closer::onTimeout);
	int ret = app.exec();

	/* Will cause an error if either intercept or Libmacro is still enabled
	 * when Libmacro is deleted. */
	libmacroPt->setEnabled(false);
	/* Threads are closing */
	QThread::msleep(682);
	delete mangleSignal;
	delete libmacroPt;

	return ret;
}

static std::regex regexMouseJoy("^js|^mouse", std::regex::icase);
static std::regex regexJoy("^js", std::regex::icase);
static std::regex regexEvent("^event", std::regex::icase);
static std::regex regexHandler("^\\s*H:\\s*HANDLER[A-Z]*=", std::regex::icase);
static std::regex regexBusVirtual("^\\s*I:\\s*BUS=0*6\\s", std::regex::icase);

#ifdef MCR_PLATFORM_LINUX
static bool setInterceptList(const QByteArrayList &list)
{
	qDebug() << "Set intercept list: " << list;
	deviceFileList = list;
	std::vector<const char *> dataList;
	for(auto &iter : deviceFileList) {
		dataList.push_back(iter.constData());
	}
	if (mcr_intercept_set_grabs(&**libmacroPt, dataList.data(),
									dataList.size()))
		return false;

	if (mcr_intercept_set_enabled(&**libmacroPt, true)) {
		dmsg;
		qDebug() << "Unable to enable";
		return false;
	}
	return true;
}

static void lineHandler(const std::string &line,
						QStringList &eventSet)
{
	static bool useDeviceFlag = true;
	/* New device line, we might use this device. */
	if (line.empty()) {
		useDeviceFlag = true;
		return;
	}
	/* I: BUS=0006 is a virtual device.  Ignore virtual devices. */
	if (std::regex_search(line, regexBusVirtual)) {
		useDeviceFlag = false;
		return;
	}
	/* H: HANDLERS=, this is the goods. */
	if (useDeviceFlag && std::regex_search(line, regexHandler)) {
		QStringList handlerSet;
		/* Remove handlers=, everything else are the events */
		std::string eventLine = std::regex_replace(line, regexHandler, "");
		std::string str;
		std::istringstream ss(eventLine);
		while(ss) {
			ss >> str;
			if (!ss)
				break;
			/* Auto-detected mice and joysticks may cause instability
			 * with your whole computer. */
			if (onlyHearKeysFlag) {
				if (std::regex_search(str, regexMouseJoy))
					return;
			}
			if (!handlerSet.contains(str.c_str()))
				handlerSet.append(str.c_str());
		}
		/* Has a keyboard, push all event files to our list. */
		if (handlerSet.contains("kbd") || !onlyHearKeysFlag) {
			for (auto &iter : handlerSet) {
				if (iter.contains("event")) {
					if (!eventSet.contains(iter))
						eventSet.append(iter);
				}
			}
		}
	}
}

static bool setInterceptList()
{
	std::ifstream f(DEV_FILE);
	std::string line;
	QStringList eventSet;
	if (!f.is_open() || f.fail())
		return false;
	/* DEV_FILE file reports empty file and getline may not work? */
	while (f.peek() != EOF) {
		std::getline(f, line);
		lineHandler(line, eventSet);
	}
	if (eventSet.empty())
		return false;
	deviceFileList.clear();
	for (auto &iter : eventSet) {
		if (std::regex_search(iter.toStdString(), regexEvent)) {
			deviceFileList.append((QString(MCR_EVENT_PATH "/") + iter).toUtf8());
		}
	}
	qDebug() << "auto file set = " << deviceFileList;
	setInterceptList(deviceFileList);
	return true;
}
#endif

static bool setInterceptList(const QCommandLineParser &parser)
{
#ifdef MCR_PLATFORM_WINDOWS
	UNUSED(parser);
	if (onlyHearKeysFlag)
		mcr_intercept_mouse_set_enabled(&**libmacroPt, false);
	return true;
#else
	QStringList list = parser.positionalArguments();
	if (mcr_intercept_set_enabled(&**libmacroPt, false)) {
		dmsg;
		return false;
	}
	if (list.empty()) {
		qDebug() << "Event file devices are not set.  Using the auto-find feature.";
		return setInterceptList();
	}
	std::string repl;
	for (int i = list.size() - 1; i >= 0; i--) {
		repl = list[i].toStdString();
		if (exists(repl)) {
			deviceFileList.push_back(repl.c_str());
		} else {
			repl.insert(0, MCR_EVENT_PATH "/");
			if (exists(repl)) {
				deviceFileList.push_back(repl.c_str());
			} else {
				list.removeAt(i);
			}
		}
	}
	setInterceptList(deviceFileList);

	return true;
#endif
}

static bool receive(struct mcr_DispatchReceiver *, struct mcr_Signal * dispatchSignal,
					unsigned int mods)
{
	if (!dispatchSignal) {
		std::cerr << "Oddly we have dispatched a null signal." << std::endl;
		return false;
	}
	std::cout << "Receiving signal of type: " << mcr_ISignal_name(&**libmacroPt,
			  dispatchSignal->isignal) << std::endl;
	std::cout << "Current modifiers are " << mods << '.' << std::endl;
	/* Found a key, print values */
	if (dispatchSignal->isignal == &libmacroPt->iKey) {
		mcr_Key *keyPt = reinterpret_cast<mcr_Key *>(mcr_Signal_data(dispatchSignal));
		if (keyPt) {
			std::cout << "Key: " << keyPt->key << ":" << libmacroPt->serial.keyName(keyPt->key) << std::endl;
			std::cout << "Apply: " << keyPt->apply << ":";
			switch (keyPt->apply) {
			case MCR_SET:
				std::cout << "MCR_SET" << std::endl;
				break;
			case MCR_UNSET:
				std::cout << "MCR_UNSET" << std::endl;
				break;
			case MCR_BOTH:
				std::cout << "MCR_BOTH" << std::endl;
				break;
			case MCR_TOGGLE:
				std::cout << "MCR_TOGGLE" << std::endl;
				break;
			}
			if (keyPt->key == endKey) {
				std::cout << "Ending key has been found.  Closing program." << std::endl;
				qApp->quit();
			} else if (mangleYourKeysFlag) {
				mangleSignal->self.dispatch_flag = false;
				mangleKey->key = keyPt->key + 1;
				mangleKey->apply = keyPt->apply;
				mcr_send(&**libmacroPt, &mangleSignal->self);
			}
		} else {
			std::cout << "Oops, key instance has no data." << std::endl;
		}
		/* Do not block non-key signals */
		return mangleYourKeysFlag;
	}
	return false;
}
