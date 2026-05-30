#pragma once

#include "../vendor/imgui/imgui.h"

#include <cmath>
#include <cstdio>
#include <string>
#include <unordered_map>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#ifndef ImMin
    #define ImMin(a, b) ((a) < (b) ? (a) : (b))
    #define ImMax(a, b) ((a) > (b) ? (a) : (b))
    #define ImClamp(v, lo, hi) ((v) < (lo) ? (lo) : (v) > (hi) ? (hi) : (v))
#endif

static bool boolVar = false;
static float val = 0.5f;

namespace UI {

    namespace Color {

        constexpr ImVec4 BgDeep {0.055f, 0.055f, 0.075f, 1.000f};
        constexpr ImVec4 BgBase {0.090f, 0.090f, 0.115f, 1.000f};
        constexpr ImVec4 BgElevate {0.130f, 0.130f, 0.165f, 1.000f};
        constexpr ImVec4 BgWidget {0.160f, 0.160f, 0.200f, 1.000f};

        constexpr ImVec4 Accent {0.000f, 0.820f, 0.880f, 1.000f};
        constexpr ImVec4 AccentDim {0.000f, 0.480f, 0.530f, 1.000f};
        constexpr ImVec4 AccentBright {0.450f, 1.000f, 1.000f, 1.000f};
        constexpr ImVec4 AccentGlow {0.000f, 0.820f, 0.880f, 0.120f};

        constexpr ImVec4 Danger {0.960f, 0.220f, 0.340f, 1.000f};
        constexpr ImVec4 Success {0.150f, 0.870f, 0.420f, 1.000f};
        constexpr ImVec4 Warning {1.000f, 0.730f, 0.070f, 1.000f};

        constexpr ImVec4 TextHigh {0.940f, 0.940f, 0.960f, 1.000f};
        constexpr ImVec4 TextMid {0.580f, 0.580f, 0.630f, 1.000f};
        constexpr ImVec4 TextLow {0.330f, 0.330f, 0.370f, 1.000f};
    } // namespace Color

    namespace _impl {

        static std::unordered_map<ImGuiID, float> s_Anim;

        inline float Anim(ImGuiID id, float target, float speed = 12.f) {
            float& v = s_Anim[id];
            float dt = ImGui::GetIO().DeltaTime;
            v += (target - v) * (1.f - expf(-speed * dt));
            return v;
        }

        inline ImVec4 Lerp4(ImVec4 a, ImVec4 b, float t) {
            return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t};
        }

        inline ImU32 U32(ImVec4 c) {
            return ImGui::ColorConvertFloat4ToU32(c);
        }

        inline ImVec4 Alpha(ImVec4 c, float a) {
            return {c.x, c.y, c.z, a};
        }

