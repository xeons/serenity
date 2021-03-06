/*
 * Copyright (c) 2020, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "ShutdownDialog.h"
#include <AK/String.h>
#include <AK/Vector.h>
#include <LibGUI/BoxLayout.h>
#include <LibGUI/Button.h>
#include <LibGUI/Label.h>
#include <LibGUI/RadioButton.h>
#include <LibGUI/Widget.h>
#include <LibGfx/Font.h>
#include <LibGfx/FontDatabase.h>

struct Option {
    String title;
    Vector<char const*> cmd;
    bool enabled;
    bool default_action;
};

static const Vector<Option> options = {
    { "Shut down", { "/bin/shutdown", "--now", nullptr }, true, true },
    { "Restart", { "/bin/reboot", nullptr }, true, false },
    { "Log out", {}, false, false },
    { "Sleep", {}, false, false },
};

Vector<char const*> ShutdownDialog::show()
{
    auto dialog = ShutdownDialog::construct();
    auto rc = dialog->exec();
    if (rc == ExecResult::ExecOK && dialog->m_selected_option != -1)
        return options[dialog->m_selected_option].cmd;

    return {};
}

ShutdownDialog::ShutdownDialog()
    : Dialog(nullptr)
{
    resize(180, 180 + ((static_cast<int>(options.size()) - 3) * 16));
    center_on_screen();
    set_resizable(false);
    set_title("SerenityOS");
    set_icon(Gfx::Bitmap::load_from_file("/res/icons/16x16/power.png"));

    // Request WindowServer to re-update us on the current theme as we might've not been alive for the last notification.
    refresh_system_theme();

    auto& main = set_main_widget<GUI::Widget>();
    main.set_layout<GUI::VerticalBoxLayout>();
    main.layout()->set_margins({ 8, 8, 8, 8 });
    main.layout()->set_spacing(8);
    main.set_fill_with_background_color(true);

    auto& header = main.add<GUI::Label>();
    header.set_text("What would you like to do?");
    header.set_fixed_height(16);
    header.set_font(Gfx::FontDatabase::default_bold_font());

    for (size_t i = 0; i < options.size(); i++) {
        auto action = options[i];
        auto& radio = main.add<GUI::RadioButton>();
        radio.set_enabled(action.enabled);
        radio.set_text(action.title);

        radio.on_checked = [this, i](auto) {
            m_selected_option = i;
        };

        if (action.default_action) {
            radio.set_checked(true);
            m_selected_option = i;
        }
    }

    auto& button_box = main.add<GUI::Widget>();
    button_box.set_layout<GUI::HorizontalBoxLayout>();
    button_box.layout()->set_spacing(8);

    auto& ok_button = button_box.add<GUI::Button>();
    ok_button.on_click = [this](auto) {
        done(ExecResult::ExecOK);
    };
    ok_button.set_text("OK");

    auto& cancel_button = button_box.add<GUI::Button>();
    cancel_button.on_click = [this](auto) {
        done(ExecResult::ExecCancel);
    };
    cancel_button.set_text("Cancel");
}

ShutdownDialog::~ShutdownDialog()
{
}
