#include "sensor.h"

int is_valid;
float quaternion_w, quaternion_x, quaternion_y, quaternion_z;
float pitch, roll, yaw;
float accelerometer_x, accelerometer_y, accelerometer_z;
float r, g, b, cindex, confidence;
float gyroscope_x, gyroscope_y, gyroscope_z;
float locator_x, locator_y;
float velocity_x, velocity_y;
float speed;
float light;
float time_upper, time_lower;

float left_motor_temperature, right_motor_temperature;
u_int8_t left_motor_status, right_motor_status;

u_int16_t sensor_list[15];
int sensor_count;
pthread_t sensor_thread;

u_int8_t gyro_flags;
u_int8_t *color_data;

float get_ambient_light_sensor_value() {
     union {
        u_int32_t intvalue;
        float floatvalue;
    } cvfloat; 
 
    char * reallyafloat = 
        messageSendAndRecv(GET_AMBIENT_LIGHT_SENSOR_VALUE, SENSOR_DEVICE_ID,
                              NO_SOURCE, TARGET_ID_NORDIC, 
                              NULL, 0, 4);

    u_int32_t integer = reallyafloat[0] << 24 | reallyafloat[1] << 16 | reallyafloat[2] << 8 | reallyafloat[3];
    cvfloat.intvalue = integer;

    return cvfloat.floatvalue;
}

void reset_sensor_list() {
    sensor_count = 0;
}

void add_sensor(u_int16_t sensor) { 
    sensor_list[sensor_count++] = sensor;
}

void configure_streaming_service() {
    if (sensor_count == 0) return;

    u_int16_t sensor;
    u_int8_t * place;
    u_int8_t * sensor_data = (u_int8_t *) malloc(sizeof(u_int8_t)*3*sensor_count + 1);

    int count;
    for (int p=1; p<=2; p++) {   // processor
        for (int t=1; t<=2; t++) {    // token
            count = 0;
            place = sensor_data;
            *place = t;
            place++;
            for (int s=0; s<sensor_count; s++) {   // sensor
                sensor = sensor_list[s];
                if ((sensor & 0xF000) >> 12 != p) continue;
                if ((sensor & 0x0F00) >> 8 != t) continue;
                // processor is right; token is right!
                if (logging_level >= VERBOSE) printf("*** Got sensor %04x, processor = %01x, token = %01x\n", 
                                                    sensor, (sensor & 0xF000) >> 12, (sensor & 0x0F00) >> 8);
                place[0] = 0x00;
                place[1] = sensor & 0x000F;
                place[2] = (sensor & 0x00F0) >> 4;
                place += 3;
                count++;
            }
            if (count > 0) {
                messageSend(CONFIGURE_STREAMING_SERVICE, SENSOR_DEVICE_ID,
                            NO_SOURCE, p,
                            sensor_data, count*3+1);
            }
        }
    }
}

int keep_reading;
#define FALSE 0
#define TRUE  1

double normalize(unsigned int value, long in_min, unsigned long in_max, long out_min, unsigned long out_max) {
    if (logging_level == VERYVERBOSE) 
        printf("Value = %d, in_min = %d, in_max = %d, out_min = %d, out_max = %d\n", value, in_min, in_max, out_min, out_max);
    double r1 = value - in_min;
    double r2 = in_max - in_min;
    //printf("Marker #1 = %f,%f\n", r1, r2);
    r1 = r1 / r2;
    r2 = out_max - out_min;
    r1 = r1 * r2 + out_min;
    double r = ((((float)(value - in_min))/((float)(in_max - in_min))) * (out_max - out_min)) + out_min;
    if (logging_level == VERYVERBOSE) printf("...r = %f\n", r);
    return r1;
}

