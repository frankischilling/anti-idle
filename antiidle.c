#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/ioctl.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

// Linux key codes
#define KEY_LEFT  105
#define KEY_RIGHT 106
#define KEY_UP    103
#define KEY_DOWN  108

static int uinput_fd = -1;

// Timing patterns for more human-like behavior
typedef struct {
    int min_interval;
    int max_interval;
    int weight;  // probability weight
} TimingPattern;

// Multiple timing patterns to vary behavior
TimingPattern timing_patterns[] = {
    {30, 60, 40},    // Short intervals (30-60s) - 40% weight
    {60, 120, 30},   // Medium intervals (1-2min) - 30% weight
    {120, 180, 20},  // Long intervals (2-3min) - 20% weight
    {180, 240, 10}   // Very long (3-4min) - 10% weight
};

int weighted_random_pattern() {
    int total_weight = 0;
    for (int i = 0; i < 4; i++) {
        total_weight += timing_patterns[i].weight;
    }
    
    int random = rand() % total_weight;
    int cumulative = 0;
    
    for (int i = 0; i < 4; i++) {
        cumulative += timing_patterns[i].weight;
        if (random < cumulative) {
            return i;
        }
    }
    return 0;
}

// Generate more human-like delays with micro-variations
int get_human_delay() {
    int pattern_idx = weighted_random_pattern();
    TimingPattern pattern = timing_patterns[pattern_idx];
    
    int base = pattern.min_interval + rand() % (pattern.max_interval - pattern.min_interval);
    
    // Add micro-variations (±5 seconds) for more natural timing
    int variation = (rand() % 11) - 5;  // -5 to +5 seconds
    
    return base + variation;
}

// Generate human-like key press duration (varies between 30-80ms)
int get_press_duration_us() {
    return 30000 + (rand() % 50000);  // 30-80ms
}

// Occasionally add a "double tap" or quick correction
int should_double_tap() {
    return (rand() % 100) < 8;  // 8% chance
}

// Occasionally miss and correct (press opposite then correct)
int should_misclick() {
    return (rand() % 100) < 3;  // 3% chance
}

int init_uinput(void) {
    struct uinput_setup usetup;
    
    uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uinput_fd < 0) {
        perror("ERROR: Cannot open /dev/uinput");
        fprintf(stderr, "You may need to:\n");
        fprintf(stderr, "  1. Run as root, OR\n");
        fprintf(stderr, "  2. Add yourself to 'input' group: sudo usermod -a -G input $USER\n");
        fprintf(stderr, "  3. Load uinput module: sudo modprobe uinput\n");
        return -1;
    }
    
    // Enable key events
    ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY);
    ioctl(uinput_fd, UI_SET_KEYBIT, KEY_LEFT);
    ioctl(uinput_fd, UI_SET_KEYBIT, KEY_RIGHT);
    ioctl(uinput_fd, UI_SET_KEYBIT, KEY_UP);
    ioctl(uinput_fd, UI_SET_KEYBIT, KEY_DOWN);
    
    // Enable mouse events
    ioctl(uinput_fd, UI_SET_EVBIT, EV_REL);
    ioctl(uinput_fd, UI_SET_RELBIT, REL_X);
    ioctl(uinput_fd, UI_SET_RELBIT, REL_Y);
    
    // Setup the device with slightly randomized IDs
    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x046d + (rand() % 10);   // Logitech-like vendor
    usetup.id.product = 0xc52b + (rand() % 100); // Varied product ID
    strcpy(usetup.name, "USB Keyboard");
    
    ioctl(uinput_fd, UI_DEV_SETUP, &usetup);
    ioctl(uinput_fd, UI_DEV_CREATE);
    
    usleep(500000);  // 500ms delay
    
    printf("SUCCESS: uinput device created\n");
    fflush(stdout);
    
    return 0;
}

void cleanup_uinput(void) {
    if (uinput_fd >= 0) {
        ioctl(uinput_fd, UI_DEV_DESTROY);
        close(uinput_fd);
        uinput_fd = -1;
    }
}

void send_key_event(int keycode, int value) {
    struct input_event ev;
    
    if (uinput_fd < 0) return;
    
    memset(&ev, 0, sizeof(ev));
    ev.type = EV_KEY;
    ev.code = keycode;
    ev.value = value;
    gettimeofday(&ev.time, NULL);
    
    write(uinput_fd, &ev, sizeof(ev));
    
    // Sync event
    memset(&ev, 0, sizeof(ev));
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    gettimeofday(&ev.time, NULL);
    
    write(uinput_fd, &ev, sizeof(ev));
}

