/*
 *  wmpowercon-4.0
 *
 *    A WindowMaker dockable application that allows laptop users
 *    to graphically monitor the status of their power source. Based on the
 *    GPL source from wmapm-3.1.
 *
 *  Previous versions written and released under GPL by:
 *    Chris D. Faulhaber <jedgar@fxp.org>. WMAPM Version 3.0
 *    Michael G. Henderson <mghenderson@lanl.gov>. WMAPM Version 2.0
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program (see the file COPYING); if not, write to the
 *  Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 *
 *  Portions of code derived from:
 *      wmmon           : (c) 1998 Martijn Pieterse (pieterse@xs4all.nl) and
 *                                 Antoine Nulle (warp@xs4all.nl) 
 *
 *  Thanx to Timecop <timecop@linuxwarez.com> for pointing out/helping to
 *  Toggle fix the meter mismatch.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <X11/X.h>
#include <X11/xpm.h>
#include "wmpowercon.h"
#include "../wmgeneral/wmgeneral.h"
#include "wmapm_master.xpm"
#include "wmapm_mask.xbm"
#include "linux_acpi.h"

void displayTime(int time_in_minutes);
void displayGraph(int percent);
void displayChargeStatus(int ac_line_on);
void displayLeds(int red, int yellow, int green);
void pressEvent(XButtonEvent *xev);

/*  
 *   main  
 */
int main(int argc, char *argv[]) {
  
  // BEGIN hardcoded test params
  int     ac_line_status     = 1;
  int     battery_status     = 3;   // Battery charging, 2 = critical, 1 = low, 3 = charged
  int     battery_time       = 2000; // Remaining time in minutes
  int     battery_percentage = 0;
  // END hardcoded test params

  XEvent  event;

  // Init the base window    
  openXwindow(argc, argv, wmapm_master, wmapm_mask_bits, wmapm_mask_width, wmapm_mask_height);

  while (1) {
    battery_info res = get_battery_info(fopen("/sys/class/power_supply/BAT1/uevent", "r"));

    displayGraph(res.percent);
    displayChargeStatus(!res.discharging);
    displayTime(battery_time);
    displayLeds(1, 1, 1); 

    battery_percentage == 100 ? (battery_percentage = 0) : (battery_percentage++);

    // Process pending X events
    while (XPending(display)) {
      XNextEvent(display, &event);
      switch(event.type){
        case Expose:
          RedrawWindow();
          break;
        case ButtonPress:
          pressEvent(&event.xbutton);
          break;
        case ButtonRelease:
          break;
      }
    }
  
    // Redraw and wait for next update 
    RedrawWindow();
    usleep(100000);
  }
}

/*
 *  displayTime() -- Display the given time in minutes on the dockapp
 */
void displayTime(int time_in_minutes) {
  if (time_in_minutes >= 1440) { // Time too large, no charge
    copyXPMArea(83, 106, 41, 9, 15, 7);
  
  } else if (time_in_minutes >= 0) {
    int hour = time_in_minutes / 60;
    int min  = time_in_minutes % 60;

    copyXPMArea((hour / 10) * 7 + 5, 93, 7, 9, 21, 7);  // Show 10's (hour)
    copyXPMArea((hour % 10) * 7 + 5, 93, 7, 9, 29, 7);  // Show 1's (hour) 
    copyXPMArea(76, 93, 2, 9, 38, 7);                   // colon  
    copyXPMArea((min / 10) * 7 + 5, 93, 7, 9, 42, 7);   // Show 10's (min)
    copyXPMArea((min % 10) * 7 + 5, 93, 7, 9, 50, 7);   // Show 1's (min)  
  }
}

/*
 *  displayGraph() -- Display the given percentage on the dockapp bar graph
 */
void displayGraph(int percent) {
  copyXPMArea(76, 81, 19, 7, 7, 34);                          // Show Default %
  copyXPMArea(66, 31, 49, 9, 7, 21);                          // Show Default Meter
    
  if (percent == 100) {
    copyXPMArea(15, 81, 1, 7,  7, 34);                        // If 100%, show 100%
    copyXPMArea( 5, 81, 6, 7,  9, 34);
    copyXPMArea( 5, 81, 6, 7, 15, 34);
    copyXPMArea(64, 81, 7, 7, 21, 34);                        // Show '%'
    copyXPMArea(66, 42, 49, 9, 7, 21);                        // Show Meter
  } else {
    if (percent >= 10)
      copyXPMArea((percent / 10) * 6 + 4, 81, 6, 7,  9, 34);  // Show 10's
    
    copyXPMArea((percent % 10) * 6 + 4, 81, 6, 7, 15, 34);    // Show 1's
    copyXPMArea(64, 81, 7, 7, 21, 34);                        // Show '%'

    // Show meter
    int k = percent * 49 / 100;
    copyXPMArea(66, 42, k, 9, 7, 21);
        
    (k % 2) ? copyXPMArea(66+k-1, 52, 1, 9, 7+k-1, 21) :  copyXPMArea(66+k, 52, 1, 9, 7+k, 21);
  }
}

/*
 *  displayChargeStatus() -- Display whether we're charging or discharging
 */
void displayChargeStatus(int ac_line_on) {
  if ( ac_line_on ) { // Battery is charging
    copyXPMArea(68,  6, 26, 7, 31, 35); // AC line symbol on
    copyXPMArea(98,  6, 5, 7,  6,  7);  // "bright C" charge status
    copyXPMArea(75, 81, 1, 2, 17,  9);  // Change default + to -
    copyXPMArea(75, 81, 1, 2, 17, 12);  // Change default + to -
  } else {
    copyXPMArea(68, 20, 26, 7, 31, 35); // Battery symbol on
    copyXPMArea(104, 6,  5, 7,  6, 7);  // Default "dim C" charge status
    copyXPMArea(83, 93, 41, 9, 15, 7);  // Default +00:00 time
  }
}

/*
 *  displayLeds() -- Update the three LEDs at the bottom of the dockapp
 */
void displayLeds(int red, int yellow, int green) {
  red    ? copyXPMArea(95, 68, 4, 4, 24, 51)  : copyXPMArea(75, 68, 4, 4, 24, 51);
  yellow ? copyXPMArea(101, 68, 4, 4, 30, 51) : copyXPMArea(81,  68, 4, 4, 30, 51);
  green  ? copyXPMArea(107, 68, 4, 4, 36, 51) : copyXPMArea(87,  68, 4, 4, 36, 51);
}

/*  
 *  pressEvent() -- Handle button presses from X
 */
void pressEvent(XButtonEvent *xev){

  int x = xev->x;
  int y = xev->y;

  if (x >= 5 && y >= 48 && x <= 17 && y <= 58) {
    copyXPMArea(5, 106, 13, 11, 5, 48);   // S Button pressed
    printf("S button pushed\n");
    RedrawWindow();
    usleep(50000);
    copyXPMArea(42, 106, 13, 11, 5, 48);  // S Button released
    RedrawWindow();
  } else if (x >= 46 && y >= 48 && x <= 58 && y <= 58) {
    copyXPMArea(21, 106, 13, 11, 46, 48);   // Z Button pressed
    printf("Z button pushed\n");
    RedrawWindow();
    usleep(50000);
    copyXPMArea(57, 106, 13, 11, 46, 48);   // Z Button released
    RedrawWindow();
  }
}
