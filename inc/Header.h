#pragma once

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_internal.h"
#include "imgui.h"
#include <stdio.h>
#include <stdlib.h>

static inline ImDrawFlags MFixRectCornerFlags(ImDrawFlags flags)
{
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    // Obsoleted in 1.82 (from February 2021)
    // Legacy Support for hard coded ~0 (used to be a suggested equivalent to ImDrawCornerFlags_All)
    //   ~0   --> ImDrawFlags_RoundCornersAll or 0
    if (flags == ~0)
        return ImDrawFlags_RoundCornersAll;

    // Legacy Support for hard coded 0x01 to 0x0F (matching 15 out of 16 old flags combinations)
    //   0x01 --> ImDrawFlags_RoundCornersTopLeft (VALUE 0x01 OVERLAPS ImDrawFlags_Closed but ImDrawFlags_Closed is never valid in this path!)
    //   0x02 --> ImDrawFlags_RoundCornersTopRight
    //   0x03 --> ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight
    //   0x04 --> ImDrawFlags_RoundCornersBotLeft
    //   0x05 --> ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersBotLeft
    //   ...
    //   0x0F --> ImDrawFlags_RoundCornersAll or 0
    // (See all values in ImDrawCornerFlags_)
    if (flags >= 0x01 && flags <= 0x0F)
        return (flags << 4);

    // We cannot support hard coded 0x00 with 'float rounding > 0.0f' --> replace with ImDrawFlags_RoundCornersNone or use 'float rounding = 0.0f'
#endif

    // If this triggers, please update your code replacing hardcoded values with new ImDrawFlags_RoundCorners* values.
    // Note that ImDrawFlags_Closed (== 0x01) is an invalid flag for AddRect(), AddRectFilled(), PathRect() etc...
    IM_ASSERT((flags & 0x0F) == 0 && "Misuse of legacy hardcoded ImDrawCornerFlags values!");

    if ((flags & ImDrawFlags_RoundCornersMask_) == 0)
        flags |= ImDrawFlags_RoundCornersAll;

    return flags;
}


void AddRectFilledMultiColor(ImDrawList* drawlist, const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left, float rounding, ImDrawFlags flags)
{
    if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) & IM_COL32_A_MASK) == 0)
        return;

    flags = MFixRectCornerFlags(flags);
    rounding = ImMin(rounding, ImFabs(p_max.x - p_min.x) * (((flags & ImDrawCornerFlags_Top) == ImDrawCornerFlags_Top) || ((flags & ImDrawCornerFlags_Bot) == ImDrawCornerFlags_Bot) ? 0.5f : 1.0f) - 1.0f);
    rounding = ImMin(rounding, ImFabs(p_max.y - p_min.y) * (((flags & ImDrawCornerFlags_Left) == ImDrawCornerFlags_Left) || ((flags & ImDrawCornerFlags_Right) == ImDrawCornerFlags_Right) ? 0.5f : 1.0f) - 1.0f);

    // https://github.com/ocornut/imgui/issues/3710#issuecomment-758555966
    if (rounding > 0.0f)
    {
        const int size_before = drawlist->VtxBuffer.Size;
        drawlist->AddRectFilled(p_min, p_max, IM_COL32_WHITE, rounding, flags);
        const int size_after = drawlist->VtxBuffer.Size;

        for (int i = size_before; i < size_after; i++)
        {
            ImDrawVert* vert = drawlist->VtxBuffer.Data + i;

            ImVec4 upr_left = ImGui::ColorConvertU32ToFloat4(col_upr_left);
            ImVec4 bot_left = ImGui::ColorConvertU32ToFloat4(col_bot_left);
            ImVec4 up_right = ImGui::ColorConvertU32ToFloat4(col_upr_right);
            ImVec4 bot_right = ImGui::ColorConvertU32ToFloat4(col_bot_right);

            float X = ImClamp((vert->pos.x - p_min.x) / (p_max.x - p_min.x), 0.0f, 1.0f);

            // 4 colors - 8 deltas

            float r1 = upr_left.x + (up_right.x - upr_left.x) * X;
            float r2 = bot_left.x + (bot_right.x - bot_left.x) * X;

            float g1 = upr_left.y + (up_right.y - upr_left.y) * X;
            float g2 = bot_left.y + (bot_right.y - bot_left.y) * X;

            float b1 = upr_left.z + (up_right.z - upr_left.z) * X;
            float b2 = bot_left.z + (bot_right.z - bot_left.z) * X;

            float a1 = upr_left.w + (up_right.w - upr_left.w) * X;
            float a2 = bot_left.w + (bot_right.w - bot_left.w) * X;


            float Y = ImClamp((vert->pos.y - p_min.y) / (p_max.y - p_min.y), 0.0f, 1.0f);
            float r = r1 + (r2 - r1) * Y;
            float g = g1 + (g2 - g1) * Y;
            float b = b1 + (b2 - b1) * Y;
            float a = a1 + (a2 - a1) * Y;
            ImVec4 RGBA(r, g, b, a);

            //RGBA = RGBA * ImGui::ColorConvertU32ToFloat4(vert->col
            ImVec4 tmpa = ImGui::ColorConvertU32ToFloat4(vert->col);
            //RGBA.x = RGBA.x * ImGui::ColorConvertU32ToFloat4(vert->col).x;
            //RGBA.y = RGBA.y * ImGui::ColorConvertU32ToFloat4(vert->col).y;
           // RGBA.z = RGBA.z * ImGui::ColorConvertU32ToFloat4(vert->col).z;
           // RGBA.w = RGBA.w * ImGui::ColorConvertU32ToFloat4(vert->col).w;
            RGBA.x = RGBA.x * tmpa.x;
            RGBA.y = RGBA.y * tmpa.y;
            RGBA.z = RGBA.z * tmpa.z;
            RGBA.w = RGBA.w * tmpa.w;

            vert->col = ImColor(RGBA);
        }
        return;
    }

    const ImVec2 uv = drawlist->_Data->TexUvWhitePixel;
    drawlist->PrimReserve(6, 4);
    drawlist->PrimWriteIdx((ImDrawIdx)(drawlist->_VtxCurrentIdx)); drawlist->PrimWriteIdx((ImDrawIdx)(drawlist->_VtxCurrentIdx + 1)); drawlist->PrimWriteIdx((ImDrawIdx)(drawlist->_VtxCurrentIdx + 2));
    drawlist->PrimWriteIdx((ImDrawIdx)(drawlist->_VtxCurrentIdx)); drawlist->PrimWriteIdx((ImDrawIdx)(drawlist->_VtxCurrentIdx + 2)); drawlist->PrimWriteIdx((ImDrawIdx)(drawlist->_VtxCurrentIdx + 3));
    drawlist->PrimWriteVtx(p_min, uv, col_upr_left);
    drawlist->PrimWriteVtx(ImVec2(p_max.x, p_min.y), uv, col_upr_right);
    drawlist->PrimWriteVtx(p_max, uv, col_bot_right);
    drawlist->PrimWriteVtx(ImVec2(p_min.x, p_max.y), uv, col_bot_left);
}


