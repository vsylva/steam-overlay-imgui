#pragma once

// ============================================================
//  FPS Tool Demo  —  全组件演示，无任何外部函数调用
//  在你的主循环里调用 RenderFPSToolDemo() 即可
// ============================================================

#include "mui.hpp"

#include <cmath>
#include <cstring>

// ────────────────────────────────────────────────────────────
//  静态演示状态（仅作展示用，不驱动任何真实功能）
// ────────────────────────────────────────────────────────────
namespace Demo {

    // ── 性能监控 ──
    static float s_fps = 144.f;
    static float s_frametime = 6.9f;
    static float s_ping = 28.f;
    static float s_packetloss = 0.f;
    static float s_cpuLoad = 0.42f;
    static float s_gpuLoad = 0.78f;
    static float s_vramUsed = 0.61f;
    static float s_ramUsed = 0.35f;

    // ── 画面设置 ──
    static bool s_vsync = false;
    static bool s_fxaa = true;
    static bool s_motionBlur = false;
    static bool s_showFps = true;
    static bool s_showPing = true;
    static bool s_showMap = false;
    static bool s_showDamage = true;
    static float s_brightness = 0.55f;
    static float s_contrast = 0.50f;
    static float s_saturation = 0.60f;
    static float s_fov = 0.72f; // 0=70°  1=110°
    static float s_sensitivity = 0.38f;
    static int s_resolutionIdx = 2;
    static int s_qualityIdx = 1;
    static int s_fpsCap = 7; // 0..10 → 60..240

    // ── 快捷键 ──
    static UI::KeybindRecorder s_kb_scoreBoard;
    static UI::KeybindRecorder s_kb_map;
    static UI::KeybindRecorder s_kb_ping;
    static UI::KeybindRecorder s_kb_screenshot;
    static UI::KeybindRecorder s_kb_fpsToggle;

    // ── 其他 ──
    static int s_activeTab = 0;
    static char s_profileName[64] = u8"Default";
    static UI::Toast s_toast;

    static bool s_kbInitialized = false;

    inline void InitKeybinds() {
        if (s_kbInitialized)
            return;
        s_kb_scoreBoard.mainKey = ImGuiKey_Tab;
        s_kb_map.mainKey = ImGuiKey_M;
        s_kb_ping.mainKey = ImGuiKey_F3;
        s_kb_ping.ctrl = true;
        s_kb_screenshot.mainKey = ImGuiKey_F12;
        // s_kb_fpsToggle 故意留空，演示u8"未绑定"状态
        s_kbInitialized = true;
    }

    // 用 sin 模拟实时抖动，让演示看起来u8"活"
    inline void TickSimulation() {
        float t = (float)ImGui::GetTime();
        s_fps = 144.f + sinf(t * 0.7f) * 8.f;
        s_frametime = 1000.f / s_fps;
        s_ping = 28.f + sinf(t * 1.3f) * 4.f;
        s_cpuLoad = 0.42f + sinf(t * 0.5f) * 0.08f;
        s_gpuLoad = 0.78f + sinf(t * 0.9f) * 0.06f;
        s_vramUsed = 0.61f + sinf(t * 0.4f) * 0.04f;
        s_ramUsed = 0.35f + sinf(t * 0.3f) * 0.03f;
    }

} // namespace Demo

// ────────────────────────────────────────────────────────────
//  辅助：把 [0,1] 映射到颜色（绿→黄→红）
// ────────────────────────────────────────────────────────────
static inline ImVec4 LoadColor(float v) {
    using namespace UI::Color;
    if (v < 0.60f)
        return Success;
    if (v < 0.85f)
        return Warning;
    return Danger;
}

