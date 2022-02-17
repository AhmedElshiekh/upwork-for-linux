#include "TrayIcon.hpp"
#include <utility>
#include <gtkmm/icontheme.h>

namespace wfl::ui
{
    namespace
    {
        std::pair<char const*, char const*> getTrayIconNames()
        {
            constexpr auto const UPWORK_TRAY                     = "upwork-tray";
            constexpr auto const UPWORK_TRAY_ATTENTION           = "upwork-tray-attention";
            constexpr auto const UPWORK_FOR_LINUX_TRAY           = "upwork-for-linux-tray";
            constexpr auto const UPWORK_FOR_LINUX_TRAY_ATTENTION = "upwork-for-linux-tray-attention";

            auto const iconTheme = Gtk::IconTheme::get_default();
            if (iconTheme->has_icon(UPWORK_TRAY) && iconTheme->has_icon(UPWORK_TRAY_ATTENTION))
            {
                return {UPWORK_TRAY, UPWORK_TRAY_ATTENTION};
            }
            else if (iconTheme->has_icon(UPWORK_FOR_LINUX_TRAY) && iconTheme->has_icon(UPWORK_FOR_LINUX_TRAY_ATTENTION))
            {
                return {UPWORK_FOR_LINUX_TRAY, UPWORK_FOR_LINUX_TRAY_ATTENTION};
            }
            else
            {
                return {"indicator-messages", "indicator-messages-new"};
            }
        }
    }

    TrayIcon::TrayIcon()
        : m_appIndicator{app_indicator_new("com.github.upwork-for-linux.tray", "", APP_INDICATOR_CATEGORY_COMMUNICATIONS)}
        , m_popupMenu{}
        , m_signalShow{}
        , m_signalAbout{}
        , m_signalQuit{}
    {
        auto const [trayIconName, attentionIconName] = getTrayIconNames();
        app_indicator_set_icon_full(m_appIndicator, trayIconName, "Upwork for Linux Tray");
        app_indicator_set_attention_icon_full(m_appIndicator, attentionIconName, "Upwork for Linux Tray Attention");

        auto const showMenuItem = Gtk::manage(new Gtk::MenuItem{"Show"});
        auto const aboutMenuItem = Gtk::manage(new Gtk::MenuItem{"About"});
        auto const quitMenuItem = Gtk::manage(new Gtk::MenuItem{"Quit"});
        m_popupMenu.append(*showMenuItem);
        m_popupMenu.append(*aboutMenuItem);
        m_popupMenu.append(*quitMenuItem);

        app_indicator_set_menu(m_appIndicator, m_popupMenu.gobj());

        showMenuItem->signal_activate().connect([this]{ m_signalShow.emit(); });
        aboutMenuItem->signal_activate().connect([this]{ m_signalAbout.emit(); });
        quitMenuItem->signal_activate().connect([this]{ m_signalQuit.emit(); });

        m_popupMenu.show_all();

        app_indicator_set_status(m_appIndicator, APP_INDICATOR_STATUS_PASSIVE);
        app_indicator_set_menu(m_appIndicator, m_popupMenu.gobj());
    }

    void TrayIcon::setVisible(bool visible)
    {
        app_indicator_set_status(m_appIndicator, (visible ? APP_INDICATOR_STATUS_ACTIVE : APP_INDICATOR_STATUS_PASSIVE));
    }

    bool TrayIcon::isVisible() const
    {
        return (app_indicator_get_status(m_appIndicator) != APP_INDICATOR_STATUS_PASSIVE);
    }

    void TrayIcon::setAttention(bool attention)
    {
        if (!isVisible())
        {
            return;
        }

        app_indicator_set_status(m_appIndicator, (attention ? APP_INDICATOR_STATUS_ATTENTION : APP_INDICATOR_STATUS_ACTIVE));
    }

    sigc::signal<void> TrayIcon::signalShow() const noexcept
    {
        return m_signalShow;
    }

    sigc::signal<void> TrayIcon::signalAbout() const noexcept
    {
        return m_signalAbout;
    }

    sigc::signal<void> TrayIcon::signalQuit() const noexcept
    {
        return m_signalQuit;
    }
}