void AddImageCircle(ImDrawList* drawlist, ImTextureID user_texture_id, const ImVec2& center, float radius, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col, ImDrawFlags flags)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    drawlist->PushTextureID(user_texture_id);
    int vert_start_idx = drawlist->VtxBuffer.Size;
    drawlist->AddCircleFilled(center, radius, IM_COL32_WHITE);
    int vert_end_idx = drawlist->VtxBuffer.Size;
    ImVec2 c1 = center, c2 = center;
    c1.x = c1.x - radius;
    c1.y = c1.y - radius;
    c2.x = c2.x + radius;
    c2.y = c2.y + radius;
    ImGui::ShadeVertsLinearUV(drawlist, vert_start_idx, vert_end_idx, c1, c2, uv_min, uv_max, true);
    drawlist->PopTextureID();
}

#ifndef M_PI
#define M_PI 3.14
#endif

/*
const float time = ((float)(((unsigned int) (ImGui::GetTime()*1000.f))%50000)-25000.f)/25000.f;
    float progress=(time>0?time:-time);
    // No IDs needed for ProgressBars:
    ImGui::ProgressBar("ProgressBar",progress);
    ImGui::ProgressBar("ProgressBar",1.f-progress);
    ImGui::ProgressBar("",500+progress*1000,500,1500,"%4.0f (absolute value in [500,1500] and fixed bar size)",ImVec2(150,-1));
    ImGui::ProgressBar("",500+progress*1000,500,1500,"%3.0f%% (same as above, but with percentage and new colors)",ImVec2(150,-1),ImVec4(0.7,0.7,1,1),ImVec4(0.05,0.15,0.5,0.8),ImVec4(0.8,0.8,0,1));
    // This one has just been added to ImGui:
    //char txt[48]="";sprintf(txt,"%3d%% (ImGui default progress bar)",(int)(progress*100));
    //ImGui::ProgressBar(progress,ImVec2(0,0),txt);
*/
float ProgressBar(const char* optionalPrefixText, float value, const float minValue, const float maxValue, const char* format, const ImVec2& sizeOfBarWithoutTextInPixels, const ImVec4& colorLeft, const ImVec4& colorRight, const ImVec4& colorBorder) {
    if (value < minValue) value = minValue;
    else if (value > maxValue) value = maxValue;
    const float valueFraction = (maxValue == minValue) ? 1.0f : ((value - minValue) / (maxValue - minValue));
    const bool needsPercConversion = strstr(format, "%%") != NULL;

    ImVec2 size = sizeOfBarWithoutTextInPixels;
    if (size.x <= 0) size.x = ImGui::GetWindowWidth() * 0.25f;
    if (size.y <= 0) size.y = ImGui::GetTextLineHeightWithSpacing(); // or without

    const ImFontAtlas* fontAtlas = ImGui::GetIO().Fonts;

    if (optionalPrefixText && strlen(optionalPrefixText) > 0) {
        ImGui::AlignTextToFramePadding();
        ImGui::Text("%s", optionalPrefixText);
        ImGui::SameLine();
    }

    if (valueFraction > 0) {
        ImGui::Image(fontAtlas->TexID, ImVec2(size.x * valueFraction, size.y), fontAtlas->TexUvWhitePixel, fontAtlas->TexUvWhitePixel, colorLeft, colorBorder);
    }
    if (valueFraction < 1) {
        if (valueFraction > 0) ImGui::SameLine(0, 0);
        ImGui::Image(fontAtlas->TexID, ImVec2(size.x * (1.f - valueFraction), size.y), fontAtlas->TexUvWhitePixel, fontAtlas->TexUvWhitePixel, colorRight, colorBorder);
    }
    ImGui::SameLine();

    ImGui::Text(format, needsPercConversion ? (valueFraction * 100.f + 0.0001f) : value);
    return valueFraction;
}


