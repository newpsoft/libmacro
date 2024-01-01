#include <QtCore>
#include <QtTest/QtTest>

#include "tcreate.h"
#include "tregistry.h"
#include "signal/tgendispatch.h"
#include "macro/tmacroreceive.h"
#include "tkeys.h"

int main(int argc, char **argv)
{
	TCreate tCreate;
//	TRegistry tRegistry;
//	TGenDispatch tGendispatch;
//	TMacroReceive tMacroreceive;
    TKeys tKeys;
	QTest::qExec(&tCreate, argc, argv);
//	QTest::qExec(&tRegistry, argc, argv);
//	QTest::qExec(&tGendispatch, argc, argv);
//	QTest::qExec(&tMacroreceive, argc, argv);
    QTest::qExec(&tKeys, argc, argv);
	return 0;
}
