#pragma once

#include <mgos.h>
#include <mgos_bt_gap.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BT_GAP_SCAN_GAP MGOS_EVENT_BASE('B', 'T', 'S')

bool mgos_bt_gap_scan_start(const struct mgos_bt_gap_scan_opts *opts);
void mgos_bt_gap_scan_stop();

#ifdef __cplusplus
}
#endif
