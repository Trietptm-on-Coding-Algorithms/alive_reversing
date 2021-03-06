#include "stdafx.h"
#include "PsxDisplay.hpp"
#include "Function.hpp"
#include "ScreenManager.hpp"
#include "VGA.hpp"
#include "Error.hpp"
#include "Midi.hpp"
#include <type_traits>
#include <gmock/gmock.h>

ALIVE_VAR(1, 0x5C1130, PsxDisplay, gPsxDisplay_5C1130, {});

EXPORT void CC PSX_PutDrawEnv_4F5980(const PSX_DRAWENV* /*pDrawEnv*/)
{
    NOT_IMPLEMENTED();
}

EXPORT void CC sub_4945D0()
{
    NOT_IMPLEMENTED();
}

EXPORT void CC PSX_Display_OrderingTable_4F6540(int* /*pOT*/)
{
    NOT_IMPLEMENTED();
}

EXPORT void CC PSX_OrderingTable_Init_4F6290(int* /*otBuffer*/, int /*otBufferSize*/)
{
    NOT_IMPLEMENTED();
}

EXPORT void CC sub_4EDAB0(Bitmap* /*pBmp*/, int /*left*/, int /*top*/, int /*width*/)
{
    NOT_IMPLEMENTED();
}

EXPORT void CC sub_4ED9E0()
{
    NOT_IMPLEMENTED();
}

// TODO: Probably not font related at all?
EXPORT int __cdecl Fnt_4955F0(__int16 a1, __int16 a2, __int16 a3, __int16 a4)
{
    NOT_IMPLEMENTED();
    return 0;
}

/*
// No visible effect but just resets data
void __cdecl sub_495660()
{

}

// Prevents fonts getting to vram?
signed __int16 __cdecl sub_4956C0(unsigned __int16 a1, __int16 a2, unsigned __int16 a3, WORD *a4)
{
    return 1;
}

// LED font/menu glyphs related
signed int __cdecl sub_4958F0(signed __int16 *a1, char a2)
{
    return 1;
}

// LED font rendering related
void __cdecl sub_495A60(int a1, int a2)
{

}
*/

ALIVE_VAR(1, 0xC1D1A0, Bitmap, stru_C1D1A0, {});
ALIVE_VAR(1, 0xC2D060, PSX_DISPENV, sLastDispEnv_C2D060, {});
ALIVE_VAR(1, 0xBD146D, BYTE, sScreenMode_BD146D, 0);
ALIVE_VAR(1, 0xBD0F20, BYTE, byte_BD0F20, 0);
ALIVE_VAR(1, 0x578324, BYTE, byte_578324, 0);
ALIVE_VAR(1, 0xBD1465, BYTE, sPsxEMU_show_vram_BD1465, 0);
ALIVE_VAR(1, 0xBD1464, BYTE, byte_BD1464, 0);

struct TextRecords
{
    char field_0_src_txt[1024];
    char field_400_dst_txt[1027];
};
ALIVE_ASSERT_SIZEOF(TextRecords, 0x803);

struct DebugTexts
{
    BYTE field_0_xMargin;
    BYTE field_1_yMargin;
    BYTE field_2_displayWidth;
    BYTE field_3_displayHeight;
    DWORD field_4_max_len;
    BYTE field_8_bgColour;
    TextRecords field_9_text;
};
ALIVE_ASSERT_SIZEOF(DebugTexts, 0x80C);

ALIVE_VAR(1, 0xBD0F28, int, sFntCount_BD0F28, 0);
ALIVE_ARY(1, 0xC27640, DebugTexts, 4, sTexts_C27640, {});

EXPORT void CC DebugFont_Reset_4F8B40()
{
    memset(sTexts_C27640, 0, sizeof(DebugTexts)*4); // 8240u
    sFntCount_BD0F28 = 0;
}

EXPORT void CC DebugFont_Update_Text_4F8BE0(signed int idx)
{
    if (idx >= 0 && idx <= 3)
    {
        strcpy(sTexts_C27640[idx].field_9_text.field_400_dst_txt, sTexts_C27640[idx].field_9_text.field_0_src_txt);
        sTexts_C27640[idx].field_9_text.field_0_src_txt[0] = 0;
    }
}

ALIVE_ARY(1, 0xBB47CC, char, 600, sDebugFontTmpBuffer_BB47CC, {});
ALIVE_VAR(1, 0xBB4A24, short, sbDebugFontLoaded_BB4A24, 0);
ALIVE_VAR(1, 0xBB47C8, int, sDebugTextIdx_BB47C8, 0);


