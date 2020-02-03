#include <stdlib.h>
#include <stdio.h>
#include "rvrdefs.h"
#include "rvrio.h"
#include "bounds.h"

#define ENABLE_GYRO_MAX_NOTIFY                          0x0F
#define GYRO_MAX_NOTIFY                                 0x10
#define RESET_LOCATOR_X_AND_Y                           0x13
#define SET_LOCATOR_FLAGS                               0x17
#define GET_BOT_TO_BOT_INFRARED_READINGS                0x22
#define GET_RGBC_SENSOR_VALUES                          0x23
#define START_ROBOT_TO_ROBOT_INFRARED_BROADCASTING      0x27
#define START_ROBOT_TO_ROBOT_INFRARED_FOLLOWING         0x28
#define STOP_ROBOT_TO_ROBOT_INFRARED_BROADCASTING       0x29
#define ROBOT_TO_ROBOT_INFRARED_MESSAGE_RECEIVED_NOTIFY 0x2c
#define GET_AMBIENT_LIGHT_SENSOR_VALUE                  0x30
#define STOP_ROBOT_TO_ROBOT_INFRARED_FOLLOWING          0x32
#define START_ROBOT_TO_ROBOT_INFRARED_EVADING           0x33
#define STOP_ROBOT_TO_ROBOT_INFRARED_EVADING            0x34
#define ENABLE_COLOR_DETECTION_NOTIFY                   0x35
#define COLOR_DETECTION_NOTIFY                          0x36
#define GET_CURRENT_DETECTED_COLOR_READING              0x37
#define ENABLE_COLOR_DETECTION                          0x38
#define CONFIGURE_STREAMING_SERVICE                     0x39
#define START_STREAMING_SERVICE                         0x3a
#define STOP_STREAMING_SERVICE                          0x3b
#define CLEAR_STREAMING_SERVICE                         0x3c
#define STREAMING_SERVICE_DATA_NOTIFY                   0x3d
#define ENABLE_ROBOT_INFRARED_MESSAGE_NOTIFY            0x3e
#define SEND_INFRARED_MESSAGE                           0x3f
#define GET_MOTOR_TEMPERATURE                           0x4a
#define GET_MOTOR_THERMAL_PROTECTION_STATUS             0x4b
#define ENABLE_MOTOR_THERMAL_PROTECTION_STATUS_NOTIFY   0x4c
#define MOTOR_THERMAL_PROTECTION_STATUS_NOTIFY          0x4d
#define SENSOR_DATA                                     0xFF

#define LEFT_MOTOR_INDEX   0x00
#define RIGHT_MOTOR_INDEX  0x01

#define THERMAL_PROTECTION_STATUS_OK       0x00
#define THERMAL_PROTECTION_STATUS_WARN     0x01
#define THERMAL_PROTECTION_STATUS_CRITICAL 0x02

#define STREAMING_DATA_SIZE_EIGHT_BIT      0x00
#define STREAMING_DATA_SIZE_SIXTEEN_BIT    0x01
#define STREAMING_DATA_SIZE_THIRTY_TWO_BIT 0x02

#define GYRO_MAX_BITMASK_NONE         0x00
#define GYRO_MAX_BITMASK_MAX_PLUS_X   0x01
#define GYRO_MAX_BITMASK_MAX_MINUS_X  0x02
#define GYRO_MAX_BITMASK_MAX_PLUS_Y   0x04
#define GYRO_MAX_BITMASK_MAX_MINUS_Y  0x08
#define GYRO_MAX_BITMASK_MAX_PLUS_Z   0x10
#define GYRO_MAX_BITMASK_MAX_MINUS_Z  0x20

#define LOCATOR_FLAGS_BITMASK_NONE           0x00
#define LOCATOR_FLAGS_BITMASK_AUTO_CALIBRATE 0x01

#define INFRARED_SENSOR_LOCATION_BITMASK_NONE         0x00000000
#define INFRARED_SENSOR_LOCATION_BITMASK_FRONT_LEFT   0x000000ff
#define INFRARED_SENSOR_LOCATION_BITMASK_FRONT_RIGHT  0x0000ff00
#define INFRARED_SENSOR_LOCATION_BITMASK_BACK_RIGHT   0x00ff0000
#define INFRARED_SENSOR_LOCATION_BITMASK_BACK_LEFT    0xff000000

#define COLOR_NOT_IDENTIFIED  0xFF