// ────────────────────────────────────────────────────────────
//  Tab 0 ── 性能监控
// ────────────────────────────────────────────────────────────
static void RenderTab_Performance() {
    using namespace UI;
    using namespace Demo;

    // ── 网络状态 ──────────────────────────────────────────
    SectionHeader(u8"网络状态");

    float pingNorm = ImClamp(s_ping / 200.f, 0.f, 1.f);
    ImVec4 pingColor = LoadColor(pingNorm);

    ImGui::BeginGroup();
    StatusDot(u8"服务器已连接", Color::Success);
    ImGui::SameLine(0.f, 24.f);
    StatusDot(u8"语音已连接", Color::Success);
    ImGui::SameLine(0.f, 24.f);
    StatusDot(u8"反作弊运行中", Color::Accent);
    ImGui::EndGroup();

    ImGui::Spacing();

    {
        // 自定义进度条行：标签 + 条 + 数值 badge
        float avail = ImGui::GetContentRegionAvail().x;

        // Ping
        ImGui::Text(u8"延迟 (Ping)");
        ImGui::SameLine(avail * 0.38f);
        ImGui::PushItemWidth(avail * 0.42f);
        ProgressBar(pingNorm, {avail * 0.42f, 7.f}, pingColor);
        ImGui::PopItemWidth();
        ImGui::SameLine(0.f, 8.f);
        char buf[24];
        snprintf(buf, sizeof(buf), u8"%.0f ms", s_ping);
        Badge(buf, pingColor);

        // 丢包
        ImGui::Text(u8"丢包率");
        ImGui::SameLine(avail * 0.38f);
        ProgressBar(s_packetloss, {avail * 0.42f, 7.f}, Color::Success);
        ImGui::SameLine(0.f, 8.f);
        Badge(u8"0 %", Color::Success);
    }

    ImGui::Spacing();
    Separator();

    // ── 帧率 ──────────────────────────────────────────────
    SectionHeader(u8"帧率");

    {
        float avail = ImGui::GetContentRegionAvail().x;

        // FPS 大数字 badge
        char fpsBuf[32];
        snprintf(fpsBuf, sizeof(fpsBuf), u8"%.0f FPS", s_fps);
        Badge(fpsBuf, s_fps >= 120.f ? Color::Success : (s_fps >= 60.f ? Color::Warning : Color::Danger));
        ImGui::SameLine(0.f, 10.f);
        char ftBuf[32];
        snprintf(ftBuf, sizeof(ftBuf), u8"%.2f ms", s_frametime);
        Badge(ftBuf, Color::AccentDim);
        ImGui::SameLine(0.f, 10.f);
        Badge(s_vsync ? u8"VSync ON" : u8"VSync OFF", s_vsync ? Color::Accent : Color::TextLow);

        ImGui::Spacing();

        // FPS 历史条形（静态模拟）
        float barW = (avail - 10 * 4.f) / 11.f;
        static float fakeBars[11] = {0.85f, 0.92f, 0.88f, 0.95f, 0.90f, 0.87f, 0.93f, 0.96f, 0.89f, 0.91f, 0.94f};
        // 让最后一根动起来
        fakeBars[10] = ImClamp(s_fps / 160.f, 0.f, 1.f);
        ImVec2 cursor = ImGui::GetCursorScreenPos();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        float bh = 40.f;
        for (int i = 0; i < 11; ++i) {
            float bx = cursor.x + i * (barW + 4.f);
            float by = cursor.y + bh * (1.f - fakeBars[i]);
            ImVec4 bc = LoadColor(1.f - fakeBars[i]);
            // shadow
            dl->AddRectFilled({bx + 1.f, by + 1.f}, {bx + barW, cursor.y + bh + 1.f}, IM_COL32(0, 0, 0, 40), 3.f);
            dl->AddRectFilled({bx, by}, {bx + barW, cursor.y + bh}, UI::_impl::U32(UI::_impl::Alpha(bc, 0.80f)), 3.f);
            // sheen
            dl->AddRectFilled(
                {bx, by},
                {bx + barW, by + 2.f},
                UI::_impl::U32(UI::_impl::Alpha(UI::Color::AccentBright, 0.25f)),
                1.f
            );
        }
        ImGui::Dummy({avail, bh + 4.f});
    }

    ImGui::Spacing();
    Separator();

    // ── 硬件负载 ──────────────────────────────────────────
    SectionHeader(u8"硬件负载");

    struct LoadItem {
        const char* name;
        float val;
        const char* unit;
    };

    LoadItem items[] = {
        {u8"CPU", s_cpuLoad, u8""},
        {u8"GPU", s_gpuLoad, u8""},
        {u8"VRAM", s_vramUsed, u8""},
        {u8"RAM", s_ramUsed, u8""},
    };
    float avail = ImGui::GetContentRegionAvail().x;

    for (auto& it : items) {
        ImVec4 c = LoadColor(it.val);
        ImGui::Text(u8"%s", it.name);
        ImGui::SameLine(avail * 0.14f);
        ProgressBar(it.val, {avail * 0.62f, 8.f}, c);
        ImGui::SameLine(0.f, 8.f);
        char pct[16];
        snprintf(pct, sizeof(pct), u8"%.0f %%", it.val * 100.f);
        Badge(pct, c);
        ImGui::Spacing();
    }
}

