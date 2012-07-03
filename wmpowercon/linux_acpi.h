#ifndef _LINUX_ACPI_H
#define _LINUX_ACPI_H

typedef struct {
  int discharging;
  int full;
  int percent;
} battery_info;

battery_info get_battery_info(FILE *sysfs_bat_fp);

#endif
