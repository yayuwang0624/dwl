/* Taken from https://github.com/djpohly/dwl/issues/466 */
#define COLOR(hex)    { ((hex >> 24) & 0xFF) / 255.0f, \
                        ((hex >> 16) & 0xFF) / 255.0f, \
                        ((hex >> 8) & 0xFF) / 255.0f, \
                        (hex & 0xFF) / 255.0f }
/* appearance */
static const int sloppyfocus               = 1;  /* focus follows mouse */
static const int bypass_surface_visibility = 0;  /* 1 means idle inhibitors will disable idle tracking even if it's surface isn't visible  */
static const unsigned int borderpx         = 1;  /* border pixel of windows */
static const float rootcolor[]             = COLOR(0x222222ff);
static const float bordercolor[]           = COLOR(0x444444ff);
static const float focuscolor[]            = COLOR(0x6b88caff);
static const float urgentcolor[]           = COLOR(0xff0000ff);
/* To conform the xdg-protocol, set the alpha to zero to restore the old behavior */
static const float fullscreen_bg[]         = {0.1f, 0.1f, 0.1f, 1.0f}; /* You can also use glsl colors */

/* tagging - TAGCOUNT must be no greater than 31 */
#define TAGCOUNT (9)

/* logging */
static int log_level = WLR_ERROR;

static const Rule rules[] = {
	/* app_id                   title  tags mask    isfloating  monitor */
	{ "Gimp",                   NULL,  1 << 4,      1,           -1 },
	{ "Emacs",                  NULL,  1 << 1,      0,            0 },
	{ "Google-chrome",          NULL,  1 << 2,      0,           -1 },
	{ "firefox",                NULL,  1 << 2,      0,            1 },
	{ "zen",                    NULL,  1 << 2,      0,            0 },
	{ "netease-cloud-music",    NULL,  1 << 6,      0,           -1 },
	{ "sayonara",               NULL,  1 << 6,      0,            1 },
	{ "discord",                NULL,  1 << 7,      0,            1 },
	{ "Slack",                  NULL,  1 << 7,      0,            1 },
};

/* layout(s) */
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* monitors */
/* Adopted from the autorandr profile. NOTE: wlroots matches DRM connector
 * names, which differ from X11/autorandr names — autorandr's "HDMI-0" is
 * "HDMI-A-1" here and "DP-0" is "DP-1" (verified via /sys/class/drm). HDMI-A-1
 * is portrait (rotated left) at the origin; DP-1 is landscape (the primary) to
 * its right. Positions are layout pixels at scale 1: HDMI-A-1 rotated is 2160
 * px wide, so DP-1 begins at x=2160.
 * If HDMI-A-1 rotates the wrong way, swap _90 for _270. */
static const MonitorRule monrules[] = {
	/* name        mfact  nmaster scale layout       rotate/reflect              x     y */
	{ "HDMI-A-1",  0.55f, 1,      1,    &layouts[0], WL_OUTPUT_TRANSFORM_90,     0,    0 },
	{ "DP-1",      0.55f, 1,      1,    &layouts[0], WL_OUTPUT_TRANSFORM_NORMAL, 2160, 0 },
	{ NULL,        0.55f, 1,      1,    &layouts[0], WL_OUTPUT_TRANSFORM_NORMAL, -1,   -1 },
};

/* keyboard */
static const struct xkb_rule_names xkb_rules = {
	/* can specify fields: rules, model, layout, variant, options */
	.options = NULL,
};

static const int repeat_rate = 25;
static const int repeat_delay = 600;

/* Trackpad */
static const int tap_to_click = 1;
static const int tap_and_drag = 1;
static const int drag_lock = 1;
static const int natural_scrolling = 0;
static const int disable_while_typing = 1;
static const int left_handed = 0;
static const int middle_button_emulation = 0;
static const enum libinput_config_scroll_method scroll_method = LIBINPUT_CONFIG_SCROLL_2FG;
static const enum libinput_config_click_method click_method = LIBINPUT_CONFIG_CLICK_METHOD_BUTTON_AREAS;
static const uint32_t send_events_mode = LIBINPUT_CONFIG_SEND_EVENTS_ENABLED;
static const enum libinput_config_accel_profile accel_profile = LIBINPUT_CONFIG_ACCEL_PROFILE_ADAPTIVE;
static const double accel_speed = 0.0;
static const enum libinput_config_tap_button_map button_map = LIBINPUT_CONFIG_TAP_MAP_LRM;

/* If you want to use the windows key for MODKEY, use WLR_MODIFIER_LOGO */
#define MODKEY WLR_MODIFIER_LOGO

