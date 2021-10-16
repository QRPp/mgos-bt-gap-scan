#pragma once

#include <mgos.h>
#include <mgos_bt_gap.h>

#ifdef __cplusplus
extern "C" {
#endif

bool mgos_bt_gap_scan_start(const struct mgos_bt_gap_scan_opts *opts);
void mgos_bt_gap_scan_stop();

#ifdef __cplusplus
}
#endif
