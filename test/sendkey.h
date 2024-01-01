#ifndef SENDKEY_H
#define SENDKEY_H

#include "mcr/libmacro.h"

class SendKey
{
public:
#ifdef MCR_PLATFORM_WINDOWS
    const int RETURN = VK_RETURN;
#else
    const int RETURN = KEY_ENTER;
#endif
    int key;

    explicit SendKey(mcr::Libmacro *libmacro_pt, int key)
        : key(key), libmacro_pt(libmacro_pt)
    {
    }

    void send(int value);
    void send();

private:
    mcr::Libmacro *libmacro_pt;
};

#endif // SENDKEY_H
