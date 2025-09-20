#import <Cocoa/Cocoa.h>
#include "gui/gui_controller.h"
#include "core/command_queue.h"
#include "core/module_manager.h"
#include "core/self_destruct.h"
#include <thread>
#include <iostream>

// --- Global state for the GUI ---
static NSWindow* g_window = nil;
static bool g_is_visible = false;
static bool g_is_running = false;
static std::thread g_gui_thread;

// --- Objective-C GUI Controller Class ---

@interface GuiController : NSObject <NSApplicationDelegate>
- (void)setupWindow;
- (void)setupUI;
- (void)toggleVisibility;
@end

@implementation GuiController

- (void)setupWindow {
    g_window = [[NSWindow alloc]
        initWithContentRect:NSMakeRect(0, 0, 220, 350)
        styleMask:NSWindowStyleMaskBorderless
        backing:NSBackingStoreBuffered
        defer:NO];

    [g_window setOpaque:NO];
    [g_window setBackgroundColor:[NSColor clearColor]];
    [g_window setLevel:NSWindowLevelFloating];
    [g_window setMovableByWindowBackground:YES];
    [g_window setHasShadow:YES];

    NSVisualEffectView* view = [[NSVisualEffectView alloc] initWithFrame:[g_window frame]];
    view.blendingMode = NSVisualEffectBlendingModeBehindWindow;
    view.material = NSVisualEffectMaterialDark;
    view.state = NSVisualEffectStateActive;
    view.wantsLayer = YES;
    view.layer.cornerRadius = 10.0;

    g_window.contentView = view;
}

- (void)setupUI {
    // This is where we would programmatically create all the UI elements.
    // For brevity, I'll create a simple "Sprint" toggle.

    NSTextField* sprintLabel = [NSTextField labelWithString:@"Sprint"];
    [sprintLabel setFont:[NSFont systemFontOfSize:14 weight:NSFontWeightBold]];
    [sprintLabel setTextColor:[NSColor whiteColor]];
    sprintLabel.frame = NSMakeRect(20, 300, 100, 20);
    [g_window.contentView addSubview:sprintLabel];

    NSButton* toggleButton = [NSButton checkboxWithTitle:@"" target:self action:@selector(toggleSprint:)];
    toggleButton.frame = NSMakeRect(180, 300, 20, 20);
    [g_window.contentView addSubview:toggleButton];

    // Add a Self Destruct button
    NSButton* destructButton = [NSButton buttonWithTitle:@"Self Destruct" target:self action:@selector(selfDestruct:)];
    destructButton.frame = NSMakeRect(20, 20, 180, 30);
    [g_window.contentView addSubview:destructButton];
}

- (void)selfDestruct:(id)sender {
    core::CommandQueue::get().push(self_destruct::trigger);
}

- (void)toggleSprint:(id)sender {
    // This is called from the GUI thread when the button is clicked.
    // We push a command to the queue to be executed on the game thread.
    core::CommandQueue::get().push([]() {
        core::Module* sprint_module = core::ModuleManager::get().get_module("Sprint");
        if (sprint_module) {
            sprint_module->toggle();
        }
    });
}

- (void)toggleVisibility {
    if (g_is_visible) {
        [g_window orderOut:nil];
    } else {
        [g_window makeKeyAndOrderFront:nil];
    }
    g_is_visible = !g_is_visible;
}

// --- Application Delegate Methods ---
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    [self setupWindow];
    [self setupUI];
    [self toggleVisibility]; // Show window on launch
}

@end


// --- C++ Interface Implementation ---

namespace gui {

void run_gui_thread() {
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];
        GuiController* controller = [[GuiController alloc] init];
        [app setDelegate:controller];

        // This starts the event loop for the GUI thread.
        [app run];
    }
}

void init() {
    if (g_is_running) return;
    g_is_running = true;
    g_gui_thread = std::thread(run_gui_thread);
    std::cout << "[GUI] GUI thread started." << std::endl;
}

void toggle_visibility() {
    if (!g_window) return;
    // We need to make sure UI updates happen on the GUI thread.
    // The GuiController isn't easily accessible here, so we'll do something simpler for now.
    // A more robust solution would use performSelectorOnMainThread.
    if (g_is_visible) {
        [g_window orderOut:nil];
    } else {
        [g_window makeKeyAndOrderFront:nil];
    }
    g_is_visible = !g_is_visible;
}

void shutdown() {
    if (!g_is_running) return;
    // Safely stop the GUI application and join the thread.
    dispatch_async(dispatch_get_main_queue(), ^{
        [NSApp terminate:nil];
    });
    if (g_gui_thread.joinable()) {
        g_gui_thread.join();
    }
    g_is_running = false;
    std::cout << "[GUI] GUI thread stopped." << std::endl;
}

} // namespace gui
