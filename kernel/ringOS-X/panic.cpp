#include "panic.hpp"
#include "BasicRenderer.hpp"

void Panic(const char *panicMessage)
{
    GlobalRenderer->Clear(Colors.red, true);

    GlobalRenderer->Print("Kernel Panic\n\n");
    GlobalRenderer->Print(panicMessage);
}
