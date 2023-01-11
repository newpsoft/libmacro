#include "tregistry.h"

#include <QRandomGenerator>

void TRegistry::initTestCase()
{

}

void TRegistry::cleanupTestCase()
{

}

void TRegistry::base()
{
	mcr::Registry *boo;
	std::string lorem =
		#include "assets/lorem_ipsum.txt"
			;
	std::string name;
	for (size_t i = 0; i < COUNT; i++) {
		boo = new mcr::Registry;
		for (size_t j = 0; j < COUNT; j++) {
			mcr_Interface_init(_interfaces + i);
			name = lorem.substr(j, COUNT);
			mcr_register(&**boo, _interfaces + i, name.c_str(), NULL, 0);
//			boo->setName(_interfaces + i, name.c_str());
			QCOMPARE(boo->interface(name.c_str()), _interfaces + i);
		}
		delete boo;
	}
}
