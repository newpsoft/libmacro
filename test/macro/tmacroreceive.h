#pragma once

#include <QtTest/QtTest>

#include "mcr/libmacro.h"

class TMacroReceive : public QObject
{
	Q_OBJECT
public:
	TMacroReceive() : _ctx(nullptr), _mcrPt(nullptr)
	{
	}

private slots:
	void initTestCase();
	void cleanupTestCase();

	void receive();
private:
	mcr_context *_ctx;
	mcr::Macro *_mcrPt;
};