void send_mouse_rel(int x, int y) {
    struct input_event ev;
    
    if (uinput_fd < 0) return;
    
    if (x != 0) {
        memset(&ev, 0, sizeof(ev));
        ev.type = EV_REL;
        ev.code = REL_X;
        ev.value = x;
        gettimeofday(&ev.time, NULL);
        write(uinput_fd, &ev, sizeof(ev));
    }
    
    if (y != 0) {
        memset(&ev, 0, sizeof(ev));
        ev.type = EV_REL;
        ev.code = REL_Y;
        ev.value = y;
        gettimeofday(&ev.time, NULL);
        write(uinput_fd, &ev, sizeof(ev));
    }
    
    // Sync event
    memset(&ev, 0, sizeof(ev));
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    gettimeofday(&ev.time, NULL);
    write(uinput_fd, &ev, sizeof(ev));
}

int perform_key_action(int keycode) {
    int press_duration = get_press_duration_us();
    
    send_key_event(keycode, 1);
    usleep(press_duration);
    send_key_event(keycode, 0);
    
    return press_duration;  // Return duration in microseconds
}

const char* get_key_name(int keycode) {
    switch(keycode) {
        case KEY_LEFT: return "LEFT";
        case KEY_RIGHT: return "RIGHT";
        case KEY_UP: return "UP";
        case KEY_DOWN: return "DOWN";
        default: return "UNKNOWN";
    }
}

void perform_human_action(int action_num, FILE *log_file) {
    int keys[] = {KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN};
    int num_keys = sizeof(keys) / sizeof(keys[0]);
    
    int chosen_key = keys[rand() % num_keys];
    time_t action_time = time(NULL);
    struct tm *tm_info = localtime(&action_time);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    
    // Simulate occasional misclick/correction
    if (should_misclick()) {
        // Press wrong key briefly
        int wrong_key = keys[rand() % num_keys];
        while (wrong_key == chosen_key) {
            wrong_key = keys[rand() % num_keys];
        }
        int wrong_duration = perform_key_action(wrong_key);
        usleep(100000 + rand() % 200000);  // 100-300ms pause
        // Then correct with intended key
        int correct_duration = perform_key_action(chosen_key);
        
        if (log_file) {
            fprintf(log_file, "[%s] Action #%d: %s (%dms) -> %s (%dms) (corrected)\n", 
                    time_str, action_num, get_key_name(wrong_key), wrong_duration/1000,
                    get_key_name(chosen_key), correct_duration/1000);
        }
        printf("[%s] Action #%d: %s (%dms) -> %s (%dms) (corrected)\n", 
               time_str, action_num, get_key_name(wrong_key), wrong_duration/1000,
               get_key_name(chosen_key), correct_duration/1000);
        fflush(stdout);
        return;
    }
    
    // Normal action
    int press_duration = perform_key_action(chosen_key);
    
    // Occasional double-tap (human uncertainty)
    if (should_double_tap()) {
        usleep(80000 + rand() % 120000);  // 80-200ms between taps
        int second_duration = perform_key_action(chosen_key);
        
        if (log_file) {
            fprintf(log_file, "[%s] Action #%d: %s (%dms) %s (%dms) (double-tap)\n", 
                    time_str, action_num, get_key_name(chosen_key), press_duration/1000,
                    get_key_name(chosen_key), second_duration/1000);
        }
        printf("[%s] Action #%d: %s (%dms) %s (%dms) (double-tap)\n", 
               time_str, action_num, get_key_name(chosen_key), press_duration/1000,
               get_key_name(chosen_key), second_duration/1000);
        fflush(stdout);
    } else {
        if (log_file) {
            fprintf(log_file, "[%s] Action #%d: %s (%dms)\n", 
                    time_str, action_num, get_key_name(chosen_key), press_duration/1000);
        }
        printf("[%s] Action #%d: %s (%dms)\n", 
               time_str, action_num, get_key_name(chosen_key), press_duration/1000);
        fflush(stdout);
    }
    
    // Small random post-action pause
    usleep((10 + rand() % 40) * 1000);  // 10-50ms
}

Window select_window(Display *dpy) {
    printf("Click on the window you want to target...\n");
    fflush(stdout);

    Window root = DefaultRootWindow(dpy);
    Window target = None;
    XGrabPointer(dpy, root, False, ButtonPressMask, GrabModeSync, GrabModeAsync,
                 None, None, CurrentTime);
    XEvent event;
    XAllowEvents(dpy, SyncPointer, CurrentTime);
    XWindowEvent(dpy, root, ButtonPressMask, &event);
    target = event.xbutton.subwindow ? event.xbutton.subwindow : root;
    XUngrabPointer(dpy, CurrentTime);

    return target;
}

