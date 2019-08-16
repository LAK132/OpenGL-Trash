// https://github.com/ocornut/imgui/issues/1720#issuecomment-378694082

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#ifndef IMGUI_SPLITTER
#define IMGUI_SPLITTER

namespace ImGui
{    
    bool Splitter(float thickness, float* size1, float* size2, float min_size1, float min_size2, float max_size, bool split_vertically = true, float splitter_long_axis_size = -1.0f);
}

#endif // IMGUI_SPLITTER

#ifdef IMGUI_SPLITTER_IMPLEM

namespace ImGui
{    
    bool Splitter(float thickness, float* size1, float* size2, float min_size1, float min_size2, float max_size, bool split_vertically, float splitter_long_axis_size)
    {
        if(*size1 > max_size - (min_size2 + thickness)) 
            *size1 = max_size - (min_size2 + thickness);
        *size2 = max_size - (*size1 + thickness);
        if(*size1 < min_size1) *size1 = min_size1;
        if(*size2 < min_size2) *size2 = min_size2;

        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImGuiID id = window->GetID("##Splitter");
        ImRect bb;
        bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
        bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
        return SplitterBehavior(id, bb, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
    }
}

#endif // IMGUI_SPLITTER_IMPLEM