EXPORT int CC DebugFont_Open_4F8AB0(BYTE xMargin, BYTE yMargin, BYTE displayWidth, BYTE displayHeight, BYTE bgColour, unsigned int maxLenChars)
{
    const int idx = sFntCount_BD0F28;
    if (sFntCount_BD0F28 == 4)
    {
        return -1;
    }

    ++sFntCount_BD0F28;

    sTexts_C27640[idx].field_0_xMargin = xMargin;
    sTexts_C27640[idx].field_1_yMargin = yMargin;
    sTexts_C27640[idx].field_2_displayWidth = displayWidth;
    sTexts_C27640[idx].field_3_displayHeight = displayHeight;
    sTexts_C27640[idx].field_8_bgColour = bgColour | 1;
    sTexts_C27640[idx].field_9_text.field_0_src_txt[0] = 0;
    sTexts_C27640[idx].field_9_text.field_400_dst_txt[0] = 0;

    int limitedMaxLen = maxLenChars;
    if (maxLenChars > 1023)
    {
        limitedMaxLen = 1023;
    }
    sTexts_C27640[idx].field_4_max_len = limitedMaxLen;
    return idx;
}

EXPORT int CC DebugFont_Init_4DCF40() // Font
{
    if (!sbDebugFontLoaded_BB4A24)
    {
        Fnt_4955F0(960, 256, 991, 287);
        Fnt_4955F0(960, 384, 975, 385);
        sbDebugFontLoaded_BB4A24 = 1;
    }
    DebugFont_Reset_4F8B40();
    sDebugTextIdx_BB47C8 = DebugFont_Open_4F8AB0(8, 16, gPsxDisplay_5C1130.field_0_width, 200, 0, 600u);
    //nullsub_7(sTextIdx_BB47C8);
    sDebugFontTmpBuffer_BB47CC[0] = 0;
    return 0;
}


EXPORT int CC DebugFont_Printf_4F8B60(int idx, const char* formatStr, ...)
{
    va_list va;
    va_start(va, formatStr);
    if (idx < 0 || idx > 3)
    {
        return -1;
    }

    char buffer[1024] = {};
    vsprintf(buffer, formatStr, va);
    strncat(sTexts_C27640[idx].field_9_text.field_0_src_txt, buffer, sTexts_C27640[idx].field_4_max_len);
    return strlen(sTexts_C27640[idx].field_9_text.field_0_src_txt);
}

EXPORT void CC DebugFont_Flush_4DD050()
{
    DebugFont_Printf_4F8B60(sDebugTextIdx_BB47C8, sDebugFontTmpBuffer_BB47CC);
    DebugFont_Update_Text_4F8BE0(sDebugTextIdx_BB47C8);
    sDebugFontTmpBuffer_BB47CC[0] = 0;
}

namespace Test
{
    static void Test_sub_4F8AB0()
    {
        DebugFont_Reset_4F8B40();

        DebugFont_Open_4F8AB0(8, 16, 77, 200, 33, 600);
        ASSERT_EQ(16, sTexts_C27640[0].field_1_yMargin);
        ASSERT_EQ(0, sTexts_C27640[0].field_9_text.field_0_src_txt[0]);
        ASSERT_EQ(0, sTexts_C27640[0].field_9_text.field_400_dst_txt[0]);

        DebugFont_Open_4F8AB0(8, 22, 77, 200, 33, 600);
        ASSERT_EQ(22, sTexts_C27640[1].field_1_yMargin);

        DebugFont_Open_4F8AB0(8, 33, 77, 200, 33, 600);
        ASSERT_EQ(33, sTexts_C27640[2].field_1_yMargin);

        DebugFont_Open_4F8AB0(8, 55, 77, 200, 33, 600);
        ASSERT_EQ(55, sTexts_C27640[3].field_1_yMargin);

        ASSERT_EQ(-1, DebugFont_Open_4F8AB0(8, 33, 77, 200, 33, 600));

        DebugFont_Reset_4F8B40();
    }

    void PsxDisplayTests()
    {
        Test_sub_4F8AB0();
    }
}

static void PSX_DrawDebugTextBuffers(Bitmap* pBmp, const RECT& rect)
{
    if (sFntCount_BD0F28 <= 0)
    {
        return;
    }

    const LONG fontHeight = BMP_Get_Font_Height_4F21F0(pBmp);
    for (int i = 0; i < sFntCount_BD0F28; i++)
    {
        DebugTexts* pRecord = &sTexts_C27640[i];
        const int xpos = rect.left + pRecord->field_0_xMargin;
        int ypos = rect.top + pRecord->field_1_yMargin;
        const int bgColour = pRecord->field_8_bgColour;
        for (char* j = strtok(pRecord->field_9_text.field_400_dst_txt, "\n\r"); j; j = strtok(0, "\n\r"))
        {
            int fontColour = Bmp_Convert_Colour_4F17D0(&sPsxVram_C1D160, 255, 255, 191);
            BMP_Draw_String_4F2230(pBmp, xpos, ypos, fontColour, bgColour, j);
            ypos += fontHeight;
        }
    }
}