char *get_window_name(Display *dpy, Window win) {
    Atom prop = XInternAtom(dpy, "_NET_WM_NAME", False);
    Atom type;
    int format;
    unsigned long nitems, bytes_after;
    unsigned char *prop_value = NULL;

    if (XGetWindowProperty(dpy, win, prop, 0, (~0L), False, AnyPropertyType,
                           &type, &format, &nitems, &bytes_after, &prop_value) == Success) {
        if (prop_value) return (char *)prop_value;
    }
    return "Unknown";
}

void focus_and_raise_window(Display *dpy, Window win) {
    Atom net_active_window = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False);
    Atom net_wm_state = XInternAtom(dpy, "_NET_WM_STATE", False);
    Atom net_wm_state_hidden = XInternAtom(dpy, "_NET_WM_STATE_HIDDEN", False);
    XEvent e;
    
    // Check if window is minimized/hidden
    Atom type;
    int format;
    unsigned long nitems, bytes_after;
    unsigned char *prop_value = NULL;
    
    if (XGetWindowProperty(dpy, win, net_wm_state, 0, (~0L), False, XA_ATOM,
                           &type, &format, &nitems, &bytes_after, &prop_value) == Success) {
        if (prop_value && nitems > 0) {
            Atom *states = (Atom *)prop_value;
            for (unsigned long i = 0; i < nitems; i++) {
                if (states[i] == net_wm_state_hidden) {
                    // Window is minimized, unminimize it
                    e.xclient.type = ClientMessage;
                    e.xclient.serial = 0;
                    e.xclient.send_event = True;
                    e.xclient.display = dpy;
                    e.xclient.window = win;
                    e.xclient.message_type = net_wm_state;
                    e.xclient.format = 32;
                    e.xclient.data.l[0] = 1; // _NET_WM_STATE_REMOVE
                    e.xclient.data.l[1] = net_wm_state_hidden;
                    e.xclient.data.l[2] = 0;
                    e.xclient.data.l[3] = 0;
                    e.xclient.data.l[4] = 0;
                    
                    XSendEvent(dpy, DefaultRootWindow(dpy), False,
                               SubstructureRedirectMask | SubstructureNotifyMask, &e);
                    XFlush(dpy);
                    usleep(100000); // 100ms delay for window manager to process
                    break;
                }
            }
            if (prop_value) XFree(prop_value);
        }
    }
    
    // Raise the window
    XRaiseWindow(dpy, win);
    XFlush(dpy);
    usleep(50000); // 50ms delay
    
    // Focus the window using EWMH
    e.xclient.type = ClientMessage;
    e.xclient.serial = 0;
    e.xclient.send_event = True;
    e.xclient.display = dpy;
    e.xclient.window = win;
    e.xclient.message_type = net_active_window;
    e.xclient.format = 32;
    e.xclient.data.l[0] = 2; // Source indication: application
    e.xclient.data.l[1] = CurrentTime;
    e.xclient.data.l[2] = 0;
    e.xclient.data.l[3] = 0;
    e.xclient.data.l[4] = 0;
    
    XSendEvent(dpy, DefaultRootWindow(dpy), False,
               SubstructureRedirectMask | SubstructureNotifyMask, &e);
    XFlush(dpy);
    usleep(100000); // 100ms delay for focus to take effect
}