// ────────────────────────────────────────────────────────────
//  Tab 1 ── 画面 & 游戏设置
// ────────────────────────────────────────────────────────────
static void RenderTab_Settings() {
    using namespace UI;
    using namespace Demo;

    float avail = ImGui::GetContentRegionAvail().x;
    float halfW = avail * 0.48f;

    // ── 显示开关 ──────────────────────────────────────────
    SectionHeader(u8"HUD 显示");

    ImGui::Columns(2, u8"hud_cols", false);
    ImGui::SetColumnWidth(0, halfW);

    Toggle(u8"显示帧率", &s_showFps);
    ImGui::Spacing();
    Toggle(u8"显示延迟", &s_showPing);
    ImGui::Spacing();
    Toggle(u8"显示小地图", &s_showMap);

    ImGui::NextColumn();

    Toggle(u8"显示伤害数字", &s_showDamage);
    ImGui::Spacing();
    Checkbox(u8"垂直同步", &s_vsync);
    ImGui::Spacing();
    Checkbox(u8"FXAA 抗锯齿", &s_fxaa);
    ImGui::Spacing();
    Checkbox(u8"动态模糊", &s_motionBlur);

    ImGui::Columns(1);

    ImGui::Spacing();
    Separator();

    // ── 分辨率 & 质量下拉 ─────────────────────────────────
    SectionHeader(u8"画面质量");

    const char* resOptions[] = {u8"1280 × 720", u8"1920 × 1080", u8"2560 × 1440", u8"3840 × 2160"};
    const char* qualOptions[] = {u8"低 (Low)", u8"中 (Medium)", u8"高 (High)", u8"极致 (Ultra)"};

    ImGui::Text(u8"分辨率");
    ImGui::SameLine(avail * 0.32f);
    ImGui::PushItemWidth(avail * 0.65f);
    if (BeginCombo(u8"##res", resOptions[s_resolutionIdx])) {
        for (int i = 0; i < 4; ++i) {
            bool sel = (i == s_resolutionIdx);
            if (ImGui::Selectable(resOptions[i], sel))
                s_resolutionIdx = i;
            if (sel)
                ImGui::SetItemDefaultFocus();
        }
        EndCombo();
    }
    ImGui::PopItemWidth();

    ImGui::Spacing();

    ImGui::Text(u8"画面质量");
    ImGui::SameLine(avail * 0.32f);
    ImGui::PushItemWidth(avail * 0.65f);
    if (BeginCombo(u8"##qual", qualOptions[s_qualityIdx])) {
        for (int i = 0; i < 4; ++i) {
            bool sel = (i == s_qualityIdx);
            if (ImGui::Selectable(qualOptions[i], sel))
                s_qualityIdx = i;
            if (sel)
                ImGui::SetItemDefaultFocus();
        }
        EndCombo();
    }
    ImGui::PopItemWidth();

    ImGui::Spacing();
    Separator();

    // ── 色彩 & 视角滑条 ────────────────────────────────────
    SectionHeader(u8"色彩 & 视角");

    SliderFloat(u8"亮度", &s_brightness, 0.f, 1.f);
    ImGui::Spacing();
    SliderFloat(u8"对比度", &s_contrast, 0.f, 1.f);
    ImGui::Spacing();
    SliderFloat(u8"饱和度", &s_saturation, 0.f, 1.f);

    ImGui::Spacing();
    Separator();

    SectionHeader(u8"操控");

    // FOV 映射到 70-110
    float fovDisplay = 70.f + s_fov * 40.f;
    char fovFmt[32];
    snprintf(fovFmt, sizeof(fovFmt), u8"%.0f °", fovDisplay);
    // 暂用 SliderFloat，显示度数
    SliderFloat(u8"视野角 (FOV)", &s_fov, 0.f, 1.f, u8"%.2f");
    ImGui::Spacing();
    SliderFloat(u8"鼠标灵敏度", &s_sensitivity, 0.f, 1.f);
    ImGui::Spacing();
    SliderInt(u8"帧率上限", &s_fpsCap, 0, 10); // 演示整数滑条

    ImGui::Spacing();
    Separator();

    // ── 操作按钮行 ────────────────────────────────────────
    float btnW = (avail - 12.f) / 3.f;
    if (Button(u8"应用设置", {btnW, 36.f}, ButtonVariant::Primary)) {
        s_toast.Show(u8"设置已应用！", 2.5f, Color::Success);
    }
    ImGui::SameLine(0.f, 6.f);
    if (Button(u8"恢复默认", {btnW, 36.f}, ButtonVariant::Ghost)) {
        s_brightness = 0.55f;
        s_contrast = 0.50f;
        s_saturation = 0.60f;
        s_fov = 0.72f;
        s_sensitivity = 0.38f;
        s_toast.Show(u8"已恢复默认值", 2.0f, Color::Warning);
    }
    ImGui::SameLine(0.f, 6.f);
    if (Button(u8"重置统计", {btnW, 36.f}, ButtonVariant::Danger)) {
        s_toast.Show(u8"统计数据已清空", 2.0f, Color::Danger);
    }
}

