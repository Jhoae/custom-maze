#pragma once

#include <functional>
#include <memory>
#include <string>

// Capture-run compatibility adapter (2026). The original menu commands
// and ofApp::appMenuFunction remain the source of application behavior.
class MacMenu {
public:
    using Handle = void*;
    using Callback = std::function<void(const std::string&, bool)>;

    explicit MacMenu(Callback callback);
    ~MacMenu();
    Handle CreateWindowMenu();
    Handle AddPopupMenu(Handle parent, const std::string& title);
    bool AddPopupItem(Handle parent, const std::string& title,
                      bool checked, bool autoCheck);
    bool AddPopupSeparator(Handle parent);
    bool SetWindowMenu();
    bool RemoveWindowMenu();
    bool SetPopupItem(const std::string& title, bool checked);
    void SetTopmost(bool topmost);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