#define TAGKEYS(KEY,SKEY,TAG) \
	{ MODKEY,                    KEY,            view,            {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_CTRL,  KEY,            toggleview,      {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_SHIFT, SKEY,           tag,             {.ui = 1 << TAG} }, \
	{ MODKEY|WLR_MODIFIER_CTRL|WLR_MODIFIER_SHIFT,SKEY,toggletag, {.ui = 1 << TAG} }

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static const char *termcmd[]    = { "kitty", NULL };
static const char *menucmd[]    = { "rofi", "-show", "run", NULL };

static const Key keys[] = {
	/* Note that Shift changes certain key codes: c -> C, 2 -> at, etc. */
	/* modifier                              key                          function         argument */
	{ MODKEY,                                XKB_KEY_p,                   spawn,           {.v = menucmd} },
	{ MODKEY|WLR_MODIFIER_SHIFT,             XKB_KEY_P,                   spawn,           SHCMD("SUDO_ASKPASS=~/.config/rofi/sudo-rofi.sh rofi -show run -run-command \"sudo -A {cmd}\"") },
	{ MODKEY|WLR_MODIFIER_SHIFT,             XKB_KEY_Return,              spawn,           {.v = termcmd} },

	/* bar — toggle dwlb visibility (dwm's Mod+b togglebar) */
	{ MODKEY,                                XKB_KEY_b,                   spawn,           SHCMD("dwlb -toggle-visibility all") },

	/* focus / stack manipulation */
	{ MODKEY,                                XKB_KEY_j,                   focusstack,      {.i = +1} },
	{ MODKEY,                                XKB_KEY_k,                   focusstack,      {.i = -1} },
	{ MODKEY|WLR_MODIFIER_SHIFT,             XKB_KEY_J,                   movestack,       {.i = +1} },
	{ MODKEY|WLR_MODIFIER_SHIFT,             XKB_KEY_K,                   movestack,       {.i = -1} },
	{ MODKEY,                                XKB_KEY_Tab,                 focusstack,      {.i = +1} },
	{ MODKEY|WLR_MODIFIER_SHIFT,             XKB_KEY_ISO_Left_Tab,        focusstack,      {.i = -1} },

	/* master/stack sizing */
	{ MODKEY,                                XKB_KEY_i,                   incnmaster,      {.i = +1} },
	{ MODKEY,                                XKB_KEY_d,                   incnmaster,      {.i = -1} },
	{ MODKEY,                                XKB_KEY_h,                   setmfact,        {.f = -0.05f} },
	{ MODKEY,                                XKB_KEY_l,                   setmfact,        {.f = +0.05f} },
	{ MODKEY,                                XKB_KEY_Return,              zoom,            {0} },

	/* window operations */
	{ MODKEY|WLR_MODIFIER_SHIFT,             XKB_KEY_C,                   killclient,      {0} },
	{ MODKEY,                                XKB_KEY_f,                   togglefloating,  {0} },
	{ MODKEY,                                XKB_KEY_e,                   togglefullscreen,{0} },

	/* layouts */
	{ MODKEY,                                XKB_KEY_t,                   setlayout,       {.v = &layouts[0]} },
	{ MODKEY,                                XKB_KEY_m,                   setlayout,       {.v = &layouts[2]} },
	{ MODKEY,                                XKB_KEY_space,               cyclelayout,     {.i = +1} },
	{ MODKEY|WLR_MODIFIER_CTRL,              XKB_KEY_comma,               cyclelayout,     {.i = -1} },
	{ MODKEY|WLR_MODIFIER_CTRL,              XKB_KEY_period,              cyclelayout,     {.i = +1} },

	/* tag navigation */
	{ MODKEY,                                XKB_KEY_0,                   view,            {.ui = ~0} },
	{ MODKEY|WLR_MODIFIER_SHIFT,             XKB_KEY_parenright,          tag,             {.ui = ~0} },
	{ MODKEY,                                XKB_KEY_Left,                shiftview,       {.i = -1} },
	{ MODKEY,                                XKB_KEY_Right,               shiftview,       {.i = +1} },
	{ MODKEY|WLR_MODIFIER_SHIFT,             XKB_KEY_Left,                shiftboth,       {.i = -1} },
	{ MODKEY|WLR_MODIFIER_SHIFT,             XKB_KEY_Right,               shiftboth,       {.i = +1} },

	/* multi-monitor */
	{ MODKEY|WLR_MODIFIER_CTRL,              XKB_KEY_Left,                focusmon,        {.i = WLR_DIRECTION_LEFT} },
	{ MODKEY|WLR_MODIFIER_CTRL,              XKB_KEY_Right,               focusmon,        {.i = WLR_DIRECTION_RIGHT} },
	{ MODKEY|WLR_MODIFIER_CTRL,              XKB_KEY_Tab,                 focusmon,        {.i = WLR_DIRECTION_RIGHT} },
	{ MODKEY|WLR_MODIFIER_CTRL|WLR_MODIFIER_SHIFT, XKB_KEY_Left,          tagmon,          {.i = WLR_DIRECTION_LEFT} },
	{ MODKEY|WLR_MODIFIER_CTRL|WLR_MODIFIER_SHIFT, XKB_KEY_Right,         tagmon,          {.i = WLR_DIRECTION_RIGHT} },
	{ MODKEY,                                XKB_KEY_comma,               focusmon,        {.i = WLR_DIRECTION_LEFT} },
	{ MODKEY,                                XKB_KEY_period,              focusmon,        {.i = WLR_DIRECTION_RIGHT} },
	{ MODKEY|WLR_MODIFIER_SHIFT,             XKB_KEY_less,                tagmon,          {.i = WLR_DIRECTION_LEFT} },
	{ MODKEY|WLR_MODIFIER_SHIFT,             XKB_KEY_greater,             tagmon,          {.i = WLR_DIRECTION_RIGHT} },

	{ MODKEY,                                XKB_KEY_slash,               spawn,           SHCMD("notify-send --icon=$HOME/.config/dunst/monitor.png --urgency low Monitor") },

	TAGKEYS(          XKB_KEY_1, XKB_KEY_exclam,                          0),
	TAGKEYS(          XKB_KEY_2, XKB_KEY_at,                              1),
	TAGKEYS(          XKB_KEY_3, XKB_KEY_numbersign,                      2),
	TAGKEYS(          XKB_KEY_4, XKB_KEY_dollar,                          3),
	TAGKEYS(          XKB_KEY_5, XKB_KEY_percent,                         4),
	TAGKEYS(          XKB_KEY_6, XKB_KEY_asciicircum,                     5),
	TAGKEYS(          XKB_KEY_7, XKB_KEY_ampersand,                       6),
	TAGKEYS(          XKB_KEY_8, XKB_KEY_asterisk,                        7),
	TAGKEYS(          XKB_KEY_9, XKB_KEY_parenleft,                       8),

	/* quit (RESTARTSIG in dwm becomes plain quit — no compositor restart possible in Wayland) */
	{ MODKEY,                                XKB_KEY_q,                   quit,            {0} },
	{ MODKEY|WLR_MODIFIER_SHIFT,             XKB_KEY_Q,                   quit,            {0} },
	{ MODKEY|WLR_MODIFIER_CTRL|WLR_MODIFIER_SHIFT, XKB_KEY_Q,             quit,            {0} },

	/* screenshot / media keys */
	{ MODKEY,                                XKB_KEY_Print,               spawn,           SHCMD("grim -g \"$(slurp)\" - | wl-copy") },
	{ 0,                                     XKB_KEY_XF86AudioRaiseVolume,spawn,           SHCMD("pactl set-sink-volume @DEFAULT_SINK@ +2%") },
	{ 0,                                     XKB_KEY_XF86AudioLowerVolume,spawn,           SHCMD("pactl set-sink-volume @DEFAULT_SINK@ -2%") },
	{ 0,                                     XKB_KEY_XF86AudioMute,       spawn,           SHCMD("pactl set-sink-mute @DEFAULT_SINK@ toggle") },

	/* Ctrl-Alt-Backspace and Ctrl-Alt-Fx used to be handled by X server */
	{ WLR_MODIFIER_CTRL|WLR_MODIFIER_ALT,    XKB_KEY_Terminate_Server,    quit,            {0} },
#define CHVT(n) { WLR_MODIFIER_CTRL|WLR_MODIFIER_ALT,XKB_KEY_XF86Switch_VT_##n, chvt, {.ui = (n)} }
	CHVT(1), CHVT(2), CHVT(3), CHVT(4), CHVT(5), CHVT(6),
	CHVT(7), CHVT(8), CHVT(9), CHVT(10), CHVT(11), CHVT(12),
};

static const Button buttons[] = {
	{ MODKEY, BTN_LEFT,   moveresize,     {.ui = CurMove} },
	{ MODKEY, BTN_MIDDLE, togglefloating, {0} },
	{ MODKEY, BTN_RIGHT,  moveresize,     {.ui = CurResize} },
};
