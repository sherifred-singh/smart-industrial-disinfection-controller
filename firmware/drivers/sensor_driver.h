/**
 * @file sensor_driver.h
 * @brief Industrial Multi-Sensor Acquisition & Signal Conditioning Driver
 * @author Sherifred Singh
 * 
 * Acquires, scales, digitally filters (EMA/Median), and validates raw ADC voltages
 * and digital safety switch inputs (temperature, humidity, concentration, interlocks).
 */

#ifndef SENSOR_DRIVER_H
#define SENSOR_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Process Engineering Sensor Readings (Scaled & Filtered)
 */
typedef struct {
    int16_t  chamber_temp_c10;    /**< Chamber temperature in 0.1°C (e.g. 245 = 24.5°C) */
    uint16_t chamber_humidity_rh; /**< Chamber relative humidity (0 - 100% RH) */
    uint16_t chamber_conc_ppm;    /**< Disinfection agent concentration in PPM */
    uint16_t ambient_leak_ppm;    /**< External room ambient leak concentration in PPM */
    uint16_t supply_voltage_mv;   /**< 24V bus voltage in millivolts */

    /* Debounced Digital Safety Inputs */
    bool is_door_closed;          /**< true = Interlock engaged & closed */
    bool is_estop_normal;         /**< true = E-Stop healthy (not pressed) */
    bool is_airflow_ok;           /**< true = Exhaust scrubber differential flow OK */
    bool is_service_mode;         /**< true = Manual technician service key turned */
    bool is_start_btn_pressed;    /**< Debounced start button pulse */
    bool is_stop_btn_pressed;     /**< Debounced stop button pulse */

    /* Sensor Health Diagnostics */
    bool temp_sensor_valid;       /**< false if open-circuit or short-circuit detected */
    bool conc_sensor_valid;
} sensor_data_t;

/**
 * @brief Initialize sensor driver, filter coefficients, and debounce structures.
 */
void sensor_driver_init(void);

/**
 * @brief Periodic 10ms task to sample ADC channels, update filters, and debounce switches.
 * @param elapsed_ms Step interval (e.g. 10ms)
 */
void sensor_driver_process_10ms(uint32_t elapsed_ms);

/**
 * @brief Get the latest conditioned sensor snapshot.
 * @param out_data Pointer to destination struct
 */
void sensor_driver_get_readings(sensor_data_t *out_data);

/**
 * @brief Clear transient button press latch flags.
 */
void sensor_driver_clear_button_latches(void);

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_DRIVER_H */