// ────────────────────────────────────────────────────────────
//  Tab 2 ── 快捷键管理
// ────────────────────────────────────────────────────────────
static void RenderTab_Keybinds() {
    using namespace UI;
    using namespace Demo;

    SectionHeader(u8"游戏内快捷键");

    ImGui::TextDisabled(u8"点击行以开始录制 · Esc 取消 · Backspace/Delete 清除绑定");
    ImGui::Spacing();

    struct KbEntry {
        const char* label;
        const char* tip;
        KeybindRecorder* kb;
    };

    KbEntry entries[] = {
        {u8"计分板", u8"按住显示本局计分板 (Tab)", &s_kb_scoreBoard},
        {u8"小地图", u8"切换大地图叠加层显示", &s_kb_map},
        {u8"网络信息", u8"显示详细 Ping / 丢包 / 路由信息", &s_kb_ping},
        {u8"截图", u8"保存无 HUD 纯净截图到 Screenshots 目录", &s_kb_screenshot},
        {u8"FPS 叠加层", u8"临时开关帧率计数器显示", &s_kb_fpsToggle},
    };

    for (auto& e : entries) {
        KeybindRow(e.label, e.tip, *e.kb);
        ImGui::Spacing();
    }

    Separator();
    SectionHeader(u8"快捷键预览");

    // 展示 Keybind 组件（只读展示样式）
    float avail = ImGui::GetContentRegionAvail().x;
    ImGui::TextDisabled(u8"以下为当前生效按键（Keybind 组件演示）：");
    ImGui::Spacing();

    struct KbPreview {
        const char* action;
        const char* key1;
        const char* key2;
    };

    KbPreview previews[] = {
        {u8"移动", u8"W A S D", nullptr},
        {u8"跳跃", u8"Space", nullptr},
        {u8"蹲下", u8"Ctrl", nullptr},
        {u8"奔跑", u8"Shift", nullptr},
        {u8"开镜", u8"右键", nullptr},
        {u8"重新装填", u8"R", nullptr},
        {u8"切换武器", u8"Q", nullptr},
        {u8"投掷物", u8"G", nullptr},
    };

    int col = 0;
    ImGui::Columns(2, u8"kb_preview_cols", false);
    ImGui::SetColumnWidth(0, avail * 0.50f);
    for (auto& p : previews) {
        // 行：动作名 + Keybind 小标签
        ImVec4 tc = UI::Color::TextMid;
        ImGui::GetWindowDrawList()->AddText(ImGui::GetCursorScreenPos(), UI::_impl::U32(tc), p.action);
        ImGui::Dummy({ImGui::CalcTextSize(p.action).x, ImGui::GetTextLineHeight()});
        ImGui::SameLine(0.f, 6.f);
        Keybind(p.key1);
        if (p.key2) {
            ImGui::SameLine(0.f, 4.f);
            Keybind(p.key2);
        }
        ImGui::Spacing();
        ++col;
        if (col == 4)
            ImGui::NextColumn();
    }
    ImGui::Columns(1);
}