int main(int argc, char *argv[]) {
    Display *display;
    FILE *log;
    int startup_delay = 10;
    int test_mode = 0;

    // Parse arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--test") == 0 || strcmp(argv[i], "-t") == 0) {
            test_mode = 1;
        } else {
            startup_delay = atoi(argv[i]);
        }
    }

    // Seed with more entropy
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_sec * 1000000 + tv.tv_usec);

    // Initialize uinput
    if (init_uinput() < 0) {
        return 1;
    }
    atexit(cleanup_uinput);

    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Cannot open X display.\n");
        cleanup_uinput();
        return 1;
    }

    // Select target window
    Window target = select_window(display);
    char *name = get_window_name(display, target);
    printf("Selected window: 0x%lx (%s)\n", target, name);
    fflush(stdout);

    if (test_mode) {
        printf("=== TEST MODE ===\n");
        printf("Sending test inputs for 10 seconds...\n");
        fflush(stdout);
        
        log = fopen("antiidle.log", "a");
        if (log) {
            time_t now = time(NULL);
            struct tm *tm_info = localtime(&now);
            char time_str[64];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
            fprintf(log, "[%s] === TEST MODE STARTED ===\n", time_str);
            fclose(log);
        }
        
        for (int i = 0; i < 10; i++) {
            // Focus and raise the target window before sending input
            focus_and_raise_window(display, target);
            
            log = fopen("antiidle.log", "a");
            printf("Test %d/10\n", i + 1);
            perform_human_action(i + 1, log);
            if (log) fclose(log);
            sleep(1);
        }
        
        log = fopen("antiidle.log", "a");
        if (log) {
            time_t now = time(NULL);
            struct tm *tm_info = localtime(&now);
            char time_str[64];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
            fprintf(log, "[%s] === TEST MODE COMPLETED ===\n", time_str);
            fclose(log);
        }
        
        printf("\nTest complete!\n");
        cleanup_uinput();
        XCloseDisplay(display);
        return 0;
    }

    printf("Anti-idle will start in %d seconds...\n", startup_delay);
    sleep(startup_delay);
    
    time_t session_start = time(NULL);
    struct tm *tm_info = localtime(&session_start);
    char start_time_str[64];
    strftime(start_time_str, sizeof(start_time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    
    log = fopen("antiidle.log", "a");
    if (log) {
        fprintf(log, "\n[%s] === SESSION STARTED ===\n", start_time_str);
        fprintf(log, "[%s] Target window: 0x%lx (%s)\n", start_time_str, target, name);
        fclose(log);
    }
    
    printf("Starting enhanced anti-idle with human-like patterns...\n");
    printf("Session started at: %s\n", start_time_str);
    printf("Target window: %s\n", name);
    fflush(stdout);

    int action_count = 0;
    int session_actions = 0;
    int total_keys_pressed = 0;

    while (1) {
        action_count++;
        session_actions++;
        
        // Calculate elapsed time at the START of each iteration (before action)
        time_t now = time(NULL);
        int session_duration = (now - session_start) / 60;
        int session_seconds = (now - session_start) % 60;
        
        // Focus and raise the target window before sending input
        focus_and_raise_window(display, target);
        
        // Open log file for this action
        log = fopen("antiidle.log", "a");
        
        // Perform human-like action (now logs internally)
        perform_human_action(action_count, log);
        total_keys_pressed++;
        
        if (log) fclose(log);
        
        // Calculate next delay with human-like variance
        int delay = get_human_delay();
        
        // Occasional longer break (simulate tab-out/distraction)
        // ~5% chance every 10 actions
        if (action_count > 10 && (rand() % 100) < 5) {
            int extra_delay = 60 + rand() % 120;  // Extra 1-3 minutes
            delay += extra_delay;
            
            struct tm *tm_info = localtime(&now);
            char time_str[64];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
            
            log = fopen("antiidle.log", "a");
            if (log) {
                fprintf(log, "[%s] Extended break: +%ds (total: %ds)\n", 
                        time_str, extra_delay, delay);
                fclose(log);
            }
            printf("[%s] Extended break: +%ds (total: %ds)\n", 
                   time_str, extra_delay, delay);
            fflush(stdout);
        }
        
        // Ensure we stay under timeout (240s = 4 minutes)
        if (delay > 240) {
            delay = 180 + rand() % 60;  // 3-4 minutes
        }

        // Calculate next action time
        time_t next_action_time = now + delay;
        
        // Format next action time
        struct tm *next_tm = localtime(&next_action_time);
        char next_time_str[64];
        strftime(next_time_str, sizeof(next_time_str), "%H:%M:%S", next_tm);
        
        struct tm *tm_info = localtime(&now);
        char time_str[64];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
        
        // Always log stats after every action
        log = fopen("antiidle.log", "a");
        if (log) {
            fprintf(log, "[%s] Stats: %dm %ds elapsed | %d actions | %d keys | Next action at %s (in %ds)\n", 
                    time_str, session_duration, session_seconds, session_actions, total_keys_pressed, next_time_str, delay);
            fclose(log);
        }
        
        // Always print stats to console after every action
        printf("[%s] Stats: %dm %ds elapsed | %d actions | %d keys | Next action at %s (in %ds)\n", 
               time_str, session_duration, session_seconds, session_actions, total_keys_pressed, next_time_str, delay);
        fflush(stdout);

        sleep(delay);
    }

    XCloseDisplay(display);
    cleanup_uinput();
    return 0;
}