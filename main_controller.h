#pragma once
extern void ctrl_handle_events();
extern void ctrl_key(int key, int scancode, int action, int mods);
extern void ctrl_scroll(double xoff, double yoff);
extern void ctrl_cursor(double x, double y);
extern void ctrl_get_deltas(float& dx, float& dy, float& dz, float& dt, float& dp);