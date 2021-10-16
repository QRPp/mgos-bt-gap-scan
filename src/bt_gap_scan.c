#include <mgos.h>
#include <mgos_bt_gap.h>
#include <mgos_config.h>

#include <mgos-helpers/log.h>

#include <mgos_bt_gap_scan.h>

struct mgos_bt_gap_scan_opts bgs_opts;

static void scan_start(int ev, void *ev_data, void *userdata) {
  if (ev != MGOS_BT_GAP_EVENT_SCAN_STOP) return;
  return (void) TRY_GT(mgos_bt_gap_scan, &bgs_opts);
err:
  mgos_bt_gap_scan_stop();
}

bool mgos_bt_gap_scan_start(const struct mgos_bt_gap_scan_opts *opts) {
  if (opts)
    bgs_opts = *opts;
  else {
    bgs_opts.duration_ms = 59999;
    bgs_opts.active = mgos_sys_config_get_bt_scan_active();
  }
  TRY_GT(mgos_event_add_group_handler, MGOS_BT_GAP_EVENT_SCAN_STOP, scan_start,
         NULL);
  TRY_GT(mgos_bt_gap_scan, &bgs_opts);
  return true;

err:
  mgos_bt_gap_scan_stop();
  return false;
}

void mgos_bt_gap_scan_stop() {
  mgos_event_remove_group_handler(MGOS_BT_GAP_EVENT_SCAN_STOP, scan_start,
                                  NULL);
}

bool mgos_bt_gap_scan_init() {
  if (mgos_sys_config_get_bt_enable() && mgos_sys_config_get_bt_scan_loop())
    mgos_bt_gap_scan_start(NULL);
  return true;
}
