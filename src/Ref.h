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

#define entree_throttle 0
#define entree_yaw 1
#define entree_roll 2
#define entree_pitch 3

#define pid_pitch 0
#define pid_roll 1
#define pid_yaw 2

#define rates_pitch 0
#define rates_roll 1
#define rates_yaw 2

// switch values in the radio
// in order to use my recieve wich has a builtin mode to have 6 different values on one chanel
#define cmd_mode_delta 50
#define cmd_mode_1 921
#define cmd_mode_2 1152
#define cmd_mode_3 1383
#define cmd_mode_4 1615
#define cmd_mode_5 1846
#define cmd_mode_6 2077

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
//d
//i
#define status_gains_rate_kp 13
#define status_gains_rate_kd 14
#define status_gains_rate_ki 15

#define status_experience_mode 16
#define status_experience_time 17
#define status_filter_mode 18
#define status_filter_param 19

// number of items
#define status_length 20

#define order_Saving 0
#define orders_DisplayGains 1

#define grav 9.81

#define PI 3.1415926535
// IMU
   /*
   Filter :
    0 : no
    1 : acceleration
    2 : gyration
    3 : both
    */
   
#define IMU_Filter_usage_none 0
#define IMU_Filter_usage_acc 1
#define IMU_Filter_usage_gyr 2
#define IMU_Filter_usage_both 3

#define exp_time_start -4
#define exp_time_motor_start -3
#define exp_time_cmd_start 0
#define exp_time_cmd_stop 7.5
#define exp_time_motor_stop 7.5
#define exp_time_end 10
