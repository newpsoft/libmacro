#include "tkeys.h"
#include "sendkey.h"
#include <iostream>
#include <thread>

TKeys::TKeys(QObject *parent)
    : QObject{parent}, libmacro_pt(nullptr)
{}

void TKeys::initTestCase()
{
    libmacro_pt = new mcr::Libmacro;
}

void TKeys::cleanupTestCase()
{
    libmacro_pt->setEnabled(false);
    delete libmacro_pt;
}

void sendKeyThread(mcr::Libmacro *libmacro_pt)
{
    SendKey sender = SendKey(libmacro_pt, 'a');
    sender.send();
}

void TKeys::inputAnything()
{
    std::string s;
    std::thread t(sendKeyThread, libmacro_pt);
    t.detach();
    std::cin >> s;
    Q_ASSERT(!s.empty());
}
