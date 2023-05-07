// imgui-notify by patrickcjk
// https://github.com/patrickcjk/imgui-notify

#include "imgui_notify.hpp"


    auto ImGuiToast::set_title(const char* format, va_list args) { vsnprintf(this->title, sizeof(this->title), format, args); }

    auto ImGuiToast::set_content(const char* format, va_list args) { vsnprintf(this->content, sizeof(this->content), format, args); }

    auto ImGuiToast::set_title(const char* format, ...) -> void { NOTIFY_FORMAT(this->set_title, format); }

    auto ImGuiToast::set_content(const char* format, ...) -> void { NOTIFY_FORMAT(this->set_content, format); }

    auto ImGuiToast::set_type(const ImGuiToastType& type) -> void { IM_ASSERT(type < ImGuiToastType_COUNT); this->type = type; };



    NOTIFY_INLINE auto ImGuiToast::get_title() -> char* { return this->title; };

    NOTIFY_INLINE auto ImGuiToast::get_default_title() -> char*
    {
        if (!strlen(this->title))
        {
            switch (this->type)
            {
            case ImGuiToastType_None:
                return NULL;
            case ImGuiToastType_Success:
                return "Success";
            case ImGuiToastType_Warning:
                return "Warning";
            case ImGuiToastType_Error:
                return "Error";
            case ImGuiToastType_Info:
                return "Info";
            }
        }

        return this->title;
    };

    NOTIFY_INLINE auto ImGuiToast::get_type() -> const ImGuiToastType& { return this->type; };

    NOTIFY_INLINE auto ImGuiToast::get_color() -> const ImVec4&
    {
        switch (this->type)
        {
        case ImGuiToastType_None:
            return { 255, 255, 255, 255 }; // White
        case ImGuiToastType_Success:
            return { 0, 255, 0, 255 }; // Green
        case ImGuiToastType_Warning:
            return { 255, 255, 0, 255 }; // Yellow
        case ImGuiToastType_Error:
            return { 255, 0, 0, 255 }; // Error
        case ImGuiToastType_Info:
            return { 0, 157, 255, 255 }; // Blue
        }
    }

    NOTIFY_INLINE auto ImGuiToast::get_icon() -> const char*
    {
        switch (this->type)
        {
        case ImGuiToastType_None:
            return NULL;
        case ImGuiToastType_Success:
            return ICON_FA_CHECK_CIRCLE;
        case ImGuiToastType_Warning:
            return ICON_FA_EXCLAMATION_TRIANGLE;
        case ImGuiToastType_Error:
            return ICON_FA_TIMES_CIRCLE;
        case ImGuiToastType_Info:
            return ICON_FA_INFO_CIRCLE;
        }
    }

    NOTIFY_INLINE auto ImGuiToast::get_content() -> char* { return this->content; };

    NOTIFY_INLINE auto ImGuiToast::get_elapsed_time() { return GetTickCount64() - this->creation_time; }

    NOTIFY_INLINE auto ImGuiToast::get_phase() -> const ImGuiToastPhase&
    {
        const auto elapsed = get_elapsed_time();

        if (elapsed > NOTIFY_FADE_IN_OUT_TIME + this->dismiss_time + NOTIFY_FADE_IN_OUT_TIME)
        {
            return ImGuiToastPhase_Expired;
        }
        else if (elapsed > NOTIFY_FADE_IN_OUT_TIME + this->dismiss_time)
        {
            return ImGuiToastPhase_FadeOut;
        }
        else if (elapsed > NOTIFY_FADE_IN_OUT_TIME)
        {
            return ImGuiToastPhase_Wait;
        }
        else
        {
            return ImGuiToastPhase_FadeIn;
        }
    }

    NOTIFY_INLINE auto ImGuiToast::get_fade_percent() -> const float
    {
        const auto phase = get_phase();
        const auto elapsed = get_elapsed_time();

        if (phase == ImGuiToastPhase_FadeIn)
        {
            return ((float)elapsed / (float)NOTIFY_FADE_IN_OUT_TIME) * NOTIFY_OPACITY;
        }
        else if (phase == ImGuiToastPhase_FadeOut)
        {
            return (1.f - (((float)elapsed - (float)NOTIFY_FADE_IN_OUT_TIME - (float)this->dismiss_time) / (float)NOTIFY_FADE_IN_OUT_TIME)) * NOTIFY_OPACITY;
        }

        return 1.f * NOTIFY_OPACITY;
    }

    // Constructors

    ImGuiToast::ImGuiToast(ImGuiToastType type, int dismiss_time = NOTIFY_DEFAULT_DISMISS)
    {
        IM_ASSERT(type < ImGuiToastType_COUNT);

        this->type = type;
        this->dismiss_time = dismiss_time;
        this->creation_time = GetTickCount64();

        memset(this->title, 0, sizeof(this->title));
        memset(this->content, 0, sizeof(this->content));
    }

    ImGuiToast::ImGuiToast(ImGuiToastType type, const char* format, ...) : ImGuiToast(type) { NOTIFY_FORMAT(this->set_content, format); }

    ImGuiToast::ImGuiToast(ImGuiToastType type, int dismiss_time, const char* format, ...) : ImGuiToast(type, dismiss_time) { NOTIFY_FORMAT(this->set_content, format); }

    using namespace ImGui;
    NOTIFY_INLINE VOID InsertNotification(const ImGuiToast& toast)
    {
        notifications.push_back(toast);
    }

    /// <summary>
    /// Remove a toast from the list by its index
    /// </summary>
    /// <param name="index">index of the toast to remove</param>
    NOTIFY_INLINE VOID RemoveNotification(int index)
    {
        notifications.erase(notifications.begin() + index);
    }

    /// <summary>
    /// Render toasts, call at the end of your rendering!
    /// </summary>
    NOTIFY_INLINE VOID RenderNotifications()
    {
        const auto vp_size = GetMainViewport()->Size;

        float height = 0.f;

        for (auto i = 0; i < notifications.size(); i++)
        {
            auto* current_toast = &notifications[i];

            // Remove toast if expired
            if (current_toast->get_phase() == ImGuiToastPhase_Expired)
            {
                RemoveNotification(i);
                continue;
            }

            // Get icon, title and other data
            const auto icon = current_toast->get_icon();
            const auto title = current_toast->get_title();
            const auto content = current_toast->get_content();
            const auto default_title = current_toast->get_default_title();
            const auto opacity = current_toast->get_fade_percent(); // Get opacity based of the current phase

            // Window rendering
            auto text_color = current_toast->get_color();
            text_color.w = opacity;

            // Generate new unique name for this toast
            char window_name[50];
            sprintf_s(window_name, "##TOAST%d", i);

            //PushStyleColor(ImGuiCol_Text, text_color);
            SetNextWindowBgAlpha(opacity);
            SetNextWindowPos(ImVec2(vp_size.x - NOTIFY_PADDING_X, vp_size.y - NOTIFY_PADDING_Y - height), ImGuiCond_Always, ImVec2(1.0f, 1.0f));
            Begin(window_name, NULL, NOTIFY_TOAST_FLAGS);
            ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());

            // Here we render the toast content
            {
                PushTextWrapPos(vp_size.x / 3.f); // We want to support multi-line text, this will wrap the text after 1/3 of the screen width

                bool was_title_rendered = false;

                // If an icon is set
                if (!NOTIFY_NULL_OR_EMPTY(icon))
                {
                    //Text(icon); // Render icon text
                    TextColored(text_color, icon);
                    was_title_rendered = true;
                }

                // If a title is set
                if (!NOTIFY_NULL_OR_EMPTY(title))
                {
                    // If a title and an icon is set, we want to render on same line
                    if (!NOTIFY_NULL_OR_EMPTY(icon))
                        SameLine();

                    Text(title); // Render title text
                    was_title_rendered = true;
                }
                else if (!NOTIFY_NULL_OR_EMPTY(default_title))
                {
                    if (!NOTIFY_NULL_OR_EMPTY(icon))
                        SameLine();

                    Text(default_title); // Render default title text (ImGuiToastType_Success -> "Success", etc...)
                    was_title_rendered = true;
                }

                // In case ANYTHING was rendered in the top, we want to add a small padding so the text (or icon) looks centered vertically
                if (was_title_rendered && !NOTIFY_NULL_OR_EMPTY(content))
                {
                    SetCursorPosY(GetCursorPosY() + 5.f); // Must be a better way to do this!!!!
                }

                // If a content is set
                if (!NOTIFY_NULL_OR_EMPTY(content))
                {
                    if (was_title_rendered)
                    {
#ifdef NOTIFY_USE_SEPARATOR
                        Separator();
#endif
                    }

                    Text(content); // Render content text
                }

                PopTextWrapPos();
            }

            // Save height for next toasts
            height += GetWindowHeight() + NOTIFY_PADDING_MESSAGE_Y;

            // End
            End();
        }
    }

    /// <summary>
    /// Adds font-awesome font, must be called ONCE on initialization
    /// <param name="FontDataOwnedByAtlas">Fonts are loaded from read-only memory, should be set to false!</param>
    /// </summary>
    NOTIFY_INLINE VOID MergeIconsWithLatestFont(float font_size, bool FontDataOwnedByAtlas = false)
    {
        static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        icons_config.FontDataOwnedByAtlas = FontDataOwnedByAtlas;

        GetIO().Fonts->AddFontFromMemoryTTF((void*)fa_solid_900, sizeof(fa_solid_900), font_size, &icons_config, icons_ranges);
    }


