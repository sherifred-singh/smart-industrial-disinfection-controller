/**
 * @file main_sim.c
 * @brief Interactive CLI Simulation Console for Industrial Disinfection Controller
 * @author Sherifred Singh
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
static int _kbhit(void) {
    struct termios oldt, newt;
    int ch, oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}
static int _getch(void) { return getchar(); }
#endif

#include "process_controller.h"
#include "app_fsm.h"
#include "sensor_driver.h"
#include "actuator_driver.h"
#include "config_manager.h"
#include "alarm_fault_manager.h"
#include "modbus_rtu_slave.h"
#include "crc16.h"
#include "hal_gpio.h"
#include "hal_adc.h"
#include "hal_timer.h"

extern void hal_gpio_sim_set_input(hal_gpio_pin_t pin, hal_gpio_level_t level);
extern void hal_adc_sim_set_raw(hal_adc_channel_t ch, uint16_t raw_count);

static void print_dashboard(void)
{
    sensor_data_t sensors;
    sensor_driver_get_readings(&sensors);
    const config_data_t *cfg = config_manager_get();
    app_state_t state = app_fsm_get_state();

    printf("\033[H\033[J");
    printf("=========================================================================\n");
    printf("     SMART INDUSTRIAL DISINFECTION & ENVIRONMENT CONTROLLER SIMULATOR    \n");
    printf("     Author: Sherifred Singh | Embedded Firmware Architecture            \n");
    printf("=========================================================================\n");

    printf(" [SYSTEM STATE] : \033[1;36m%-16s\033[0m | Elapsed: %2us | Remaining: %2us\n",
           app_fsm_state_to_str(state),
           app_fsm_get_phase_elapsed_sec(),
           app_fsm_get_phase_remaining_sec());

    printf("-------------------------------------------------------------------------\n");
    printf(" [SENSORS]  Temp: %4.1f C (Max: %u C) | Humidity: %3u %%RH (Min: %u %%RH)\n",
           (float)sensors.chamber_temp_c10 / 10.0f, cfg->max_temperature_c,
           sensors.chamber_humidity_rh, cfg->min_humidity_rh);
    printf("            Chamber Conc: %3u PPM        | Ambient Leak: %2u PPM (Trip: %u PPM)\n",
           sensors.chamber_conc_ppm, sensors.ambient_leak_ppm, cfg->ambient_leak_limit_ppm);
    printf("            E-Stop: %s | Door: %s | Airflow: %s\n",
           sensors.is_estop_normal ? "\033[32mHEALTHY\033[0m" : "\033[31mTRIPPED\033[0m",
           sensors.is_door_closed ? "\033[32mCLOSED\033[0m" : "\033[33mOPEN\033[0m",
           sensors.is_airflow_ok ? "\033[32mOK\033[0m" : "\033[31mLOST\033[0m");

    printf("-------------------------------------------------------------------------\n");
    printf(" [ACTUATORS] Disinfection Relay : %s\n",
           actuator_driver_is_disinfect_on() ? "\033[1;32m[ ON  - ACTIVE DISPERSAL ]\033[0m" : "\033[37m[ OFF - DE-ENERGIZED     ]\033[0m");
    printf("             Exhaust Scrubber Fan : %s\n",
           actuator_driver_is_exhaust_on() ? "\033[1;32m[ ON  - VENTILATING      ]\033[0m" : "\033[37m[ OFF - IDLE             ]\033[0m");
    printf("             Safety Door Lock     : %s\n",
           actuator_driver_is_door_locked() ? "\033[1;33m[ LOCKED                 ]\033[0m" : "\033[37m[ UNLOCKED               ]\033[0m");

    printf(" [BEACONS]   Green: %s | Yellow: %s | Red: %s | Buzzer: %s\n",
           (hal_gpio_read(HAL_GPIO_OUT_STATUS_LED_GRN) == HAL_GPIO_LEVEL_HIGH) ? "\033[32mON\033[0m" : "OFF",
           (hal_gpio_read(HAL_GPIO_OUT_STATUS_LED_YEL) == HAL_GPIO_LEVEL_HIGH) ? "\033[33mON\033[0m" : "OFF",
           (hal_gpio_read(HAL_GPIO_OUT_STATUS_LED_RED) == HAL_GPIO_LEVEL_HIGH) ? "\033[31mON\033[0m" : "OFF",
           (hal_gpio_read(HAL_GPIO_OUT_BUZZER) == HAL_GPIO_LEVEL_HIGH) ? "\033[31mBEEPING\033[0m" : "SILENT");

    printf("-------------------------------------------------------------------------\n");
    printf(" [ALARM MASK] : 0x%04X (Critical: %s, Warning: %s)\n",
           alarm_fault_get_active_mask(),
           alarm_fault_has_critical() ? "\033[31mYES\033[0m" : "NO",
           alarm_fault_has_warning() ? "\033[33mYES\033[0m" : "NO");

    printf("=========================================================================\n");
    printf(" [KEYBOARD STIMULUS CONTROLS]\n");
    printf("  [1] Press START Button       [2] Press STOP Button\n");
    printf("  [3] Toggle EMERGENCY STOP    [4] Toggle DOOR Open/Close\n");
    printf("  [5] Inject High Temp (55 C)  [6] Inject Gas Leak (20 PPM)\n");
    printf("  [7] Reset Faults / Alarms    [8] Modbus RS-485 Frame Poll\n");
    printf("  [q] Quit Simulator\n");
    printf("=========================================================================\n");
}

int main(void)
{
    printf("Initializing Simulator Engine...\n");
    process_controller_init();

    for (int i = 0; i < 60; ++i) {
        hal_timer_delay_ms(10);
        process_controller_step_1ms();
        process_controller_step_10ms();
    }

    uint32_t last_ui_refresh_ms = 0;
    bool running = true;
    bool estop_state = true;
    bool door_state  = true;
    bool high_temp   = false;
    bool gas_leak    = false;

    while (running) {
        process_controller_step_1ms();
        process_controller_step_10ms();
        process_controller_step_100ms();
        process_controller_step_1000ms();

        if (_kbhit()) {
            int ch = _getch();
            switch (ch) {
                case '1':
                    hal_gpio_sim_set_input(HAL_GPIO_IN_START_BTN, HAL_GPIO_LEVEL_LOW);
                    for (int i = 0; i < 4; ++i) {
                        sensor_driver_process_10ms(10);
                    }
                    hal_gpio_sim_set_input(HAL_GPIO_IN_START_BTN, HAL_GPIO_LEVEL_HIGH);
                    break;

                case '2':
                    hal_gpio_sim_set_input(HAL_GPIO_IN_STOP_BTN, HAL_GPIO_LEVEL_LOW);
                    for (int i = 0; i < 4; ++i) {
                        sensor_driver_process_10ms(10);
                    }
                    hal_gpio_sim_set_input(HAL_GPIO_IN_STOP_BTN, HAL_GPIO_LEVEL_HIGH);
                    break;

                case '3':
                    estop_state = !estop_state;
                    hal_gpio_sim_set_input(HAL_GPIO_IN_ESTOP_NC, estop_state ? HAL_GPIO_LEVEL_HIGH : HAL_GPIO_LEVEL_LOW);
                    break;

                case '4':
                    door_state = !door_state;
                    hal_gpio_sim_set_input(HAL_GPIO_IN_DOOR_INTERLOCK, door_state ? HAL_GPIO_LEVEL_HIGH : HAL_GPIO_LEVEL_LOW);
                    break;

                case '5':
                    high_temp = !high_temp;
                    hal_adc_sim_set_raw(HAL_ADC_CH_CHAMBER_TEMP, high_temp ? 1400U : 931U);
                    break;

                case '6':
                    gas_leak = !gas_leak;
                    hal_adc_sim_set_raw(HAL_ADC_CH_AMBIENT_SAFETY_LEAK, gas_leak ? 1320U : 0U);
                    break;

                case '7':
                    app_fsm_dispatch(EVENT_FAULT_RESET_CMD);
                    break;

                case '8': {
                    uint8_t req[8] = {0x01, 0x04, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00};
                    uint16_t crc = crc16_modbus(req, 6);
                    req[6] = (uint8_t)(crc & 0xFFU);
                    req[7] = (uint8_t)((crc >> 8) & 0xFFU);

                    uint8_t resp[32];
                    size_t len = modbus_rtu_process_frame(req, sizeof(req), resp, sizeof(resp));
                    printf("\n[MODBUS MASTER POLL SUCCESS] Received %u bytes response over simulated RS-485.\n", (unsigned int)len);
                    hal_timer_delay_ms(800);
                    break;
                }

                case 'q':
                case 'Q':
                    running = false;
                    break;

                default:
                    break;
            }
        }

        if (hal_timer_has_elapsed(last_ui_refresh_ms, 200U)) {
            last_ui_refresh_ms = hal_timer_get_ms();
            print_dashboard();
        }

        hal_timer_delay_ms(10);
    }

    printf("\nSimulator stopped.\n");
    return 0;
}
