#define cmd_throttle 2
#define cmd_pitch 1
#define cmd_roll 0
#define cmd_yaw 3
#define cmd_arming 8
#define cmd_flight_mode 5
#define cmd_kp 6
#define cmd_kd 4
#define cmd_ki 7

#define pos_x 0
#define pos_y 1
#define pos_z 2

#define pid_pitch 0
#define pid_roll 1
#define pid_yaw 2

#define rates_pitch 0
#define rates_roll 1
#define rates_yaw 2


// status
#define status_Saving 0
#define status_DisplayGains 1

//gps
#define status_gps_fix 2
#define status_gps_get_fix 3
#define status_gps_latitude 4
#define status_gps_longitude 5
#define status_gps_height 6
#define status_gps_h_accuracy 7
#define status_gps_v_accuracy 8

// baro
#define status_baro_pressure 9
#define status_baro_temperature 10

// gains
#define status_stab_mode 11
#define status_gains_atti_kp 12
#define status_gains_rate_kp 13
#define status_gains_rate_kd 14

// number of items
#define status_length 14

#define order_Saving 0
#define orders_DisplayGains 1

#define grav 9.81
#define pi 3.14159