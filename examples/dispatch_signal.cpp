/* dispatch_signal.cpp -- Example: dispatch a signal through the dispatcher.
 *
 * Build:
 *   g++ -std=c++17 -I.. dispatch_signal.cpp -L../build -lmacro -lpthread -o dispatch_signal
 */

#include <cstdio>
#include "mcr/libmacro.h"

int main()
{
    auto ctx = mcr::factory::createContext(true);

    /* Register built-in types */
    ctx->signalRegistry().map<mcr::Modifier>();
    ctx->triggerRegistry().map<mcr::Action>();

    /* Get the generic dispatcher */
    auto &dispatcher = *ctx->genericDispatcher();

    std::printf("Dispatcher has %u receivers.\n", dispatcher.count());

    /* Create a signal and dispatch it */
    mcr::Modifier signal(ctx.get());
    unsigned int mods = 0;

    bool blocked = dispatcher.dispatch(&signal, mods);
    std::printf("Dispatch result: %s\n", blocked ? "blocked" : "passed through");

    ctx->setEnabled(false);
    return 0;
}
