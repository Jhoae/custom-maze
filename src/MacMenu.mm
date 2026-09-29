#import <Cocoa/Cocoa.h>
#include "MacMenu.h"
#include <utility>

@interface CustomMazeMenuTarget : NSObject {
@public
    MacMenu::Callback callback;
}
- (void)selected:(NSMenuItem*)sender;
@end

@implementation CustomMazeMenuTarget
- (void)selected:(NSMenuItem*)sender {
    if ([sender.representedObject boolValue]) {
        sender.state = sender.state == NSControlStateValueOn
            ? NSControlStateValueOff : NSControlStateValueOn;
    }
    if (callback) {
        callback(std::string([sender.title UTF8String]),
                 sender.state == NSControlStateValueOn);
    }
}
@end

struct MacMenu::Impl {
    NSMenu* root = nil;
    CustomMazeMenuTarget* target = nil;
};

MacMenu::MacMenu(Callback callback) : impl(new Impl) {
    impl->target = [[CustomMazeMenuTarget alloc] init];
    impl->target->callback = std::move(callback);
}

MacMenu::~MacMenu() = default;

MacMenu::Handle MacMenu::CreateWindowMenu() {
    impl->root = [[NSMenu alloc] initWithTitle:@"Custom Maze"];
    impl->root.autoenablesItems = NO;

    // A normal macOS application menu precedes the original menus.
    NSMenuItem* appItem = [[NSMenuItem alloc] initWithTitle:@"Custom Maze"
                                                  action:nil keyEquivalent:@""];
    NSMenu* appMenu = [[NSMenu alloc] initWithTitle:@"Custom Maze"];
    NSMenuItem* quitItem = [[NSMenuItem alloc] initWithTitle:@"Quit Custom Maze"
                                                   action:@selector(terminate:)
                                            keyEquivalent:@"q"];
    quitItem.target = NSApp;
    [appMenu addItem:quitItem];
    appItem.submenu = appMenu;
    [impl->root addItem:appItem];
    return (__bridge Handle)impl->root;
}

MacMenu::Handle MacMenu::AddPopupMenu(Handle parent, const std::string& title) {
    NSString* label = [NSString stringWithUTF8String:title.c_str()];
    NSMenuItem* item = [[NSMenuItem alloc] initWithTitle:label
                                               action:nil keyEquivalent:@""];
    NSMenu* submenu = [[NSMenu alloc] initWithTitle:label];
    submenu.autoenablesItems = NO;
    item.submenu = submenu;
    [(__bridge NSMenu*)parent addItem:item];
    return (__bridge Handle)submenu;
}

bool MacMenu::AddPopupItem(Handle parent, const std::string& title,
                           bool checked, bool autoCheck) {
    NSString* label = [NSString stringWithUTF8String:title.c_str()];
    NSString* shortcut = title == "Open" ? @"o" : @"";
    NSMenuItem* item = [[NSMenuItem alloc] initWithTitle:label
                                               action:@selector(selected:)
                                        keyEquivalent:shortcut];
    item.target = impl->target;
    item.state = checked ? NSControlStateValueOn : NSControlStateValueOff;
    item.representedObject = @(autoCheck);
    [(__bridge NSMenu*)parent addItem:item];
    return true;
}

bool MacMenu::AddPopupSeparator(Handle parent) {
    [(__bridge NSMenu*)parent addItem:[NSMenuItem separatorItem]];
    return true;
}

bool MacMenu::SetWindowMenu() {
    [NSApp setMainMenu:impl->root];
    return true;
}

bool MacMenu::RemoveWindowMenu() {
    // macOS manages menu-bar visibility in fullscreen; retain its native menu.
    return true;
}

bool MacMenu::SetPopupItem(const std::string& title, bool checked) {
    NSString* label = [NSString stringWithUTF8String:title.c_str()];
    for (NSMenuItem* parent in impl->root.itemArray) {
        for (NSMenuItem* item in parent.submenu.itemArray) {
            if ([item.title isEqualToString:label]) {
                item.state = checked ? NSControlStateValueOn : NSControlStateValueOff;
                return true;
            }
        }
    }
    return false;
}

void MacMenu::SetTopmost(bool topmost) {
    NSWindow* window = [NSApp keyWindow];
    if (!window) window = [NSApp mainWindow];
    window.level = topmost ? NSFloatingWindowLevel : NSNormalWindowLevel;
}