        inline void GlowRect(ImDrawList* dl, ImVec2 mn, ImVec2 mx, ImVec4 col, float radius = 8.f, float alpha = 0.18f) {
            for (int i = 3; i >= 1; --i) {
                float s = (float)i;
                float alf = alpha * (s / 3.f);
                dl->AddRectFilled(
                    {mn.x - s * radius * 0.25f, mn.y - s * radius * 0.25f},
                    {mx.x + s * radius * 0.25f, mx.y + s * radius * 0.25f},
                    U32(Alpha(col, alf)),
                    radius + s * 2.f
                );
            }
        }

    } // namespace _impl

    using namespace _impl;

    inline void ApplyTheme() {
        ImGuiStyle& s = ImGui::GetStyle();
        ImVec4* c = s.Colors;

        s.WindowRounding = 12.f;
        s.ChildRounding = 8.f;
        s.FrameRounding = 7.f;
        s.PopupRounding = 10.f;
        s.ScrollbarRounding = 8.f;
        s.GrabRounding = 6.f;
        s.TabRounding = 7.f;
        s.WindowBorderSize = 1.f;
        s.FrameBorderSize = 0.f;
        s.PopupBorderSize = 1.f;
        s.TabBorderSize = 0.f;

        s.WindowPadding = {16.f, 16.f};
        s.FramePadding = {11.f, 7.f};
        s.ItemSpacing = {10.f, 9.f};
        s.ItemInnerSpacing = {7.f, 5.f};
        s.ScrollbarSize = 10.f;
        s.GrabMinSize = 14.f;
        s.IndentSpacing = 18.f;

        c[ImGuiCol_WindowBg] = Color::BgDeep;
        c[ImGuiCol_ChildBg] = Color::BgBase;
        c[ImGuiCol_PopupBg] = {0.08f, 0.08f, 0.11f, 0.97f};

        c[ImGuiCol_Border] = {0.20f, 0.20f, 0.27f, 0.75f};
        c[ImGuiCol_BorderShadow] = {0, 0, 0, 0};

        c[ImGuiCol_FrameBg] = Color::BgWidget;
        c[ImGuiCol_FrameBgHovered] = {0.20f, 0.20f, 0.27f, 1.f};
        c[ImGuiCol_FrameBgActive] = {0.24f, 0.24f, 0.32f, 1.f};

        c[ImGuiCol_TitleBg] = {0.06f, 0.06f, 0.08f, 1.f};
        c[ImGuiCol_TitleBgActive] = {0.07f, 0.07f, 0.10f, 1.f};
        c[ImGuiCol_TitleBgCollapsed] = {0.06f, 0.06f, 0.08f, 0.80f};

        c[ImGuiCol_MenuBarBg] = {0.07f, 0.07f, 0.09f, 1.f};

        c[ImGuiCol_ScrollbarBg] = Alpha(Color::BgDeep, 0.60f);
        c[ImGuiCol_ScrollbarGrab] = Alpha(Color::AccentDim, 0.70f);
        c[ImGuiCol_ScrollbarGrabHovered] = Alpha(Color::Accent, 0.80f);
        c[ImGuiCol_ScrollbarGrabActive] = Color::Accent;

        c[ImGuiCol_CheckMark] = Color::Accent;
        c[ImGuiCol_SliderGrab] = Color::Accent;
        c[ImGuiCol_SliderGrabActive] = Color::AccentBright;

        c[ImGuiCol_Button] = Color::BgWidget;
        c[ImGuiCol_ButtonHovered] = {0.06f, 0.42f, 0.44f, 0.65f};
        c[ImGuiCol_ButtonActive] = {0.08f, 0.55f, 0.58f, 0.80f};

        c[ImGuiCol_Header] = Alpha(Color::Accent, 0.22f);
        c[ImGuiCol_HeaderHovered] = Alpha(Color::Accent, 0.38f);
        c[ImGuiCol_HeaderActive] = Alpha(Color::Accent, 0.55f);

        c[ImGuiCol_Separator] = {0.20f, 0.20f, 0.27f, 0.75f};
        c[ImGuiCol_SeparatorHovered] = Alpha(Color::Accent, 0.70f);
        c[ImGuiCol_SeparatorActive] = Color::Accent;

        c[ImGuiCol_ResizeGrip] = Alpha(Color::Accent, 0.18f);
        c[ImGuiCol_ResizeGripHovered] = Alpha(Color::Accent, 0.45f);
        c[ImGuiCol_ResizeGripActive] = Alpha(Color::Accent, 0.85f);

        c[ImGuiCol_Tab] = {0.09f, 0.09f, 0.12f, 0.95f};
        c[ImGuiCol_TabHovered] = Alpha(Color::Accent, 0.38f);
        c[ImGuiCol_TabActive] = {0.10f, 0.42f, 0.46f, 0.85f};
        c[ImGuiCol_TabUnfocused] = {0.07f, 0.07f, 0.09f, 0.95f};
        c[ImGuiCol_TabUnfocusedActive] = {0.09f, 0.33f, 0.36f, 0.75f};

        c[ImGuiCol_PlotLines] = Color::Accent;
        c[ImGuiCol_PlotLinesHovered] = Color::AccentBright;
        c[ImGuiCol_PlotHistogram] = Alpha(Color::Accent, 0.85f);
        c[ImGuiCol_PlotHistogramHovered] = Color::AccentBright;

        c[ImGuiCol_TableHeaderBg] = {0.09f, 0.09f, 0.12f, 1.f};
        c[ImGuiCol_TableBorderStrong] = {0.22f, 0.22f, 0.30f, 1.f};
        c[ImGuiCol_TableBorderLight] = {0.16f, 0.16f, 0.22f, 1.f};

        c[ImGuiCol_Text] = Color::TextHigh;
        c[ImGuiCol_TextDisabled] = Color::TextLow;
        c[ImGuiCol_TextSelectedBg] = Alpha(Color::Accent, 0.28f);

        c[ImGuiCol_NavHighlight] = Color::Accent;
        c[ImGuiCol_NavWindowingHighlight] = Alpha(Color::Accent, 0.70f);
        c[ImGuiCol_DragDropTarget] = Alpha(Color::AccentBright, 0.90f);
        c[ImGuiCol_ModalWindowDimBg] = {0.04f, 0.04f, 0.06f, 0.60f};
    }

    enum class ButtonVariant { Primary, Ghost, Danger };

    inline bool Button(const char* label, ImVec2 size = {0.f, 0.f}, ButtonVariant v = ButtonVariant::Primary) {
        ImGuiID id = ImGui::GetID(label);
        ImVec2 pos = ImGui::GetCursorScreenPos();

        ImVec2 textSz = ImGui::CalcTextSize(label);
        ImVec2 btnSz = {size.x > 0.f ? size.x : textSz.x + 28.f, size.y > 0.f ? size.y : 34.f};

        bool pressed = ImGui::InvisibleButton(label, btnSz);
        bool hovered = ImGui::IsItemHovered();
        bool active = ImGui::IsItemActive();

        float hT = Anim(id, hovered ? 1.f : 0.f, 11.f);
        float aT = Anim(id + 0xA000, active ? 1.f : 0.f, 22.f);

        ImVec2 mn = pos, mx = {pos.x + btnSz.x, pos.y + btnSz.y};
        float rr = 7.f;

        ImDrawList* dl = ImGui::GetWindowDrawList();

        ImVec4 accentCol = (v == ButtonVariant::Danger) ? Color::Danger : Color::Accent;

        if (hT > 0.02f)
            GlowRect(dl, mn, mx, accentCol, 10.f, hT * 0.22f);

        ImVec4 bg;
        if (v == ButtonVariant::Ghost) {
            bg = Alpha(accentCol, hT * 0.25f + aT * 0.15f);
        } else {
            bg = Lerp4(
                Color::BgWidget,
                Lerp4({0.04f, 0.38f, 0.42f, 1.f}, {0.08f, 0.52f, 0.56f, 1.f}, aT),
                hT * 0.65f + aT * 0.35f
            );
            if (v == ButtonVariant::Danger)
                bg = Lerp4(Color::BgWidget, {0.45f, 0.06f, 0.10f, 1.f}, hT * 0.65f + aT * 0.35f);
        }
        dl->AddRectFilled(mn, mx, U32(bg), rr);

        float borderA = (v == ButtonVariant::Ghost) ? 0.55f + hT * 0.35f : 0.25f + hT * 0.55f;
        dl->AddRect(mn, mx, U32(Alpha(accentCol, borderA)), rr, 0, 1.1f + hT * 0.7f);

        if (hT > 0.01f) {
            dl->AddRectFilled({mn.x + rr, mn.y}, {mx.x - rr, mn.y + 1.5f}, U32(Alpha(Color::AccentBright, hT * 0.60f)), 1.f);
        }

        float nudge = aT * 1.2f;

        ImVec4 tc = Lerp4(Color::TextHigh, Color::AccentBright, hT * 0.55f);
        dl->AddText({mn.x + (btnSz.x - textSz.x) * .5f, mn.y + (btnSz.y - textSz.y) * .5f + nudge}, U32(tc), label);

        return pressed;
    }

    inline bool Checkbox(const char* label, bool* v) {
        ImGuiID id = ImGui::GetID(label);
        ImVec2 pos = ImGui::GetCursorScreenPos();
        float sz = 18.f, rr = 4.5f;

        float rowH = ImMax(sz, ImGui::GetTextLineHeight());
        ImVec2 hitSz = {sz + 10.f + ImGui::CalcTextSize(label).x, rowH};

        ImGui::InvisibleButton(label, hitSz);
        bool clicked = ImGui::IsItemClicked();
        if (clicked)
            *v = !(*v);

        bool hovered = ImGui::IsItemHovered();
        float chkT = Anim(id, *v ? 1.f : 0.f, 14.f);
        float hovT = Anim(id + 0xB000, hovered ? 1.f : 0.f, 10.f);

        ImDrawList* dl = ImGui::GetWindowDrawList();
        float cy = pos.y + (rowH - sz) * .5f;
        ImVec2 mn = {pos.x, cy}, mx = {pos.x + sz, cy + sz};

        if (chkT > 0.02f)
            GlowRect(dl, mn, mx, Color::Accent, 8.f, chkT * 0.18f);

        ImVec4 fill = Lerp4(Lerp4(Color::BgWidget, {0.12f, 0.12f, 0.16f, 1.f}, hovT * 0.4f), {0.02f, 0.42f, 0.46f, 1.f}, chkT);
        dl->AddRectFilled(mn, mx, U32(fill), rr);

        float ba = 0.28f + chkT * 0.55f + hovT * 0.18f;
        dl->AddRect(mn, mx, U32(Alpha(Color::Accent, ba)), rr, 0, 1.2f);

        if (chkT > 0.005f) {
            ImVec2 p0 = {pos.x + sz * 0.20f, cy + sz * 0.52f};
            ImVec2 p1 = {pos.x + sz * 0.42f, cy + sz * 0.73f};
            ImVec2 p2 = {pos.x + sz * 0.80f, cy + sz * 0.24f};

            float t1 = ImMin(chkT * 2.f, 1.f);
            float t2 = ImMax((chkT - 0.5f) * 2.f, 0.f);

            if (t1 > 0.f) {
                ImVec2 ep = {p0.x + (p1.x - p0.x) * t1, p0.y + (p1.y - p0.y) * t1};
                dl->AddLine(p0, ep, U32(Color::AccentBright), 2.1f);
            }
            if (t2 > 0.f) {
                ImVec2 ep = {p1.x + (p2.x - p1.x) * t2, p1.y + (p2.y - p1.y) * t2};
                dl->AddLine(p1, ep, U32(Color::AccentBright), 2.1f);
            }
        }

        float ty = pos.y + (rowH - ImGui::GetTextLineHeight()) * .5f;
        ImVec4 tc = Lerp4(Color::TextMid, Color::TextHigh, chkT * 0.5f + hovT * 0.5f);
        dl->AddText({pos.x + sz + 9.f, ty}, U32(tc), label);

        return clicked;
    }

    inline bool Toggle(const char* label, bool* v) {
        ImGuiID id = ImGui::GetID(label);
        ImVec2 pos = ImGui::GetCursorScreenPos();
        float tw = 42.f, th = 22.f, tr = th * .5f;

        float rowH = ImMax(th, ImGui::GetTextLineHeight());
        float labelW = ImGui::CalcTextSize(label).x;
        ImVec2 hitSz = {tw + 10.f + labelW, rowH};

        ImGui::InvisibleButton(label, hitSz);
        bool clicked = ImGui::IsItemClicked();
        if (clicked)
            *v = !(*v);

        bool hovered = ImGui::IsItemHovered();
        float tT = Anim(id, *v ? 1.f : 0.f, 14.f);
        float hovT = Anim(id + 0xC000, hovered ? 1.f : 0.f, 9.f);

        ImDrawList* dl = ImGui::GetWindowDrawList();
        float cy = pos.y + (rowH - th) * .5f;
        ImVec2 mn = {pos.x, cy}, mx = {pos.x + tw, cy + th};

        if (tT > 0.02f)
            GlowRect(dl, mn, mx, Color::Accent, 10.f, tT * 0.22f);

        ImVec4 trackOff = Color::BgWidget;
        ImVec4 trackOn = {0.02f, 0.40f, 0.44f, 1.f};
        ImVec4 track = Lerp4(trackOff, trackOn, tT);
        track = Lerp4(track, Lerp4({0.17f, 0.17f, 0.22f, 1.f}, {0.04f, 0.50f, 0.54f, 1.f}, tT), hovT * 0.28f);
        dl->AddRectFilled(mn, mx, U32(track), tr);

        dl->AddRect(mn, mx, U32(Alpha(Color::Accent, 0.22f + tT * 0.50f + hovT * 0.14f)), tr, 0, 1.1f);

        float pad = 3.2f, kr = tr - pad;
        float kx = pos.x + tr + (tw - th) * tT;
        float ky = cy + tr;

        dl->AddCircleFilled({kx + 1.f, ky + 1.f}, kr, IM_COL32(0, 0, 0, 80));

        ImVec4 knobOff = {0.50f, 0.50f, 0.56f, 1.f};
        dl->AddCircleFilled({kx, ky}, kr, U32(Lerp4(knobOff, Color::AccentBright, tT)));

        if (tT > 0.02f)
            dl->AddCircle({kx, ky}, kr + 3.f, U32(Alpha(Color::Accent, tT * 0.38f)), 0, 1.2f);

        float ty = pos.y + (rowH - ImGui::GetTextLineHeight()) * .5f;
        ImVec4 tc = Lerp4(Color::TextMid, Color::TextHigh, tT * 0.45f + hovT * 0.55f);
        dl->AddText({pos.x + tw + 9.f, ty}, U32(tc), label);

        return clicked;
    }

    inline bool SliderFloat(const char* label, float* v, float vMin, float vMax, const char* fmt = "%.2f", float height = 34.f) {
        ImGuiID id = ImGui::GetID(label);
        ImVec2 pos = ImGui::GetCursorScreenPos();
        float w = ImGui::GetContentRegionAvail().x;

        ImGui::InvisibleButton(label, {w, height});
        bool hovered = ImGui::IsItemHovered();
        bool active = ImGui::IsItemActive();

        if (active) {
            float mx = ImGui::GetIO().MousePos.x;
            float t = (mx - pos.x) / w;
            *v = vMin + (vMax - vMin) * ImClamp(t, 0.f, 1.f);
        }

        float hovT = Anim(id, hovered ? 1.f : 0.f, 10.f);
        float actT = Anim(id + 0xD000, active ? 1.f : 0.f, 18.f);

        float frac = ImClamp((*v - vMin) / (vMax - vMin), 0.f, 1.f);

        ImDrawList* dl = ImGui::GetWindowDrawList();

        float labelY = pos.y + 3.f;
        ImVec4 labelTc = Lerp4(Color::TextMid, Color::TextHigh, hovT * 0.6f + actT * 0.4f);
        dl->AddText({pos.x, labelY}, U32(labelTc), label);

        char valBuf[32];
        snprintf(valBuf, sizeof(valBuf), fmt, *v);
        float valW = ImGui::CalcTextSize(valBuf).x;
        ImVec4 valTc = Lerp4(Color::TextMid, Color::AccentBright, actT * 0.80f + hovT * 0.30f);
        dl->AddText({pos.x + w - valW, labelY}, U32(valTc), valBuf);

        float trackH = 5.f;
        float trackY = pos.y + height - trackH - 4.f;
        float trackR = trackH * .5f;
        ImVec2 tmn = {pos.x, trackY};
        ImVec2 tmx = {pos.x + w, trackY + trackH};

        dl->AddRectFilled(tmn, tmx, U32(Color::BgWidget), trackR);
        dl->AddRect(tmn, tmx, U32(Alpha(Color::Accent, 0.18f + hovT * 0.12f)), trackR, 0, 0.8f);

        float fillX = pos.x + w * frac;
        if (fillX - pos.x > trackR * 2.f) {
            ImVec4 fillC = Lerp4(Color::AccentDim, Color::Accent, hovT * 0.55f + actT * 0.45f);
            dl->AddRectFilled(tmn, {fillX, trackY + trackH}, U32(fillC), trackR);

            float shimT = fmodf((float)ImGui::GetTime() * 1.6f, 2.0f);
            float shimNX = (shimT - 0.3f) * (fillX - pos.x);
            float shimW = (fillX - pos.x) * 0.18f;
            dl->PushClipRect(tmn, {fillX, trackY + trackH}, true);
            dl->AddRectFilled(
                {pos.x + shimNX, trackY},
                {pos.x + shimNX + shimW, trackY + trackH},
                U32(Alpha(Color::AccentBright, 0.22f)),
                trackR
            );
            dl->PopClipRect();
        }

        float grabR = 7.5f + actT * 2.2f + hovT * 1.4f;
        float grabX = pos.x + frac * w;
        float grabY = trackY + trackH * .5f;

        if (hovT > 0.01f || actT > 0.01f)
            GlowRect(
                dl,
                {grabX - grabR, grabY - grabR},
                {grabX + grabR, grabY + grabR},
                Color::Accent,
                grabR * 1.5f,
                (hovT * 0.20f + actT * 0.28f)
            );

        dl->AddCircleFilled({grabX + 1.f, grabY + 1.5f}, grabR, IM_COL32(0, 0, 0, 70));

        ImVec4 grabC = Lerp4(Color::Accent, Color::AccentBright, actT * 0.75f + hovT * 0.25f);
        dl->AddCircleFilled({grabX, grabY}, grabR, U32(grabC));

        float ringA = hovT * 0.38f + actT * 0.55f;
        if (ringA > 0.01f)
            dl->AddCircle({grabX, grabY}, grabR + 3.5f, U32(Alpha(Color::Accent, ringA)), 0, 1.3f);

        dl->AddCircleFilled(
            {grabX - grabR * 0.28f, grabY - grabR * 0.28f},
            grabR * 0.26f,
            U32(Alpha(Color::AccentBright, 0.38f + hovT * 0.28f))
        );

        return active;
    }

    inline bool SliderInt(const char* label, int* v, int vMin, int vMax) {
        float fv = static_cast<float>(*v);
        bool ch = SliderFloat(label, &fv, static_cast<float>(vMin), static_cast<float>(vMax), "%.0f");
        *v = static_cast<int>(fv);
        return ch;
    }

    inline void SectionHeader(const char* text, ImVec4 accent = Color::Accent) {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        float avail = ImGui::GetContentRegionAvail().x;
        float lh = ImGui::GetTextLineHeight();

        ImDrawList* dl = ImGui::GetWindowDrawList();

        dl->AddRectFilled({pos.x, pos.y + 2.f}, {pos.x + 3.f, pos.y + lh - 2.f}, U32(accent), 2.f);

        dl->AddText({pos.x + 10.f, pos.y}, U32(Color::TextHigh), text);

        float textRight = pos.x + 14.f + ImGui::CalcTextSize(text).x;
        float ruleY = pos.y + lh * 0.5f;
        dl->AddLine({textRight + 8.f, ruleY}, {pos.x + avail, ruleY}, U32(Alpha(accent, 0.18f)), 1.f);

        ImGui::Dummy({avail, lh + 8.f});
    }

    inline void ProgressBar(float fraction, ImVec2 size = {-1.f, 6.f}, ImVec4 fillColor = Color::Accent) {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        float w = size.x < 0.f ? ImGui::GetContentRegionAvail().x : size.x;
        float h = size.y;
        float r = h * .5f;
        fraction = ImClamp(fraction, 0.f, 1.f);

        ImDrawList* dl = ImGui::GetWindowDrawList();

        dl->AddRectFilled(pos, {pos.x + w, pos.y + h}, U32(Color::BgWidget), r);
        dl->AddRect(pos, {pos.x + w, pos.y + h}, U32(Alpha(fillColor, 0.20f)), r, 0, 0.8f);

        float fw = w * fraction;
        if (fw > r * 2.f) {
            dl->AddRectFilled(pos, {pos.x + fw, pos.y + h}, U32(Alpha(fillColor, 0.85f)), r);

            float st = fmodf((float)ImGui::GetTime() * 1.5f, 2.2f);
            float sx = pos.x + (st - 0.3f) * fw;
            float sw = fw * 0.15f;
            dl->PushClipRect(pos, {pos.x + fw, pos.y + h}, true);
            dl->AddRectFilled({sx, pos.y}, {sx + sw, pos.y + h}, U32(Alpha(Color::AccentBright, 0.22f)), r);
            dl->PopClipRect();

            dl->AddRectFilled({pos.x + fw - 3.f, pos.y}, {pos.x + fw + 2.f, pos.y + h}, U32(Alpha(fillColor, 0.55f)), 1.f);
        }

        ImGui::Dummy({w, h});
    }

    inline void Badge(const char* text, ImVec4 color = Color::Accent) {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 tsz = ImGui::CalcTextSize(text);
        float px = 7.f, py = 2.5f;
        ImVec2 mn = pos;
        ImVec2 mx = {pos.x + tsz.x + px * 2.f, pos.y + tsz.y + py * 2.f};

        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->AddRectFilled(mn, mx, U32(Alpha(color, 0.16f)), (mx.y - mn.y) * .5f);
        dl->AddRect(mn, mx, U32(Alpha(color, 0.48f)), (mx.y - mn.y) * .5f, 0, 1.f);
        dl->AddText({pos.x + px, pos.y + py}, U32(color), text);

        ImGui::Dummy({mx.x - mn.x, mx.y - mn.y});
    }

    inline void Keybind(const char* key) {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 tsz = ImGui::CalcTextSize(key);
        float px = 8.f, py = 3.f;
        ImVec2 mn = pos;
        ImVec2 mx = {pos.x + tsz.x + px * 2.f, pos.y + tsz.y + py * 2.f};
        float rr = 5.f;

        ImDrawList* dl = ImGui::GetWindowDrawList();

        dl->AddRectFilled(mn, mx, U32(Color::BgElevate), rr);

        dl->AddRectFilled({mn.x + 1.f, mx.y - 2.f}, {mx.x - 1.f, mx.y + 1.5f}, IM_COL32(0, 0, 0, 80), rr);

        dl->AddRect(mn, mx, U32(Alpha(Color::TextMid, 0.35f)), rr, 0, 1.f);

        dl->AddText({pos.x + px, pos.y + py}, U32(Color::TextMid), key);

        ImGui::Dummy({mx.x - mn.x, mx.y - mn.y + 2.f});
    }

    inline void StatusDot(const char* label, ImVec4 dotColor = Color::Success) {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        float r = 4.5f;
        float lh = ImGui::GetTextLineHeight();
        float cy = pos.y + lh * .5f;

        ImDrawList* dl = ImGui::GetWindowDrawList();

        dl->AddCircleFilled({pos.x + r, cy}, r + 2.5f, U32(Alpha(dotColor, 0.18f)));

        dl->AddCircleFilled({pos.x + r, cy}, r, U32(dotColor));

        dl->AddText({pos.x + r * 2.f + 7.f, pos.y}, U32(Color::TextMid), label);

        ImGui::Dummy({r * 2.f + 7.f + ImGui::CalcTextSize(label).x, lh});
    }

    inline void Separator(ImVec4 color = Color::Accent, float alpha = 0.18f) {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        float w = ImGui::GetContentRegionAvail().x;
        float cy = pos.y + 3.f;

        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->AddLine({pos.x, cy}, {pos.x + w, cy}, U32(Alpha(color, alpha)), 1.f);

        ImGui::Dummy({w, 6.f});
    }

    // ─────────────────────────────────────────────────────────────────────────
    //  Toast — 浮动提示，定位于传入的 ImGui 窗口内部右下角
    //  调用方式：s_toast.Render(winPos, winSize);
    // ─────────────────────────────────────────────────────────────────────────
    struct Toast {
        char msg[256] = {};
        float timer   = 0.f;
        ImVec4 color  = Color::Accent;
        float maxTime = 2.5f;

        void Show(const char* message, float duration = 2.5f, ImVec4 c = Color::Accent) {
            snprintf(msg, sizeof(msg), "%s", message);
            timer   = duration;
            maxTime = duration;
            color   = c;
        }

        // winPos / winSize: 调用方传入当前 ImGui 窗口的位置与尺寸（由
        // ImGui::GetWindowPos() / ImGui::GetWindowSize() 取得）。
        // Toast 将绘制在该窗口内部的右下角，不会跑到屏幕其他地方。
        void Render(ImVec2 winPos, ImVec2 winSize) {
            if (timer <= 0.f)
                return;
            timer -= ImGui::GetIO().DeltaTime;

            float lifeT = timer / maxTime;
            float alpha = lifeT < 0.2f ? lifeT / 0.2f : 1.f;

            // Toast 尺寸 & 内边距
            float toastW = ImMin(280.f, winSize.x - 32.f);
            float toastH = 44.f;
            float pad    = 14.f;

            // 右下角坐标，在窗口内部
            float bx = winPos.x + winSize.x - toastW - pad;
            float by = winPos.y + winSize.y - toastH - pad;

            // 入场滑动（从右侧滑入）
            float slideX = (1.f - ImMin(1.f, (maxTime - timer) / 0.25f)) * 50.f;
            bx += slideX;

            // 绘制到前景层（保证总在最上面）
            ImDrawList* fg = ImGui::GetForegroundDrawList();

            // 阴影
            fg->AddRectFilled(
                {bx + 2.f, by + 2.f},
                {bx + toastW + 2.f, by + toastH + 2.f},
                IM_COL32(0, 0, 0, (int)(60 * alpha)),
                9.f
            );

            // 背景
            fg->AddRectFilled(
                {bx, by},
                {bx + toastW, by + toastH},
                U32(Alpha({0.08f, 0.08f, 0.11f, 1.f}, alpha * 0.97f)),
                9.f
            );

            // 左侧彩色竖线
            fg->AddRectFilled(
                {bx, by + 6.f},
                {bx + 3.5f, by + toastH - 6.f},
                U32(Alpha(color, alpha)),
                2.f
            );

            // 边框
            fg->AddRect(
                {bx, by},
                {bx + toastW, by + toastH},
                U32(Alpha(color, alpha * 0.35f)),
                9.f, 0, 1.f
            );

            // 文字（居中纵向）
            ImVec2 tsz = ImGui::CalcTextSize(msg);
            fg->AddText(
                {bx + 14.f, by + (toastH - tsz.y) * 0.5f},
                U32(Alpha(Color::TextHigh, alpha)),
                msg
            );
        }
    };

    inline bool BeginCombo(const char* label, const char* previewValue) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {10.f, 7.f});
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, U32(Color::BgWidget));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, U32({0.18f, 0.18f, 0.24f, 1.f}));
        ImGui::PushStyleColor(ImGuiCol_Button, U32(Color::BgWidget));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, U32(Alpha(Color::Accent, 0.28f)));
        ImGui::PushStyleColor(ImGuiCol_PopupBg, U32({0.08f, 0.08f, 0.12f, 0.98f}));
        bool open = ImGui::BeginCombo(label, previewValue);
        ImGui::PopStyleColor(5);
        ImGui::PopStyleVar(2);
        return open;
    }

    inline void EndCombo() {
        ImGui::EndCombo();
    }

    inline bool InputText(const char* label, char* buf, size_t bufSize, ImGuiInputTextFlags flags = 0) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {10.f, 7.f});
        ImGui::PushStyleColor(ImGuiCol_FrameBg, U32(Color::BgWidget));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, U32({0.19f, 0.19f, 0.25f, 1.f}));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, U32({0.04f, 0.35f, 0.38f, 0.35f}));
        bool ch = ImGui::InputText(label, buf, bufSize, flags);
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);
        return ch;
    }

    inline bool BeginPanel(const char* strId, ImVec2 size = {0.f, 0.f}) {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, U32(Color::BgBase));
        ImGui::PushStyleColor(ImGuiCol_Border, U32(Alpha(Color::Accent, 0.22f)));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.f);
        bool vis = ImGui::BeginChild(strId, size, true);
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
        return vis;
    }

    inline void EndPanel() {
        ImGui::EndChild();
    }

} // namespace UI