// ────────────────────────────────────────────────────────────
//  Tab 3 ── 关于 & 组件演示
// ────────────────────────────────────────────────────────────
static void RenderTab_About() {
    using namespace UI;

    float avail = ImGui::GetContentRegionAvail().x;

    SectionHeader(u8"工具信息");

    ImGui::BeginGroup();
    Badge(u8"v1.0.0", Color::Accent);
    ImGui::SameLine(0.f, 8.f);
    Badge(u8"稳定版", Color::Success);
    ImGui::SameLine(0.f, 8.f);
    Badge(u8"64-bit", Color::TextMid);
    ImGui::SameLine(0.f, 8.f);
    Badge(u8"DirectX 12", Color::AccentDim);
    ImGui::EndGroup();

    ImGui::Spacing();

    StatusDot(u8"主进程运行中", Color::Success);
    ImGui::SameLine(0.f, 20.f);
    StatusDot(u8"驱动已加载", Color::Success);
    ImGui::SameLine(0.f, 20.f);
    StatusDot(u8"更新服务器离线", Color::Warning);

    ImGui::Spacing();
    Separator();

    SectionHeader(u8"配置档案");

    InputText(u8"档案名称", Demo::s_profileName, sizeof(Demo::s_profileName));
    ImGui::Spacing();

    float btnW = (avail - 8.f) / 2.f;
    if (Button(u8"导入配置", {btnW, 34.f}, ButtonVariant::Ghost))
        Demo::s_toast.Show(u8"功能演示中，暂无实际效果", 2.f, Color::Warning);
    ImGui::SameLine(0.f, 8.f);
    if (Button(u8"导出配置", {btnW, 34.f}, ButtonVariant::Ghost))
        Demo::s_toast.Show(u8"功能演示中，暂无实际效果", 2.f, Color::Warning);

    ImGui::Spacing();
    Separator();

    // ── InfoTip 集中演示 ────────────────────────────────
    SectionHeader(u8"提示图标演示");

    ImGui::Text(u8"将鼠标悬停在图标上查看说明");
    ImGui::Spacing();

    struct TipItem {
        const char* label;
        const char* tip;
        ImVec4 color;
    };

    TipItem tips[] = {
        {u8"帧率", u8"当前渲染帧率，单位 FPS。目标值建议 ≥ 60。", Color::Accent},
        {u8"延迟", u8"到游戏服务器的往返时间 (RTT)。低于 50ms 体验最佳。", Color::Warning},
        {u8"丢包", u8"UDP 数据包丢失百分比，超过 2% 会出现明显卡顿。", Color::Danger},
        {u8"分辨率", u8"渲染分辨率越高画面越清晰，但 GPU 负载也随之增加。", Color::Success},
    };

    for (auto& t : tips) {
        ImGui::Text(u8"%s", t.label);
        ImGui::SameLine(0.f, 6.f);
        InfoTip(t.tip, 8.5f, t.color);
        ImGui::SameLine(0.f, 20.f);
    }
    ImGui::NewLine();

    ImGui::Spacing();
    Separator();

    // ── 按钮变体演示 ────────────────────────────────────
    SectionHeader(u8"按钮样式");

    float bw = (avail - 16.f) / 3.f;
    if (Button(u8"Primary", {bw, 36.f}, ButtonVariant::Primary))
        Demo::s_toast.Show(u8"Primary 按钮点击！", 2.f, Color::Accent);
    ImGui::SameLine(0.f, 8.f);
    if (Button(u8"Ghost", {bw, 36.f}, ButtonVariant::Ghost))
        Demo::s_toast.Show(u8"Ghost 按钮点击！", 2.f, Color::AccentDim);
    ImGui::SameLine(0.f, 8.f);
    if (Button(u8"Danger", {bw, 36.f}, ButtonVariant::Danger))
        Demo::s_toast.Show(u8"Danger 按钮点击！", 2.5f, Color::Danger);

    ImGui::Spacing();
    Separator();

    // ── ProgressBar 颜色演示 ────────────────────────────
    SectionHeader(u8"进度条颜色变体");

    static float demoProgress = 0.f;
    demoProgress = (sinf((float)ImGui::GetTime() * 0.8f) * 0.5f + 0.5f);

    ProgressBar(demoProgress, {-1.f, 8.f}, Color::Accent);
    ImGui::Spacing();
    ProgressBar(demoProgress * 0.8f, {-1.f, 8.f}, Color::Success);
    ImGui::Spacing();
    ProgressBar(demoProgress * 0.6f + 0.3f, {-1.f, 8.f}, Color::Warning);
    ImGui::Spacing();
    ProgressBar(demoProgress * 0.4f + 0.5f, {-1.f, 8.f}, Color::Danger);
}

