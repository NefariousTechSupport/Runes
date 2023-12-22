#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <io.h>
#include <assert.h>
#include <string.h>
#if defined(DEBUG) && defined(WIN32)
#include <Windows.h>
#endif

#include "kTfbSpyroTag_HatType.hpp"

//https://stackoverflow.com/questions/60328079/piping-console-output-from-winmain-when-running-from-a-console
void RedirectIOToConsole() {
#if defined(DEBUG) && defined(WIN32)
    if (AttachConsole(ATTACH_PARENT_PROCESS)==false) return;

    HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    int SystemOutput = _open_osfhandle(intptr_t(ConsoleOutput), _O_TEXT);

    // check if output is a console and not redirected to a file
    if(isatty(SystemOutput)==false) return; // return if it's not a TTY

    FILE *COutputHandle = _fdopen(SystemOutput, "w");

    // Get STDERR handle
    HANDLE ConsoleError = GetStdHandle(STD_ERROR_HANDLE);
    int SystemError = _open_osfhandle(intptr_t(ConsoleError), _O_TEXT);
    FILE *CErrorHandle = _fdopen(SystemError, "w");

    // Get STDIN handle
    HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
    int SystemInput = _open_osfhandle(intptr_t(ConsoleInput), _O_TEXT);
    FILE *CInputHandle = _fdopen(SystemInput, "r");

    //make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
    std::ios::sync_with_stdio(true);

    // Redirect the CRT standard input, output, and error handles to the console
    freopen_s(&CInputHandle, "CONIN$", "r", stdin);
    freopen_s(&COutputHandle, "CONOUT$", "w", stdout);
    freopen_s(&CErrorHandle, "CONOUT$", "w", stderr);

    //Clear the error state for each of the C++ standard stream objects.
    std::wcout.clear();
    std::cout.clear();
    std::wcerr.clear();
    std::cerr.clear();
    std::wcin.clear();
    std::cin.clear();
#endif
}

void assertHatIds()
{
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_Sunday], "Fancy Hat") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_Carmen], "Tropical Turban") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_Mercury], "Winged Hat") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_Pumpkin], "Pumpkin Hat") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_FirefighterHelmet], "Firefighter Helmet") == 0);
    assert(strcmp(hatNames_en[60], "Hat 60") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_Carrot], "Carrot Hat") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_Trucker], "Trucker Hat") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_Rasta], "Rasta Hat") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_Volcano], "Volcano Hat") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_Tricorn], "Tricorn Hat") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_AviatorsCap], "Aviator's Cap") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_GlitteringTiara], "Glittering Tiara") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_Cactus], "Cactus Hat") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_AwesomeHat], "Awesome Hat") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_Fan], "Ceiling Fan Hat") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_Skull], "Dragon Skull") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_Melon], "Melon Hat") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_RubberGlove], "Rubber Glove Hat") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_Tibet], "Sherpa Hat") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_PorkPieChecker], "Pork Pie Hat") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_Fireflies], "Firefly Jar") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_Wizard02], "Wizard Hat (Trap Team)") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_Raver], "Raver Hat") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_Molekin], "Molekin Mountain Hat") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_DiveBomber], "Dive Bomber Hat") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_SplatterSplasher], "Splatter Splasher Spires") == 0);
    assert(strcmp(hatNames_en[kTfbSpyroTag_Hat_EonHelm], "Eon's Helm") == 0);
}