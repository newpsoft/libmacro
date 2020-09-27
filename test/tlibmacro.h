#pragma once

#include <QtTest/QtTest>

#include "mcr/libmacro.h"

class TLibmacro : public QObject
{
	Q_OBJECT
public:
	const static size_t COUNT = 2;
private slots:
	void initTestCase();
	void cleanupTestCase();
};