// ────────────────────────────────────────────────────────────
//  主入口  — 在 ImGui::NewFrame() 之后调用一次
// ────────────────────────────────────────────────────────────
inline void RenderFPSToolDemo() {
    using namespace UI;
    using namespace Demo;

    InitKeybinds();
    TickSimulation();
    ApplyTheme();

    // ── 窗口尺寸 & 位置（首次居中）──────────────────────
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowSize({560.f, 680.f}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos({io.DisplaySize.x * .5f, io.DisplaySize.y * .5f}, ImGuiCond_FirstUseEver, {0.5f, 0.5f});

    ImGui::Begin(u8"##fps_tool", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    // ── 顶部标题行 ─────────────────────────────────────
    {
        float avail = ImGui::GetContentRegionAvail().x;
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImVec2 wp = ImGui::GetWindowPos();
        ImVec2 ws = ImGui::GetWindowSize();

        // 顶部渐变装饰条
        dl->AddRectFilledMultiColor(
            wp,
            {wp.x + ws.x, wp.y + 3.f},
            IM_COL32(0, 0, 0, 0),
            IM_COL32(0, 0, 0, 0),
            _impl::U32(Color::Accent),
            _impl::U32(Color::AccentDim)
        );

        Badge(u8"FPS TOOL", Color::Accent);
        ImGui::SameLine(0.f, 8.f);
        Badge(u8"DEMO", Color::AccentDim);
        ImGui::SameLine(avail - 120.f);

        // 当前帧率简要
        char buf[32];
        snprintf(buf, sizeof(buf), u8"%.0f fps  %.1f ms", s_fps, s_frametime);
        dl->AddText(ImGui::GetCursorScreenPos(), _impl::U32(_impl::Alpha(Color::TextMid, 0.60f)), buf);
        ImGui::Dummy({120.f, ImGui::GetTextLineHeight()});
        ImGui::Spacing();
    }

    // ── Tab Bar ─────────────────────────────────────────
    static TabBar s_tabBar(u8"##main_tabs", 38.f);
    static bool s_tabsAdded = false;
    if (!s_tabsAdded) {
        s_tabBar.Add(u8"  性能监控  ").Add(u8"  画面设置  ").Add(u8"  快捷键  ").Add(u8"  关于  ");
        s_tabsAdded = true;
    }
    s_tabBar.Render(s_activeTab);

    // ── 内容面板 ────────────────────────────────────────
    float panelH = ImGui::GetContentRegionAvail().y - 4.f;
    if (BeginPanel(u8"##content", {0.f, panelH})) {
        ImGui::Spacing();
        switch (s_activeTab) {
            case 0:
                RenderTab_Performance();
                break;
            case 1:
                RenderTab_Settings();
                break;
            case 2:
                RenderTab_Keybinds();
                break;
            case 3:
                RenderTab_About();
                break;
        }
        EndPanel();
    }

    ImGui::End();

    // ── Toast 通知（浮动在最顶层）──────────────────────
    s_toast.Render(ImGui::GetWindowPos(), ImGui::GetWindowSize());
}