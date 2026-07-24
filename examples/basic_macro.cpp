/* basic_macro.cpp -- Minimal example: create a context and register types.
 *
 * Build:
 *   g++ -std=c++17 -I.. basic_macro.cpp -L../build -lmacro -lpthread -o basic_macro
 */

#include <cstdio>
#include "mcr/libmacro.h"

int main()
{
    /* Create and enable a context */
    auto ctx = mcr::factory::createContext(true);

    /* Register built-in signal and trigger types */
    ctx->signalRegistry().map<mcr::Modifier>();
    ctx->triggerRegistry().map<mcr::Action>();

    /* Register a macro */
    mcr::Macro macro(ctx.get());
    macro.setName("exampleMacro");
    macro.setEnabled(true);

    /* The macro is now registered and ready to receive signals */
    std::printf("Macro '%s' created and enabled.\n", macro.name());

    /* Clean up */
    macro.setEnabled(false);
    ctx->setEnabled(false);
    return 0;
}
