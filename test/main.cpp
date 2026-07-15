/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
  SPDX-License-Identifier: LGPL-2.1-only */

#include <qglobal.h>
#ifndef TEST_INDIVIDUAL

#include <QtCore>
#include <QtTest/QtTest>

#include <algorithm>

namespace mcr::test
{
QObjectList &testObjects()
{
	static QObjectList allTestObjects;
	return allTestObjects;
}
struct qobj_name_compare {
	bool operator()(QObject *lhsPtr, QObject *rhsPtr)
	{
		auto lName = lhsPtr->objectName();
		auto rName = rhsPtr->objectName();
		return std::lexicographical_compare(lName.cbegin(),
						    lName.cend(),
						    rName.cbegin(),
						    rName.cend());
	}
};

void addTest(QObject *obj)
{
	if (!obj)
		return;
	qInfo() << "Adding test suite:" << obj->objectName();
	auto &t = testObjects();
	auto found = std::lower_bound(t.cbegin(), t.cend(), obj,
				      qobj_name_compare());
	if (found == t.cend() || (*found)->objectName() != obj->objectName())
		t.insert(found, obj);
}
}

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	int failCount = 0;
	for (auto &iter : mcr::test::testObjects()) {
		int err = QTest::qExec(iter, app.arguments());
		if (err)
			failCount++;
	}
	qsizetype count = mcr::test::testObjects().count();
	qInfo() << "********* Test Report Summary *********"
		<< "\nTests Succeeded:" << (count - failCount)
		<< "\nTests Failed:" << failCount << "\nSuccessfulness:"
		<< (100.0 - 100.0 * (float)failCount / (float)count) << "%"
		<< "\n***************************************";
	return failCount;
}

#endif