void set_sensor_data(u_int8_t *data) {
    union {
        u_int32_t intvalue;
        float floatvalue;
    } cvfloat; 
    //u_int8_t * data;
    u_int32_t integer;
    float normalized;
    
    //keep_reading = TRUE;
    //printf("Sensor thread starting\n");
    //while (keep_reading) {
    //    data = messageRecv();

        for (int i=0; i<sensor_count; i++) {
            if (sensor_list[i] == QUATERNION) {
                is_valid = (int)data[0];
                integer = data[1] << 24 | data[2] << 16 | data[3] << 8 | data[4];
                quaternion_w = normalize(integer, 0, UINT_32_MAX, QUATERNION_DATA_MIN, QUATERNION_DATA_MAX);
                integer = data[5] << 24 | data[6] << 16 | data[7] << 8 | data[8];
                quaternion_x = normalize(integer, 0, UINT_32_MAX, QUATERNION_DATA_MIN, QUATERNION_DATA_MAX);
                integer = data[9] << 24 | data[10] << 16 | data[11] << 8 | data[12];
                quaternion_y = normalize(integer, 0, UINT_32_MAX, QUATERNION_DATA_MIN, QUATERNION_DATA_MAX);
                integer = data[13] << 24 | data[14] << 16 | data[15] << 8 | data[16];
                quaternion_z = normalize(integer, 0, UINT_32_MAX, QUATERNION_DATA_MIN, QUATERNION_DATA_MAX);
            } else if (sensor_list[i] == IMU) {
                is_valid = (int)data[0];
                integer = data[1] << 24 | data[2] << 16 | data[3] << 8 | data[4];
                pitch = normalize(integer, 0, UINT_32_MAX, IMU_PITCH_MIN, IMU_PITCH_MAX);
                integer = data[5] << 24 | data[6] << 16 | data[7] << 8 | data[8];
                roll = normalize(integer, 0, UINT_32_MAX, IMU_ROLL_MIN, IMU_ROLL_MAX);
                integer = data[9] << 24 | data[10] << 16 | data[11] << 8 | data[12];
                yaw = normalize(integer, 0, UINT_32_MAX, IMU_YAW_MIN, IMU_YAW_MAX);
            } else if (sensor_list[i] == ACCELEROMETER) {
                is_valid = (int)data[0];
                integer = data[1] << 24 | data[2] << 16 | data[3] << 8 | data[4];
                accelerometer_x = normalize(integer, 0, UINT_32_MAX, ACCELEROMETER_DATA_MIN, ACCELEROMETER_DATA_MAX);
                integer = data[5] << 24 | data[6] << 16 | data[7] << 8 | data[8];
                accelerometer_y = normalize(integer, 0, UINT_32_MAX, ACCELEROMETER_DATA_MIN, ACCELEROMETER_DATA_MAX);
                integer = data[9] << 24 | data[10] << 16 | data[11] << 8 | data[12];
                accelerometer_z = normalize(integer, 0, UINT_32_MAX, ACCELEROMETER_DATA_MIN, ACCELEROMETER_DATA_MAX);
            } else if (sensor_list[i] == COLOR_DETECTION) {
                is_valid = (int)data[0];
                integer = data[1];
                r = normalize(integer, 0, UINT_8_MAX, COLOR_DETECTION_RGB_MIN, COLOR_DETECTION_RGB_MAX);
                integer = data[2];
                g = normalize(integer, 0, UINT_8_MAX, COLOR_DETECTION_RGB_MIN, COLOR_DETECTION_RGB_MAX);
                integer = data[3];
                b = normalize(integer, 0, UINT_8_MAX, COLOR_DETECTION_RGB_MIN, COLOR_DETECTION_RGB_MAX);
                integer = data[4];
                cindex = normalize(integer, 0, UINT_8_MAX, COLOR_DETECTION_INDEX_MIN, COLOR_DETECTION_INDEX_MAX);
                integer = data[5];
                confidence = normalize(integer, 0, UINT_8_MAX, COLOR_DETECTION_CONFIDENCE_MIN, COLOR_DETECTION_CONFIDENCE_MAX);
            } else if (sensor_list[i] == GYROSCOPE) {
                is_valid = (int)data[0];
                integer = data[1] << 24 | data[2] << 16 | data[3] << 8 | data[4];
                gyroscope_x = normalize(integer, 0, UINT_32_MAX, GYROSCOPE_DATA_MIN, GYROSCOPE_DATA_MAX);
                integer = data[5] << 24 | data[6] << 16 | data[7] << 8 | data[8];
                gyroscope_y = normalize(integer, 0, UINT_32_MAX, GYROSCOPE_DATA_MIN, GYROSCOPE_DATA_MAX);
                integer = data[9] << 24 | data[10] << 16 | data[11] << 8 | data[12];
                gyroscope_z = normalize(integer, 0, UINT_32_MAX, GYROSCOPE_DATA_MIN, GYROSCOPE_DATA_MAX);
            } else if (sensor_list[i] == LOCATOR) {
                is_valid = (int)data[0];
                integer = data[1] << 24 | data[2] << 16 | data[3] << 8 | data[4];
                locator_x = normalize(integer, 0, UINT_32_MAX, LOCATOR_DATA_MIN, LOCATOR_DATA_MAX);
                integer = data[5] << 24 | data[6] << 16 | data[7] << 8 | data[8];
                locator_y = normalize(integer, 0, UINT_32_MAX, LOCATOR_DATA_MIN, LOCATOR_DATA_MAX);
           } else if (sensor_list[i] == VELOCITY) {
                is_valid = (int)data[0];
                integer = data[1] << 24 | data[2] << 16 | data[3] << 8 | data[4];
                velocity_x = normalize(integer, 0, UINT_32_MAX, VELOCITY_DATA_MIN, VELOCITY_DATA_MAX);
                integer = data[5] << 24 | data[6] << 16 | data[7] << 8 | data[8];
                velocity_y = normalize(integer, 0, UINT_32_MAX, VELOCITY_DATA_MIN, VELOCITY_DATA_MAX);
           } else if (sensor_list[i] == SPEED) {
                is_valid = (int)data[0];
                integer = data[1] << 24 | data[2] << 16 | data[3] << 8 | data[4];
                speed = normalize(integer, 0, UINT_32_MAX, SPEED_DATA_MIN, SPEED_DATA_MAX);
           } else if (sensor_list[i] == CORE_TIME) {
                // integer = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
                // time_upper = normalize(integer, 0, UINT_32_MAX, CORE_TIME_DATA_UPPER_MIN, CORE_TIME_DATA_UPPER_MAX);
                // integer = data[4] << 24 | data[5] << 16 | data[6] << 8 | data[7];
                // time_lower = normalize(integer, 0, UINT_32_MAX, CORE_TIME_DATA_LOWER_MIN, CORE_TIME_DATA_LOWER_MAX);
            } else if (sensor_list[i] == AMBIENT_LIGHT) {
                is_valid = (int)data[0];
                integer = data[1] << 24 | data[2] << 16 | data[3] << 8 | data[4];
                light = normalize(integer, 0, UINT_32_MAX, AMBIENT_LIGHT_DATA_MIN, AMBIENT_LIGHT_DATA_MAX);
            }
        }
    //}
    //printf("Sensor thread stopping\n");

}

