#pragma once

#include <QtTest/QtTest>

#include "mcr/libmacro.h"

class TRegistry : public QObject
{
	Q_OBJECT
public:
	const static size_t COUNT = 16;
private slots:
	void initTestCase();
	void cleanupTestCase();
	void base();

private:
	mcr_Interface _interfaces[COUNT];
};
