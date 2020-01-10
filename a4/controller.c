
#include <stdio.h>
#include "controller.h"

int is_valid_header(struct cignal *cig, int *device_record);
int is_valid_device_id(struct cignal *cig);
int is_valid_device_type(struct cignal *cig);


/* Checks if the message has a valid type. The gateway should only receive
 * HANDSHAKE or UPDATE messages from sensors
 */
int is_valid_type(struct cignal *cig) {
    if ((cig->hdr.type == HANDSHAKE) || (cig->hdr.type == UPDATE)) {
        return 1;
    }
    return -1;
}

/* Checks if the message has a valid device_type. The gateway should only receive
 * TEMPERATURE or HUMIDITY messages from sensors
 */
int is_valid_device_type(struct cignal *cig){
    if ((cig->hdr.device_type == TEMPERATURE) || (cig->hdr.device_type == HUMIDITY)) {
        return 1;
    }
    return -1;
}

/* Checks if the message has a valid device_id. The gateway should only receive
 * int between 11 and 99 messages from sensors
 */
int is_valid_device_id(struct cignal *cig){
    if ((cig->hdr.device_id >= 11) && (cig->hdr.device_id <= 99)) {
        return 1;
    }
    return -1;
}

/* Checks the header. And return proper error message
Checking rubric:
if type is not HANDSHAKE or UPDATE, invalid
if type is HANDSHAKE, check value/range of device_id and device_type
if type is UPDATE, check registered status, and check value/range of device_id and device_type
*/
int is_valid_header(struct cignal *cig, int *device_record){
    if (is_valid_type(cig) == -1){
        fprintf(stderr, "Invalid type, should be HANDSHAKE:1 or UPDATE:2\n");
        return -1;
    } else {
        if (cig->hdr.type == HANDSHAKE){
            if (cig->hdr.device_id != -1){
                fprintf(stderr, "Invalid device_id, should be -1\n");
                return -1;
            } 
            if (is_valid_device_type(cig) == -1){
                fprintf(stderr, "Invalid device_type, should be TEMPERATURE:1 or HUMIDITY:2\n");
                return -1;
            }
            return 1;
        } else {
            if (is_registered(cig->hdr.device_id, device_record)){
                if (is_valid_device_id(cig) == -1){
                    fprintf(stderr, "Invalid device_id, should be in range [11,99]\n");
                    return -1;
                }
                if (is_valid_device_type(cig) == -1){
                    fprintf(stderr, "Invalid device_type, should be TEMPERATURE:1 or HUMIDITY:2\n");
                    return -1;
                }
                return 1;
            } else {
                fprintf(stderr, "The device has not been registered");
                return -1;
            }
        }
    }
}

/* Returns 1 if the gateway seen this device before?
 */
int is_registered(int id, int *device_record) {
    if (device_record[id - LOWEST_ID] == 1) {
        return 1;
    }
    return -1;
}

/* Add a new device to the device_record.  Return the new device id.
 * Note that device ids will never be "de-registered" so they cannot be reused.
 */
int register_device(int *device_record) {
    for (int i = 0; i < MAXDEV; i++) {
        if (device_record[i] == 0) {
            device_record[i] = 1;
            return i + LOWEST_ID;
        }
    }
    return -1;
}

/* Turns on or off the cooler or dehumidifier based on the
 * current temperature or humidity.j
 */
void adjust_fan(struct cignal *cig) {
    cig->hdr.type = 3;
    if (cig->hdr.device_type == TEMPERATURE) {
        if (cig->value >= TEMPSET) {
            cig->cooler = ON;
        } else {
            cig->cooler = OFF;
        }
    } else if (cig->hdr.device_type == HUMIDITY) {
        if (cig->value >= HUMSET) {
            cig->dehumid = ON;
        } else {
            cig->dehumid = OFF;
        }
    }
}


/* Check each field of the incoming header to ensure that it is valid, print
 * information process about the state of the sensor using the printf messages
 * below, and adjust the fan.
 * 
 * Error messages must be printed to stderr, but the contents of the messages
 * are unspecified so you can choose good messages.
 * 
 * Print the following before returning from handling a valid event.
 * printf("********************END EVENT********************\n\n");
 * 
 * Print the following after getting a value from the relevant sensor.
 * printf("Temperature: %.4f --> Device_ID: %d\n", YOUR VARIABLES HERE);
 * printf("Humidity: %.4f --> Device_ID: %d\n", YOUR VARIABLES HERE);
 */

int process_message(struct cignal *cig, int *device_record) {
    // check whether header is valid
    if (is_valid_header(cig, device_record) == 1){
        if (cig->hdr.type == HANDSHAKE){ // if HANDSHAKE, register this device
            cig->hdr.device_id = register_device(device_record);
        } else { //if UPDATE, print info and update TEMPERATURE/HUMIDITY
            if (cig->hdr.device_type == HUMIDITY){
                printf("Humidity: %.4f --> Device_ID: %d\n", cig->value, cig->hdr.device_id);
            } else {
                printf("Temperature: %.4f --> Device_ID: %d\n", cig->value, cig->hdr.device_id);
            }
            adjust_fan(cig);
        }
        printf("********************END EVENT********************\n\n");
        return 1;
    }
    fprintf(stderr, "Received corrupted cignal! The message is discarded...\n");
    return -1;
}