void test_normalize ( ) {
    set_logging_level(VERYVERBOSE);
    float d = normalize(2142613248, 0, UINT_32_MAX, ACCELEROMETER_DATA_MIN, ACCELEROMETER_DATA_MAX);
    printf("Test 1; should be -0.036288, value = %f\n", d);
}

// Getter functions
float get_quaternion_W() {return quaternion_w;}
float get_quaternion_X() {return quaternion_x;}
float get_quaternion_Y() {return quaternion_y;}
float get_quaternion_Z() {return quaternion_z;}
float get_pitch() {return pitch;}
float get_roll() {return roll;}
float get_yaw() {return yaw;}
float get_accelerometer_X() {return accelerometer_x;}
float get_accelerometer_Y() {return accelerometer_y;}
float get_accelerometer_Z() {return accelerometer_z;}
float get_R() {return r;}
float get_G() {return g;}
float get_B() {return b;}
float get_color_index() {return cindex;}
float get_confidence() {return confidence;}
float get_gyroscope_X() {return gyroscope_x;}
float get_gyroscope_Y() {return gyroscope_y;}
float get_gyroscope_Z() {return gyroscope_z;}
float get_locator_X() {return locator_x;}
float get_locator_Y() {return locator_y;}
float get_velocity_X() {return velocity_x;}
float get_velocity_Y() {return velocity_y;}
float get_speed() {return speed;}
float get_light() {return light;}

void start_streaming_service(u_int16_t period) {
    u_int8_t * data = (u_int8_t *) malloc(sizeof(u_int8_t)*2);
    data[0] = (period & 0xFF00) >> 8;
    data[1] = (period & 0x00FF);
    messageSend(START_STREAMING_SERVICE, SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_NORDIC,
                data, 2);
    messageSend(START_STREAMING_SERVICE, SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_ST,
                data, 2);
    
    //pthread_create(&sensor_thread, NULL, read_sensors, NULL); 
    //printf("Thread created\n");
}

void stop_streaming_service() {
    keep_reading = FALSE;
    messageSend(STOP_STREAMING_SERVICE, SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_NORDIC,
                NULL, 0);
    messageSend(STOP_STREAMING_SERVICE, SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_ST,
                NULL, 0);

    //pthread_cancel(sensor_thread);
    reset_sensor_list();
}

void clear_streaming_service() {
    messageSend(CLEAR_STREAMING_SERVICE, SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_NORDIC,
                NULL, 0);
    messageSend(CLEAR_STREAMING_SERVICE, SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_ST,
                NULL, 0);

    reset_sensor_list();
}

