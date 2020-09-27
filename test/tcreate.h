#pragma once

#include <QtTest/QtTest>

#include "mcr/libmacro.h"

class TCreate : public QObject
{
	Q_OBJECT
public:
	const static size_t COUNT = 2;
private slots:
	void base();
	void baseHeap();
	void wholeContext();
	void wholeContextHeap();

private:
	void verifyContext(struct mcr_context &ctx);
	void verifyContext(mcr::Libmacro &ctx);
};
