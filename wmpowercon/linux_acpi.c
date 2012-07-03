#include <stdio.h>
#include <stdlib.h>
#include "linux_acpi.h"

battery_info get_battery_info(FILE *sysfs_bat_fp) {
  battery_info results = {0, 0};

  int full_charge, current_charge;
  long fileLen;
  char *buf, *line, *delimiter = "\n";
  char charging_state[64];

  // Read the file into a char buffer
  fseek(sysfs_bat_fp, 0L, SEEK_END);
  fileLen = ftell(sysfs_bat_fp);
  rewind(sysfs_bat_fp);

  buf = calloc(fileLen + 1, sizeof(char));
  fread(buf, fileLen, 1, sysfs_bat_fp);
  line = calloc(255, sizeof(char));
  line = strtok( buf, delimiter );

  while (line != NULL) {
    if (strncmp(line, "POWER_SUPPLY_CHARGE_FULL=", 25) == 0)
      sscanf(line, "POWER_SUPPLY_CHARGE_FULL=%d", &full_charge);

    if (strncmp(line, "POWER_SUPPLY_CHARGE_NOW=", 24) == 0)
      sscanf(line, "POWER_SUPPLY_CHARGE_NOW=%d", &current_charge);

    if (strncmp(line, "POWER_SUPPLY_STATUS=", 20) == 0) {
      sscanf(line, "POWER_SUPPLY_STATUS=%63s", charging_state);
      results.discharging = !strcmp(charging_state, "Discharging");
    }

    line = strtok( NULL, delimiter );
  }

  results.percent = (current_charge / (double)full_charge) * 100;
  free(buf);
  free(line);
  fclose(sysfs_bat_fp);

  return results;
}