//-----------------------------------------------------------------------------------------
//  Non-streaming sensor stuff

void enable_gyro_max_notify(u_int8_t is_enabled) { 
    u_int8_t * enabled = (u_int8_t *) malloc(sizeof(u_int8_t));
    enabled[0] = is_enabled;

    messageSend(ENABLE_GYRO_MAX_NOTIFY, SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_ST,
                enabled, 1); 

    gyro_flags = -1;   
}

void reset_locator_x_and_y() {
    messageSend(RESET_LOCATOR_X_AND_Y, SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_ST,
                NULL, 0);
}

void set_locator_flags(u_int8_t flags) {
    u_int8_t * flag_buffer = (u_int8_t *) malloc(sizeof(u_int8_t));
    flag_buffer[0] = flags;
    messageSend(SET_LOCATOR_FLAGS, SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_ST,
                flag_buffer, 1);
}

u_int32_t get_bot_to_bot_infrared_readings() { 
    u_int8_t * reading = 
        messageSendAndRecv(GET_BOT_TO_BOT_INFRARED_READINGS, SENSOR_DEVICE_ID,
                              NO_SOURCE, TARGET_ID_NORDIC, 
                              NULL, 0, 4);

    u_int32_t integer = reading[0] << 24 | reading[1] << 16 | reading[2] << 8 | reading[3];
    return integer;
}

u_int16_t * get_rgbc_sensor_values() { 
    u_int8_t * data = 
        messageSendAndRecv(GET_BOT_TO_BOT_INFRARED_READINGS, SENSOR_DEVICE_ID,
                              NO_SOURCE, TARGET_ID_NORDIC, 
                              NULL, 0, 8);

    u_int16_t * values = (u_int16_t *) malloc(sizeof(u_int16_t)*4);
    values[0] = data[0] << 8 | data[1];
    values[1] = data[2] << 8 | data[3];
    values[2] = data[4] << 8 | data[5];
    values[3] = data[6] << 8 | data[7];
    
    return values;
}

void start_robot_to_robot_infrared_broadcasting(u_int8_t far_code, u_int8_t near_code) {
    u_int8_t * code_buffer = (u_int8_t *) malloc(sizeof(u_int8_t)*2);
    code_buffer[0] = far_code;
    code_buffer[1] = near_code;
    messageSend(START_ROBOT_TO_ROBOT_INFRARED_BROADCASTING, SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_ST,
                code_buffer, 2);
}

void stop_robot_to_robot_infrared_broadcasting() {
    messageSend(STOP_ROBOT_TO_ROBOT_INFRARED_BROADCASTING,SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_ST,
                NULL, 0);
}

void start_robot_to_robot_infrared_following(u_int8_t far_code, u_int8_t near_code) {
    u_int8_t * code_buffer = (u_int8_t *) malloc(sizeof(u_int8_t)*2);
    code_buffer[0] = far_code;
    code_buffer[1] = near_code;
    messageSend(START_ROBOT_TO_ROBOT_INFRARED_FOLLOWING,SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_ST,
                code_buffer, 2);
}

void stop_robot_to_robot_infrared_following() {
    messageSend(STOP_ROBOT_TO_ROBOT_INFRARED_FOLLOWING,SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_ST,
                NULL, 0);
}

void start_robot_to_robot_infrared_evading(u_int8_t far_code, u_int8_t near_code) {
    u_int8_t * code_buffer = (u_int8_t *) malloc(sizeof(u_int8_t)*2);
    code_buffer[0] = far_code;
    code_buffer[1] = near_code;
    messageSend(START_ROBOT_TO_ROBOT_INFRARED_EVADING,SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_ST,
                code_buffer, 2);
}

void stop_robot_to_robot_infrared_evading() {
    messageSend(STOP_ROBOT_TO_ROBOT_INFRARED_EVADING,SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_ST,
                NULL, 0);
}

float * get_motor_temperature(u_int8_t motor_indexes) {
     union {
        u_int32_t intvalue;
        float floatvalue;
    } cvfloat; 
    u_int8_t * index_buffer = (u_int8_t *) malloc(sizeof(u_int8_t));
    index_buffer[0] = motor_indexes;
    u_int8_t * data = 
        messageSendAndRecv(GET_MOTOR_TEMPERATURE, SENSOR_DEVICE_ID,
                           NO_SOURCE, TARGET_ID_NORDIC,
                           index_buffer, 1, 8);
                           
    float * fdata = (float *) malloc(sizeof(float)*2);
    u_int32_t integer = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
    cvfloat.intvalue = integer;
    fdata[0] = cvfloat.floatvalue;
    integer = data[4] << 24 | data[5] << 16 | data[6] << 8 | data[7];
    cvfloat.intvalue = integer;
    fdata[1] = cvfloat.floatvalue;

    return fdata;
}