namespace UI {

    struct TabBar {
        struct Tab {
            const char* label = "";
            ImVec4 accent = {0.f, 0.f, 0.f, 0.f};
        };

        std::vector<Tab> tabs;
        const char* strId = "##tabbar";
        float height = 40.f;

        explicit TabBar(const char* id, float h = 40.f) : strId(id), height(h) {}

        TabBar& Add(const char* label, ImVec4 accent = {0.f, 0.f, 0.f, 0.f}) {
            tabs.push_back({label, accent});
            return *this;
        }

        bool Render(int& selected) {
            if (tabs.empty())
                return false;

            if (selected < 0)
                selected = 0;
            if (selected >= (int)tabs.size())
                selected = (int)tabs.size() - 1;

            ImVec2 pos = ImGui::GetCursorScreenPos();
            float avail = ImGui::GetContentRegionAvail().x;
            ImDrawList* dl = ImGui::GetWindowDrawList();
            bool changed = false;

            ImVec4 barTop = {0.075f, 0.075f, 0.100f, 1.f};
            ImVec4 barBot = {0.065f, 0.065f, 0.088f, 1.f};
            dl->AddRectFilledMultiColor(pos, {pos.x + avail, pos.y + height}, U32(barTop), U32(barTop), U32(barBot), U32(barBot));

            const int n = (int)tabs.size();
            const float tw = avail / (float)n;

            ImGui::SetCursorScreenPos(pos);

            for (int i = 0; i < n; ++i) {
                const Tab& tab = tabs[i];
                ImVec4 ac = (tab.accent.w > 0.01f) ? tab.accent : Color::Accent;

                float tx0 = pos.x + i * tw;
                float tx1 = tx0 + tw;

                ImGuiID tid = ImGui::GetID(tab.label);
                ImGui::SetCursorScreenPos({tx0, pos.y});
                ImGui::InvisibleButton(tab.label, {tw, height});

                bool hov = ImGui::IsItemHovered();
                bool clk = ImGui::IsItemClicked();
                if (clk && selected != i) {
                    selected = i;
                    changed = true;
                }

                float hT = Anim(tid, hov ? 1.f : 0.f, 9.f);
                float selT = Anim(tid + 0xE000, (selected == i) ? 1.f : 0.f, 17.f);

                float bgAlpha = hT * 0.09f + selT * 0.07f;
                if (bgAlpha > 0.005f)
                    dl->AddRectFilled({tx0, pos.y}, {tx1, pos.y + height}, U32(Alpha(ac, bgAlpha)));

                if (i < n - 1)
                    dl->AddLine(
                        {tx1, pos.y + height * 0.18f},
                        {tx1, pos.y + height * 0.82f},
                        U32(Alpha(Color::TextLow, 0.22f + hT * 0.08f))
                    );

                ImVec2 tsz = ImGui::CalcTextSize(tab.label);
                float lx = tx0 + (tw - tsz.x) * 0.5f;
                float ly = pos.y + (height - tsz.y) * 0.5f;

                ImVec4 tc = Lerp4(Color::TextLow, Lerp4(Color::TextHigh, ac, selT * 0.45f), selT * 0.8f + hT * 0.5f);
                dl->AddText({lx, ly}, U32(tc), tab.label);
            }

            float sepY = pos.y + height - 0.5f;
            dl->AddLine({pos.x, sepY}, {pos.x + avail, sepY}, U32(Alpha(Color::Accent, 0.12f)));

            {
                ImVec4 ac = (tabs[selected].accent.w > 0.01f) ? tabs[selected].accent : Color::Accent;

                float tgtX = selected * tw;
                float tgtW = tw;

                if (!_init) {
                    _indX = tgtX;
                    _indW = tgtW;
                    _init = true;
                } else {
                    float dt = ImGui::GetIO().DeltaTime;
                    float spd = 1.f - expf(-20.f * dt);
                    _indX += (tgtX - _indX) * spd;
                    _indW += (tgtW - _indW) * spd;
                }

                float ix = pos.x + _indX;
                float iw = _indW;
                float iy = pos.y + height - 2.5f;
                float inset = iw * 0.08f;
                float iRad = 2.f;

                dl->AddRectFilled(
                    {ix + inset - 6.f, iy - 4.f},
                    {ix + iw - inset + 6.f, iy + 4.5f},
                    U32(Alpha(ac, 0.20f)),
                    iRad + 3.f
                );

                dl->AddRectFilled({ix + inset, iy}, {ix + iw - inset, iy + 2.5f}, U32(Alpha(ac, 0.92f)), iRad);

                float cx = ix + iw * 0.5f;
                float hw = (iw - inset * 2.f) * 0.30f;
                dl->AddRectFilled({cx - hw, iy}, {cx + hw, iy + 2.5f}, U32(Alpha(Color::AccentBright, 0.60f)), iRad);
            }

            ImGui::SetCursorScreenPos({pos.x, pos.y + height + 8.f});
            return changed;
        }