// Sensors are coded as 0x<processor><token><size><sensor>
#define QUATERNION        0x2120
#define QUATERNION_DATA_SIZE       0x02
#define QUATERNION_DATA_MIN        -1.0
#define QUATERNION_DATA_MAX        1.0
#define IMU               0x2121
#define IMU_DATA_SIZE              0x02
#define IMU_PITCH_MIN              -180.0
#define IMU_PITCH_MAX              180.0
#define IMU_ROLL_MIN               -90.0
#define IMU_ROLL_MAX               90.0
#define IMU_YAW_MIN                -180.0
#define IMU_YAW_MAX                180.0
#define ACCELEROMETER     0x2122
#define ACCELEROMETER_DATA_SIZE    0x02
#define ACCELEROMETER_DATA_MIN     -16.0
#define ACCELEROMETER_DATA_MAX     16.0
#define COLOR_DETECTION   0x1103
#define COLOR_DETECTION_DATA_SIZE  0x00
#define COLOR_DETECTION_RGB_MIN    0
#define COLOR_DETECTION_RGB_MAX    255
#define COLOR_DETECTION_INDEX_MIN  0
#define COLOR_DETECTION_INDEX_MAX  255
#define COLOR_DETECTION_CONFIDENCE_MIN  0.0
#define COLOR_DETECTION_CONFIDENCE_MAX  1.0
#define GYROSCOPE         0x2124
#define GYROSCOPE_DATA_SIZE        0x02
#define GYROSCOPE_DATA_MIN         -2000.0
#define GYROSCOPE_DATA_MAX         2000.0
#define LOCATOR           0x2226
#define LOCATOR_DATA_SIZE          0x02
#define LOCATOR_DATA_MIN           -16000.0
#define LOCATOR_DATA_MAX           16000.0
#define VELOCITY          0x2227
#define VELOCITY_DATA_SIZE         0x02
#define VELOCITY_DATA_MIN          -5.0
#define VELOCITY_DATA_MAX          5.0
#define SPEED             0x2228
#define SPEED_DATA_SIZE            0x02
#define SPEED_DATA_MIN             0.0
#define SPEED_DATA_MAX             5.0
#define CORE_TIME         0x3329
#define CORE_TIME_DATA_SIZE        0x02
#define CORE_TIME_DATA_UPPER_MIN   0
#define CORE_TIME_DATA_UPPER_MAX   UINT_32_MAX
#define CORE_TIME_DATA_LOWER_MIN   0
#define CORE_TIME_DATA_LOWER_MAX   UINT_32_MAX
#define AMBIENT_LIGHT     0x122A
#define AMBIENT_LIGHT_DATA_SIZE    0x02
#define AMBIENT_LIGHT_DATA_MIN     0.0
#define AMBIENT_LIGHT_DATA_MAX     120000.0

void test_normalize();

float get_ambient_light_sensor_value();

float * get_motor_temperature(u_int8_t motor_index);

void reset_sensor_list();
void add_sensor(u_int16_t sensor);
void configure_streaming_service();
void start_streaming_service(u_int16_t period);
void set_sensor_data(u_int8_t *data);
void stop_streaming_service();
void clear_streaming_service();

float get_quaternion_W();
float get_quaternion_X();
float get_quaternion_Y();
float get_quaternion_Z();
float get_pitch();
float get_roll();
float get_yaw();
float get_accelerometer_X();
float get_accelerometer_Y();
float get_accelerometer_Z();
float get_G();
float get_R();
float get_B();
float get_color_index();
float get_confidence();
float get_gyroscope_X();
float get_gyroscope_Y();
float get_gyroscope_Z();
float get_locator_X();
float get_locator_Y();
float get_velocity_X();
float get_velocity_Y();
float get_speed();
float get_light();

u_int8_t get_gyro_flags();
void set_gyro_flags(u_int8_t flags);
int get_color_detected();
void set_color_data(u_int8_t *data);

void enable_gyro_max_notify(u_int8_t is_enabled);
void reset_locator_x_and_y();
void set_locator_flags(u_int8_t flags);
u_int32_t get_bot_to_bot_infrared_readings(); 
u_int16_t * get_rgbc_sensor_values();
void start_robot_to_robot_infrared_broadcasting(u_int8_t far_code, u_int8_t near_code);
void stop_robot_to_robot_infrared_broadcasting();
void start_robot_to_robot_infrared_following(u_int8_t far_code, u_int8_t near_code);
void stop_robot_to_robot_infrared_following();
void start_robot_to_robot_infrared_evading(u_int8_t far_code, u_int8_t near_code);
void stop_robot_to_robot_infrared_evading();
float * get_motor_temperature(u_int8_t motor_indexes);
void enable_color_detection_notify(u_int8_t is_enabled, u_int16_t interval, u_int8_t minimum_confidence_threshold);
void get_current_detected_color_reading();
void enable_color_detection(u_int8_t is_enabled);
void enable_robot_infrared_message_notify(u_int8_t is_enabled);
void send_infrared_message(u_int8_t infrared_code, u_int8_t front_strength, u_int8_t left_strength, u_int8_t right_strength, u_int8_t rear_strength);
void get_motor_thermal_protection_status();
float get_right_motor_temperature();
float get_right_motor_status();
float get_left_motor_temperature();
float get_left_motor_status();
void enable_motor_thermal_protection_status_notify(u_int8_t is_enabled);

 