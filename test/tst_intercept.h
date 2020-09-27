#include <QtCore>

#include "mcr/libmacro.h"
#include MCR_PLATFORM_H

#ifdef MCR_PLATFORM_WINDOWS
#else
	#include <linux/input.h>
	#include <signal.h>
#endif

/*! Read from /proc/bus/input/devices
 *
 *  Exclude virtual bus 6, used for things like libmacro, "I: Bus=0006"
 *  By default only include kbd handlers
 *  js* and mouse* not included by default.  They cause problems.
 */
#ifndef DEV_FILE
	#define DEV_FILE "/proc/bus/input/devices"
#endif

class Closer : public QObject
{
	Q_OBJECT
public:
	Closer(QObject *p = 0) : QObject(p) {}
	virtual ~Closer() {};

	public slots:
	void onTimeout() { qApp->quit(); }
};
