#include "sendkey.h"
#include <QThread>

void SendKey::send(int value)
{
    const unsigned long INTERVAL = 382;
    mcr::Signal keySignal = mcr::Signal(&libmacro_pt->iKey);
    mcr_Key k;
    k.key = value;
    k.apply = MCR_SET;
    keySignal.instance().data_member.data = &k;
    QThread::msleep(INTERVAL);
    mcr_send(&libmacro_pt->self, &*keySignal);
    k.apply = MCR_UNSET;
    QThread::msleep(INTERVAL);
    mcr_send(&libmacro_pt->self, &*keySignal);
}

void SendKey::send()
{
    send(key);
    send(RETURN);
}