EXPORT void CC PSX_PutDispEnv_4F5640(const PSX_DISPENV *pDispEnv, char a2)
{
    if (!pDispEnv)
    {
        Error_PushErrorRecord_4F2920("C:\\abe2\\code\\PSXEmu\\LIBGPU.C", 217, -1, "PutDispEnv(): env == NULL");
        return;
    }

    MIDI_UpdatePlayer_4FDC80();
    memcpy(&sLastDispEnv_C2D060, pDispEnv, sizeof(sLastDispEnv_C2D060));
    if (sPsxVram_C1D160.field_4_pLockedPixels)
    {
        BMP_unlock_4F2100(&sPsxVram_C1D160);
    }

    if (!byte_BD0F20 && byte_578324)
    {
        if (sPsxEMU_show_vram_BD1465)
        {
            VGA_CopyToFront_4F3710(&sPsxVram_C1D160, nullptr);
            MIDI_UpdatePlayer_4FDC80();
            return;
        }

        Bitmap* pBmp = nullptr;
        RECT rect = {};
        if (byte_BD1464)
        {
            rect.top = 0;
            rect.left = 0;
            rect.right = sLastDispEnv_C2D060.disp.w;
            rect.bottom = sLastDispEnv_C2D060.disp.h;
            pBmp = &stru_C1D1A0;
        }
        else
        {
            rect.left = sLastDispEnv_C2D060.disp.x;
            rect.top = sLastDispEnv_C2D060.disp.y;
            rect.right = sLastDispEnv_C2D060.disp.x + sLastDispEnv_C2D060.disp.w;
            rect.bottom = sLastDispEnv_C2D060.disp.y + sLastDispEnv_C2D060.disp.h;
            pBmp = &sPsxVram_C1D160;
        }

        PSX_DrawDebugTextBuffers(pBmp, rect);

        if (a2 && VGA_IsWindowMode_4F31E0())
        {
            sub_4EDAB0(pBmp, rect.left, rect.top, rect.right - rect.left);
            sub_4ED9E0();
        }
        else
        {
            VGA_CopyToFront_4F3EB0(pBmp, &rect, sScreenMode_BD146D);
        }

        // TODO: Removed dead increment here
    }
    MIDI_UpdatePlayer_4FDC80();
}

using TPsxEmuCallBack = std::add_pointer<int(DWORD)>::type;

ALIVE_VAR(1, 0xC1D184, TPsxEmuCallBack, sPsxEmu_CallBack_1_dword_C1D184, nullptr);
ALIVE_VAR(1, 0xBD0F21, BYTE, byte_BD0F21, 0);


EXPORT void CC PSX_4F58E0(const PSX_DISPENV* pDispEnv)
{
    if (!sPsxEmu_CallBack_1_dword_C1D184 || !sPsxEmu_CallBack_1_dword_C1D184(0))
    {
        if (!byte_BD0F21)
        {
            if (sVGA_Bmp1_BD2A20.field_8_width != 320 || pDispEnv->disp.w != 640)
            {
                PSX_PutDispEnv_4F5640(pDispEnv, 1);
            }
            else
            {
                PSX_PutDispEnv_4F5640(pDispEnv, 0);
            }
        }

        if (sPsxEmu_CallBack_1_dword_C1D184)
        {
            sPsxEmu_CallBack_1_dword_C1D184(1);
        }
    }
}

ALIVE_VAR(1, 0x5CA4D1, bool, sCommandLine_NoFrameSkip_5CA4D1, false);
ALIVE_VAR(1, 0x55EF8C, int, sbDisplayRenderFrame_55EF8C, 1);

void PsxDisplay::PSX_Display_Render_OT_41DDF0()
{
    if (field_8_max_buffers <= 1)
    {
        // Single buffered rendering
        PSX_PutDrawEnv_4F5980(&field_10_drawEnv[0].field_0_draw_env);
        sub_4945D0();
        if (sCommandLine_NoFrameSkip_5CA4D1)
        {
            PSX_Display_OrderingTable_4F6540(field_10_drawEnv[0].field_70_ot_buffer);
            PSX_DrawSync_4F6280(0);
        }
        else
        {
            if (sbDisplayRenderFrame_55EF8C)
            {
                PSX_Display_OrderingTable_4F6540(field_10_drawEnv[0].field_70_ot_buffer);
                PSX_DrawSync_4F6280(0);
            }
            else
            {
                pScreenManager_5BB5F4->sub_40EE10();
                byte_BD0F20 = 1;
            }
            PSX_VSync_4F6170(2);
        }
        PSX_4F58E0(&field_10_drawEnv[0].field_5C_disp_env);
        PSX_OrderingTable_Init_4F6290(field_10_drawEnv[0].field_70_ot_buffer, field_A_buffer_size);
        field_C_buffer_index = 0;
    }
    else
    {
        PSX_DrawSync_4F6280(0);
        PSX_VSync_4F6170(2);

        const auto prevBufferIdx = field_C_buffer_index;
        field_C_buffer_index++;
        if (field_C_buffer_index > field_8_max_buffers)
        {
            field_C_buffer_index = 0;
        }

        // Set up next
        PSX_OrderingTable_Init_4F6290(field_10_drawEnv[field_C_buffer_index].field_70_ot_buffer,  field_A_buffer_size);
        PSX_4F58E0(&field_10_drawEnv[field_C_buffer_index].field_5C_disp_env);
        PSX_PutDrawEnv_4F5980(&field_10_drawEnv[field_C_buffer_index].field_0_draw_env);

        // Display current
        PSX_Display_OrderingTable_4F6540(field_10_drawEnv[prevBufferIdx].field_70_ot_buffer);
    }
}