void enable_color_detection_notify(u_int8_t is_enabled, u_int16_t interval, u_int8_t minimum_confidence_threshold) {
    u_int8_t * buffer = (u_int8_t *) malloc(sizeof(u_int8_t)*4);
    buffer[0] = is_enabled;
    buffer[1] = interval >> 8 & 0xFF;
    buffer[2] = interval & 0xFF;
    buffer[3] = minimum_confidence_threshold;

    messageSend(ENABLE_COLOR_DETECTION_NOTIFY, SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_NORDIC,
                buffer, 4);

    color_data = NULL;
}

void get_current_detected_color_reading() {
    messageSend(GET_CURRENT_DETECTED_COLOR_READING, SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_NORDIC,
                NULL, 0);
}

void enable_color_detection(u_int8_t is_enabled) { 
    u_int8_t * enabled = (u_int8_t *) malloc(sizeof(u_int8_t));
    enabled[0] = is_enabled;

    messageSend(ENABLE_COLOR_DETECTION, SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_NORDIC,
                enabled, 1);    
}

void enable_robot_infrared_message_notify(u_int8_t is_enabled) { 
    u_int8_t * enabled = (u_int8_t *) malloc(sizeof(u_int8_t));
    enabled[0] = is_enabled;

    messageSend(ENABLE_ROBOT_INFRARED_MESSAGE_NOTIFY, SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_NORDIC,
                enabled, 1);    
}

void send_infrared_message(u_int8_t infrared_code, u_int8_t front_strength, u_int8_t left_strength, u_int8_t right_strength, u_int8_t rear_strength) {
    u_int8_t * buffer = (u_int8_t *) malloc(sizeof(u_int8_t)*5);
    buffer[0] = infrared_code;
    buffer[1] = front_strength;
    buffer[2] = left_strength;
    buffer[3] = right_strength;
    buffer[4] = rear_strength;

    messageSend(SEND_INFRARED_MESSAGE, SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_NORDIC,
                buffer, 5);
}

void get_motor_thermal_protection_status() {
    union {
        u_int32_t intvalue;
        float floatvalue;
    } cvfloat; 

    u_int8_t * data = 
        messageSendAndRecv(GET_MOTOR_THERMAL_PROTECTION_STATUS, SENSOR_DEVICE_ID,
                           NO_SOURCE, TARGET_ID_NORDIC,
                           NULL, 0, 10);
                           
    u_int32_t integer = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
    cvfloat.intvalue = integer;
    left_motor_temperature = cvfloat.floatvalue;
    left_motor_status = data[4];
    integer = data[5] << 24 | data[6] << 16 | data[7] << 8 | data[8];
    cvfloat.intvalue = integer;
    right_motor_temperature = cvfloat.floatvalue;
    right_motor_status = data[9];
}

float get_right_motor_temperature() { return right_motor_temperature; }
float get_right_motor_status() { return right_motor_status; }
float get_left_motor_temperature() { return left_motor_temperature; }
float get_left_motor_status() { return left_motor_status; }

void enable_motor_thermal_protection_status_notify(u_int8_t is_enabled) { 
    u_int8_t * enabled = (u_int8_t *) malloc(sizeof(u_int8_t));
    enabled[0] = is_enabled;

    messageSend(ENABLE_MOTOR_THERMAL_PROTECTION_STATUS_NOTIFY, SENSOR_DEVICE_ID,
                NO_SOURCE, TARGET_ID_NORDIC,
                enabled, 1);    
}

void set_gyro_flags(u_int8_t flags) {
    gyro_flags = flags;
}

u_int8_t get_gyro_flags() {
    return gyro_flags;
}

void set_color_data(u_int8_t *data) {
    u_int32_t integer;

    printf("*** Setting Color Data *** \n");
    printf("Data: %d, %d, %d\n", data[0], data[1], data[2]);

    r = (float)data[0];
    g = (float)data[1];
    b = (float)data[2];
    confidence = data[3];
    cindex = data[4];
}
 
int get_color_detected() {
    if (color_data == NULL) 
        return -1;
    else {
        return (color_data[0] << 16 & color_data[1] << 8 & color_data[2]);
    }
}
