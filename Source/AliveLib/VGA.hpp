#pragma once

#include "FunctionFwd.hpp"
#include "bmp.hpp"

void VGA_ForceLink();

EXPORT void CC VGA_Shutdown_4F3170();
EXPORT void CC VGA_CopyToFront_4F3710(Bitmap* pBmp, RECT* pRect);
EXPORT void CC VGA_CopyToFront_4F3EB0(Bitmap* pBmp, RECT* pRect, unsigned __int8 screenMode);
EXPORT bool VGA_IsWindowMode_4F31E0();

ALIVE_VAR_EXTERN(Bitmap, sVGA_Bmp1_BD2A20);
