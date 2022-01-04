#include <mgos.h>
#include <mgos_bt_gap.h>
#include <mgos_config.h>
#include <mgos_rpc.h>

#include <mgos-helpers/log.h>
#include <mgos-helpers/tmr.h>

#include <mgos_bt_gap_scan.h>

static struct mgos_bt_gap_scan_opts bgs_opts;
static mgos_timer_id bgs_tmr = MGOS_INVALID_TIMER_ID;

#define SCAN_RETRY_S 1
static void scan_start(void *userdata) {
  if (mgos_bt_gap_scan(&bgs_opts))
    bgs_tmr = MGOS_INVALID_TIMER_ID;
  else {
    FNERR(CALL_FAILED_FMT ", wait %u s", "mgos_bt_gap_scan", SCAN_RETRY_S);
    MGOS_TMR_SET(bgs_tmr, SCAN_RETRY_S * 1000, 0, scan_start, NULL);
  }
}

static void scan_restart(int ev, void *ev_data, void *userdata) {
  if (ev != MGOS_BT_GAP_EVENT_SCAN_STOP) return;
  mgos_event_trigger(BT_GAP_SCAN_GAP, ev_data);
  scan_start(userdata);
}

bool mgos_bt_gap_scan_start(const struct mgos_bt_gap_scan_opts *opts) {
  if (!mgos_sys_config_get_bt_enable()) return false;
  if (opts)
    bgs_opts = *opts;
  else {
    bgs_opts.duration_ms = 5000;
    bgs_opts.active = mgos_sys_config_get_bt_scan_active();
    bgs_opts.interval_ms = 96;
    bgs_opts.window_ms = 48;
  }
  TRY_RETF(mgos_event_add_handler, MGOS_BT_GAP_EVENT_SCAN_STOP, scan_restart,
           NULL);
  scan_start(NULL);
  return true;
}

void mgos_bt_gap_scan_stop() {
  MGOS_TMR_CLR(bgs_tmr);
  mgos_event_remove_handler(MGOS_BT_GAP_EVENT_SCAN_STOP, scan_restart, NULL);
}

static void bt_scan_start_handler(struct mg_rpc_request_info *ri, void *cb_arg,
                                  struct mg_rpc_frame_info *fi,
                                  struct mg_str args) {
  mgos_bt_gap_scan_stop();
  mg_rpc_send_responsef(ri, "%B", mgos_bt_gap_scan_start(NULL));
}

static void bt_scan_stop_handler(struct mg_rpc_request_info *ri, void *cb_arg,
                                 struct mg_rpc_frame_info *fi,
                                 struct mg_str args) {
  mgos_bt_gap_scan_stop();
  mg_rpc_send_responsef(ri, NULL);
}

bool mgos_bt_gap_scan_init() {
  mgos_event_register_base(BT_GAP_SCAN_GAP, "bt-gap");
  if (mgos_sys_config_get_bt_scan_loop()) mgos_bt_gap_scan_start(NULL);
  mg_rpc_add_handler(mgos_rpc_get_global(), "BT.ScanStart", "",
                     bt_scan_start_handler, NULL);
  mg_rpc_add_handler(mgos_rpc_get_global(), "BT.ScanStop", "",
                     bt_scan_stop_handler, NULL);
  return true;
}
