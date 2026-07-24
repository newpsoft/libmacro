/* serial_names.cpp -- Example: extend name-value mappings with custom names.
 *
 * Build:
 *   g++ -std=c++17 -I.. serial_names.cpp -L../build -lmacro -lpthread -o serial_names
 */

#include <cstdio>
#include "mcr/libmacro.h"

int main()
{
    auto ctx = mcr::factory::createContext(true);
    auto &serial = ctx->serial();

    /* Register a custom modifier name */
    serial.setModifierName("JoyButton1", MCR_MF_USER);
    serial.setModifierValueName(MCR_MF_USER, "Joystick Button 1");

    /* Register a custom trigger mode name */
    serial.setTriggerModeName("CustomMode", MCR_TM_USER);
    serial.setTriggerModeValueName(MCR_TM_USER, "Custom Trigger Mode");

    /* Look up by name */
    mcr_ModFlags modVal = serial.modifiers("JoyButton1");
    std::printf("modifiers(\"JoyButton1\") = %u\n", modVal);

    /* Reverse lookup by value */
    const char *modName = serial.modifiersName(MCR_MF_USER);
    std::printf("modifiersName(MCR_MF_USER) = %s\n", modName);

    /* Trigger mode lookup */
    mcr_TriggerMode tm = serial.triggerMode("CustomMode");
    std::printf("triggerMode(\"CustomMode\") = %d\n", tm);

    const char *tmName = serial.triggerModeName(MCR_TM_USER);
    std::printf("triggerModeName(MCR_TM_USER) = %s\n", tmName);

    /* Clean up */
    ctx->setEnabled(false);
    return 0;
}