      private:
        float _indX = 0.f, _indW = 0.f;
        bool _init = false;
    };

    inline void InfoTip(const char* tipText, float radius = 8.5f, ImVec4 color = Color::Accent) {
        ImGuiID id = ImGui::GetID(tipText);
        char btnId[24];
        snprintf(btnId, sizeof(btnId), "##tip%u", id);

        ImVec2 pos = ImGui::GetCursorScreenPos();
        float r = radius;
        ImVec2 c = {pos.x + r, pos.y + r};
        ImDrawList* dl = ImGui::GetWindowDrawList();

        ImGui::InvisibleButton(btnId, {r * 2.f, r * 2.f});
        bool hov = ImGui::IsItemHovered();
        float hT = Anim(id + 0xF100, hov ? 1.f : 0.f, 14.f);

        if (hT > 0.01f)
            dl->AddCircleFilled(c, r + 6.f, U32(Alpha(color, hT * 0.13f)));

        ImVec4 bg = Lerp4(Color::BgWidget, Lerp4({0.02f, 0.30f, 0.34f, 1.f}, color, 0.45f), hT);
        dl->AddCircleFilled(c, r, U32(bg));

        dl->AddCircle(c, r, U32(Alpha(color, 0.32f + hT * 0.58f)), 0, 1.3f);

        {
            ImVec4 ic = Lerp4(Color::TextMid, Color::AccentBright, hT * 0.85f);
            ImU32 icu = U32(ic);
            float dotR = r * 0.155f;
            float stemW = r * 0.145f;

            dl->AddCircleFilled({c.x, c.y - r * 0.34f}, dotR, icu);

            dl->AddRectFilled({c.x - stemW, c.y - r * 0.10f}, {c.x + stemW, c.y + r * 0.44f}, icu, stemW);
        }

        if (hov) {
            float alpha = ImClamp(hT * 6.f, 0.05f, 1.f);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {15.f, 12.f});
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 9.f);
            ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1.f);
            ImGui::PushStyleColor(ImGuiCol_PopupBg, U32(Alpha({0.04f, 0.05f, 0.08f, 1.f}, alpha * 0.97f)));
            ImGui::PushStyleColor(ImGuiCol_Border, U32(Alpha(color, alpha * 0.42f)));
            ImGui::PushStyleColor(ImGuiCol_Text, U32(Alpha(Color::TextHigh, alpha)));

            ImGui::SetNextWindowSizeConstraints({80.f, 0.f}, {300.f, 400.f});
            ImGui::BeginTooltip();

            {
                ImVec2 wp = ImGui::GetWindowPos();
                ImVec2 ws = ImGui::GetWindowSize();
                float barT = wp.y + 7.f;
                float barB = wp.y + ws.y - 7.f;
                ImGui::GetWindowDrawList()
                    ->AddRectFilled({wp.x + 1.5f, barT}, {wp.x + 4.5f, barB}, U32(Alpha(color, alpha * 0.88f)), 2.f);
            }

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8.f);
            ImGui::TextUnformatted(tipText);

            ImGui::EndTooltip();
            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(3);
        }
    }

    struct KeybindRecorder {
        ImGuiKey mainKey = ImGuiKey_None;
        bool ctrl = false;
        bool shift = false;
        bool alt = false;
        bool recording = false;

        bool IsSet() const {
            return mainKey != ImGuiKey_None;
        }

        void Clear() {
            mainKey = ImGuiKey_None;
            ctrl = shift = alt = false;
        }

        std::string GetDisplayString() const {
            if (!IsSet())
                return "Not bound";
            std::string s;
            if (ctrl)
                s += "Ctrl+";
            if (shift)
                s += "Shift+";
            if (alt)
                s += "Alt+";
            s += _KeyName(mainKey);
            return s;
        }

        bool Render(const char* label, float rowHeight = 36.f) {
            ImGuiID id = ImGui::GetID(label);
            ImVec2 pos = ImGui::GetCursorScreenPos();
            float avail = ImGui::GetContentRegionAvail().x;
            ImDrawList* dl = ImGui::GetWindowDrawList();
            bool changed = false;

            float dt = ImGui::GetIO().DeltaTime;
            _pulseT += dt * 3.4f;
            if (_flashT > 0.f)
                _flashT = ImMax(0.f, _flashT - dt * 3.2f);
            if (_clearFlT > 0.f)
                _clearFlT = ImMax(0.f, _clearFlT - dt * 3.2f);

            ImGui::InvisibleButton(label, {avail, rowHeight});
            bool rowHov = ImGui::IsItemHovered();
            bool rowClk = ImGui::IsItemClicked();

            float hovT = Anim(id, rowHov ? 1.f : 0.f, 9.f);
            float recT = Anim(id + 0xF200, recording ? 1.f : 0.f, 13.f);

            if (rowClk && !recording) {
                _savedKey = mainKey;
                _savedCtrl = ctrl;
                _savedShift = shift;
                _savedAlt = alt;
                recording = true;
            }

            if (recording) {
                bool kC = ImGui::GetIO().KeyCtrl;
                bool kS = ImGui::GetIO().KeyShift;
                bool kA = ImGui::GetIO().KeyAlt;

                if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
                    mainKey = _savedKey;
                    ctrl = _savedCtrl;
                    shift = _savedShift;
                    alt = _savedAlt;
                    recording = false;
                } else if (ImGui::IsKeyPressed(ImGuiKey_Backspace, false) || ImGui::IsKeyPressed(ImGuiKey_Delete, false)) {
                    Clear();
                    recording = false;
                    changed = true;
                    _clearFlT = 1.f;
                } else {
                    static const ImGuiKey kTable[] = {
                        ImGuiKey_A, ImGuiKey_B, ImGuiKey_C, ImGuiKey_D, ImGuiKey_E,
                        ImGuiKey_F, ImGuiKey_G, ImGuiKey_H, ImGuiKey_I, ImGuiKey_J,
                        ImGuiKey_K, ImGuiKey_L, ImGuiKey_M, ImGuiKey_N, ImGuiKey_O,
                        ImGuiKey_P, ImGuiKey_Q, ImGuiKey_R, ImGuiKey_S, ImGuiKey_T,
                        ImGuiKey_U, ImGuiKey_V, ImGuiKey_W, ImGuiKey_X, ImGuiKey_Y,
                        ImGuiKey_Z,
                        ImGuiKey_0, ImGuiKey_1, ImGuiKey_2, ImGuiKey_3, ImGuiKey_4,
                        ImGuiKey_5, ImGuiKey_6, ImGuiKey_7, ImGuiKey_8, ImGuiKey_9,
                        ImGuiKey_F1,  ImGuiKey_F2,  ImGuiKey_F3,  ImGuiKey_F4,
                        ImGuiKey_F5,  ImGuiKey_F6,  ImGuiKey_F7,  ImGuiKey_F8,
                        ImGuiKey_F9,  ImGuiKey_F10, ImGuiKey_F11, ImGuiKey_F12,
                        ImGuiKey_Tab, ImGuiKey_Space, ImGuiKey_Enter,
                        ImGuiKey_Insert, ImGuiKey_Home, ImGuiKey_End,
                        ImGuiKey_PageUp, ImGuiKey_PageDown,
                        ImGuiKey_LeftArrow, ImGuiKey_RightArrow,
                        ImGuiKey_UpArrow,   ImGuiKey_DownArrow,
                        ImGuiKey_Minus, ImGuiKey_Equal,
                        ImGuiKey_LeftBracket, ImGuiKey_RightBracket,
                        ImGuiKey_Backslash, ImGuiKey_Semicolon, ImGuiKey_Apostrophe,
                        ImGuiKey_GraveAccent, ImGuiKey_Comma, ImGuiKey_Period, ImGuiKey_Slash,
                        ImGuiKey_Keypad0, ImGuiKey_Keypad1, ImGuiKey_Keypad2,
                        ImGuiKey_Keypad3, ImGuiKey_Keypad4, ImGuiKey_Keypad5,
                        ImGuiKey_Keypad6, ImGuiKey_Keypad7, ImGuiKey_Keypad8, ImGuiKey_Keypad9,
                        ImGuiKey_KeypadAdd, ImGuiKey_KeypadSubtract,
                        ImGuiKey_KeypadMultiply, ImGuiKey_KeypadDivide,
                        ImGuiKey_KeypadDecimal, ImGuiKey_KeypadEnter,
                        ImGuiKey_PrintScreen, ImGuiKey_Pause,
                        ImGuiKey_CapsLock, ImGuiKey_NumLock, ImGuiKey_ScrollLock,
                    };

                    for (ImGuiKey k : kTable) {
                        if (ImGui::IsKeyPressed(k, false)) {
                            mainKey = k;
                            ctrl = kC;
                            shift = kS;
                            alt = kA;
                            recording = false;
                            changed = true;
                            _flashT = 1.f;
                            break;
                        }
                    }
                }
            }

            {
                float bgBlend = hovT * 0.35f + recT * 0.55f;
                ImVec4 rowBg = Lerp4(Alpha(Color::BgWidget, 0.50f), Alpha(Color::Accent, 0.11f), bgBlend);

                if (_flashT > 0.f)
                    rowBg = Lerp4(rowBg, Alpha(Color::AccentBright, 0.15f), _flashT);
                if (_clearFlT > 0.f)
                    rowBg = Lerp4(rowBg, Alpha(Color::Danger, 0.14f), _clearFlT);

                dl->AddRectFilled(pos, {pos.x + avail, pos.y + rowHeight}, U32(rowBg), 7.f);
            }

            if (recT > 0.01f) {
                float pulse = sinf(_pulseT) * 0.5f + 0.5f;
                float ba = 0.28f + pulse * 0.48f * recT;
                dl->AddRect(pos, {pos.x + avail, pos.y + rowHeight}, U32(Alpha(Color::Accent, ba)), 7.f, 0, 1.3f);

                dl->AddRectFilled(
                    {pos.x + 8.f, pos.y},
                    {pos.x + avail - 8.f, pos.y + 1.5f},
                    U32(Alpha(Color::AccentBright, pulse * recT * 0.35f)),
                    1.f
                );
            } else if (hovT > 0.01f) {
                dl->AddRect(pos, {pos.x + avail, pos.y + rowHeight}, U32(Alpha(Color::Accent, hovT * 0.24f)), 7.f, 0, 0.9f);
            }

            dl->AddRectFilled(
                {pos.x, pos.y + 8.f},
                {pos.x + 3.f, pos.y + rowHeight - 8.f},
                U32(Alpha(Color::Accent, 0.28f + hovT * 0.45f + recT * 0.35f)),
                2.f
            );

            float textY = pos.y + (rowHeight - ImGui::GetTextLineHeight()) * 0.5f;
            ImVec4 labelC = Lerp4(Color::TextMid, Color::TextHigh, hovT * 0.55f + recT * 0.45f);
            dl->AddText({pos.x + 12.f, textY}, U32(labelC), label);

            float rEdge = pos.x + avail - 10.f;
            float chipY = pos.y + (rowHeight - (ImGui::GetTextLineHeight() + 5.f)) * 0.5f;

            if (recording) {
                float pulse2 = sinf(_pulseT) * 0.5f + 0.5f;

                const char* listenTxt = "Press any key\xe2\x80\xa6";
                ImVec4 listenC = Lerp4(Color::AccentDim, Color::AccentBright, pulse2);
                ImVec2 ltsz = ImGui::CalcTextSize(listenTxt);
                float escW = ImGui::CalcTextSize("Esc").x + 16.f;
                float ltX = rEdge - escW - 8.f - ltsz.x;
                dl->AddText({ltX, textY}, U32(listenC), listenTxt);

                _DrawChip(dl, {rEdge - escW + 2.f, chipY}, "Esc", Alpha(Color::Warning, 0.90f));

            } else {
                if (IsSet()) {
                    float fa = _flashT;

                    std::string mkStr = _KeyName(mainKey);
                    ImVec4 mainC = Lerp4(Color::Accent, Color::AccentBright, fa * 0.70f);
                    ImVec4 modC = Lerp4(Color::AccentDim, Color::Accent, fa * 0.70f);

                    float cx = rEdge;

                    cx = _DrawChipR(dl, cx, chipY, mkStr.c_str(), mainC);

                    bool hasAnyMod = ctrl || shift || alt;

                    if (hasAnyMod) {
                        cx = _PlusR(dl, cx, textY);
                    }
                    if (alt) {
                        cx = _DrawChipR(dl, cx, chipY, "Alt", modC);
                        if (ctrl || shift)
                            cx = _PlusR(dl, cx, textY);
                    }
                    if (shift) {
                        cx = _DrawChipR(dl, cx, chipY, "Shift", modC);
                        if (ctrl)
                            cx = _PlusR(dl, cx, textY);
                    }
                    if (ctrl) {
                        _DrawChipR(dl, cx, chipY, "Ctrl", modC);
                    }

                } else {
                    float notBoundA = 0.38f + hovT * 0.22f;

                    const char* nbTxt = _clearFlT > 0.f ? "Cleared" : "Not bound  \xe2\x80\x94  click to record";
                    ImVec4 nbC = _clearFlT > 0.f ? Lerp4(Color::TextLow, Color::Danger, _clearFlT * 0.75f)
                                                 : Alpha(Color::TextLow, notBoundA);
                    ImVec2 nbsz = ImGui::CalcTextSize(nbTxt);
                    dl->AddText({rEdge - nbsz.x, textY}, U32(nbC), nbTxt);
                }
            }

            return changed;
        }

      private:
        float _pulseT = 0.f;
        float _flashT = 0.f;
        float _clearFlT = 0.f;

        ImGuiKey _savedKey = ImGuiKey_None;
        bool _savedCtrl = false;
        bool _savedShift = false;
        bool _savedAlt = false;

        static void _DrawChip(ImDrawList* dl, ImVec2 pos, const char* txt, ImVec4 color) {
            ImVec2 tsz = ImGui::CalcTextSize(txt);
            float px = 7.f, py = 2.5f, rr = 4.f;
            ImVec2 mn = pos;
            ImVec2 mx = {pos.x + tsz.x + px * 2.f, pos.y + tsz.y + py * 2.f};

            dl->AddRectFilled(mn, mx, U32(Alpha(color, 0.18f)), rr);
            dl->AddRectFilled({mn.x + 1.f, mx.y - 1.8f}, {mx.x - 1.f, mx.y + 2.2f}, IM_COL32(0, 0, 0, 55), rr);
            dl->AddRect(mn, mx, U32(Alpha(color, 0.58f)), rr, 0, 1.f);
            dl->AddText({pos.x + px, pos.y + py}, U32(Alpha(color, 1.f)), txt);
        }

        static float _DrawChipR(ImDrawList* dl, float rightX, float y, const char* txt, ImVec4 color) {
            ImVec2 tsz = ImGui::CalcTextSize(txt);
            float px = 7.f, py = 2.5f, rr = 4.f;
            float chipW = tsz.x + px * 2.f;
            float chipH = tsz.y + py * 2.f;
            ImVec2 mn = {rightX - chipW, y};
            ImVec2 mx = {rightX, y + chipH};

            dl->AddRectFilled(mn, mx, U32(Alpha(color, 0.18f)), rr);
            dl->AddRectFilled({mn.x + 1.f, mx.y - 1.8f}, {mx.x - 1.f, mx.y + 2.2f}, IM_COL32(0, 0, 0, 55), rr);
            dl->AddRect(mn, mx, U32(Alpha(color, 0.58f)), rr, 0, 1.f);
            dl->AddText({mn.x + px, mn.y + py}, U32(Alpha(color, 1.f)), txt);

            return mn.x;
        }

        static float _PlusR(ImDrawList* dl, float rightX, float textY) {
            const char* p = "+";
            ImVec2 psz = ImGui::CalcTextSize(p);
            float gap = 5.f;
            float px = rightX - gap - psz.x;
            dl->AddText({px, textY}, U32(Alpha(Color::TextLow, 0.70f)), p);
            return px - gap;
        }

        static std::string _KeyName(ImGuiKey k) {
#if defined(IMGUI_VERSION_NUM) && IMGUI_VERSION_NUM >= 18700
            const char* n = ImGui::GetKeyName(k);
            if (n && n[0] && n[0] != '0')
                return n;
#endif
            switch (k) {
                case ImGuiKey_Tab:         return "Tab";
                case ImGuiKey_LeftArrow:   return "Left";
                case ImGuiKey_RightArrow:  return "Right";
                case ImGuiKey_UpArrow:     return "Up";
                case ImGuiKey_DownArrow:   return "Down";
                case ImGuiKey_PageUp:      return "PgUp";
                case ImGuiKey_PageDown:    return "PgDn";
                case ImGuiKey_Home:        return "Home";
                case ImGuiKey_End:         return "End";
                case ImGuiKey_Insert:      return "Ins";
                case ImGuiKey_Delete:      return "Del";
                case ImGuiKey_Backspace:   return "Bksp";
                case ImGuiKey_Space:       return "Space";
                case ImGuiKey_Enter:       return "Enter";
                case ImGuiKey_Escape:      return "Esc";
                case ImGuiKey_F1:          return "F1";
                case ImGuiKey_F2:          return "F2";
                case ImGuiKey_F3:          return "F3";
                case ImGuiKey_F4:          return "F4";
                case ImGuiKey_F5:          return "F5";
                case ImGuiKey_F6:          return "F6";
                case ImGuiKey_F7:          return "F7";
                case ImGuiKey_F8:          return "F8";
                case ImGuiKey_F9:          return "F9";
                case ImGuiKey_F10:         return "F10";
                case ImGuiKey_F11:         return "F11";
                case ImGuiKey_F12:         return "F12";
                case ImGuiKey_A:           return "A";
                case ImGuiKey_B:           return "B";
                case ImGuiKey_C:           return "C";
                case ImGuiKey_D:           return "D";
                case ImGuiKey_E:           return "E";
                case ImGuiKey_F:           return "F";
                case ImGuiKey_G:           return "G";
                case ImGuiKey_H:           return "H";
                case ImGuiKey_I:           return "I";
                case ImGuiKey_J:           return "J";
                case ImGuiKey_K:           return "K";
                case ImGuiKey_L:           return "L";
                case ImGuiKey_M:           return "M";
                case ImGuiKey_N:           return "N";
                case ImGuiKey_O:           return "O";
                case ImGuiKey_P:           return "P";
                case ImGuiKey_Q:           return "Q";
                case ImGuiKey_R:           return "R";
                case ImGuiKey_S:           return "S";
                case ImGuiKey_T:           return "T";
                case ImGuiKey_U:           return "U";
                case ImGuiKey_V:           return "V";
                case ImGuiKey_W:           return "W";
                case ImGuiKey_X:           return "X";
                case ImGuiKey_Y:           return "Y";
                case ImGuiKey_Z:           return "Z";
                case ImGuiKey_0:           return "0";
                case ImGuiKey_1:           return "1";
                case ImGuiKey_2:           return "2";
                case ImGuiKey_3:           return "3";
                case ImGuiKey_4:           return "4";
                case ImGuiKey_5:           return "5";
                case ImGuiKey_6:           return "6";
                case ImGuiKey_7:           return "7";
                case ImGuiKey_8:           return "8";
                case ImGuiKey_9:           return "9";
                case ImGuiKey_Minus:       return "-";
                case ImGuiKey_Equal:       return "=";
                case ImGuiKey_LeftBracket: return "[";
                case ImGuiKey_RightBracket:return "]";
                case ImGuiKey_Backslash:   return "\\";
                case ImGuiKey_Semicolon:   return ";";
                case ImGuiKey_Apostrophe:  return "'";
                case ImGuiKey_GraveAccent: return "`";
                case ImGuiKey_Comma:       return ",";
                case ImGuiKey_Period:      return ".";
                case ImGuiKey_Slash:       return "/";
                case ImGuiKey_CapsLock:    return "Caps";
                case ImGuiKey_PrintScreen: return "PrtSc";
                case ImGuiKey_Pause:       return "Pause";
                case ImGuiKey_NumLock:     return "NumLk";
                case ImGuiKey_ScrollLock:  return "ScrLk";
                case ImGuiKey_Keypad0:     return "Num0";
                case ImGuiKey_Keypad1:     return "Num1";
                case ImGuiKey_Keypad2:     return "Num2";
                case ImGuiKey_Keypad3:     return "Num3";
                case ImGuiKey_Keypad4:     return "Num4";
                case ImGuiKey_Keypad5:     return "Num5";
                case ImGuiKey_Keypad6:     return "Num6";
                case ImGuiKey_Keypad7:     return "Num7";
                case ImGuiKey_Keypad8:     return "Num8";
                case ImGuiKey_Keypad9:     return "Num9";
                case ImGuiKey_KeypadAdd:      return "Num+";
                case ImGuiKey_KeypadSubtract: return "Num-";
                case ImGuiKey_KeypadMultiply: return "Num*";
                case ImGuiKey_KeypadDivide:   return "Num/";
                case ImGuiKey_KeypadDecimal:  return "Num.";
                case ImGuiKey_KeypadEnter:    return "NumEnt";
                default:                   return "?";
            }
        }
    };

    inline bool KeybindRow(const char* label, const char* tip, KeybindRecorder& kb, float rowHeight = 36.f) {
        float tipR = 7.5f;
        float tipW = tipR * 2.f + 8.f;

        ImVec2 pos = ImGui::GetCursorScreenPos();
        float avail = ImGui::GetContentRegionAvail().x;

        float tipY = pos.y + (rowHeight - tipR * 2.f) * 0.5f;
        ImGui::SetCursorScreenPos({pos.x + avail - tipW, tipY});
        InfoTip(tip, tipR);

        ImGui::SetCursorScreenPos(pos);
        ImGui::PushItemWidth(avail - tipW - 4.f);
        bool changed = kb.Render(label, rowHeight);
        ImGui::PopItemWidth();

        return changed;
    }

} // namespace UI