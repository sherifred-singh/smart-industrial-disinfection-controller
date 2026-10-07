# STM32 Nucleo-F401RE / F411RE Target Pinout & Hardware Mapping

| Logical Signal | Target Pin | STM32 Peripheral | Circuit Type | Description | Safe Default State |
| :--- | :--- | :--- | :--- | :--- | :--- |
| `HAL_GPIO_IN_START_BTN` | `PC13` | GPIO Input | Active Low, Pull-Up | Pushbutton (B1 User Button) | High (Released) |
| `HAL_GPIO_IN_STOP_BTN` | `PB0` | GPIO Input | Active Low, Pull-Up | Cycle Stop / Cancel Pushbutton | High (Released) |
| `HAL_GPIO_IN_ESTOP_NC` | `PB1` | GPIO Input | Active High, Pull-Up | Normally Closed E-Stop Contact | High (Normal / Closed) |
| `HAL_GPIO_IN_DOOR_INTERLOCK` | `PB2` | GPIO Input | Active High, Pull-Up | Enclosure Magnetic Reed Switch | High (Door Closed) |
| `HAL_GPIO_IN_SERVICE_KEY` | `PB10` | GPIO Input | Active High, Pull-Down | Maintenance Mode Keyswitch | Low (Normal Mode) |
| `HAL_GPIO_IN_AIRFLOW_SW` | `PB11` | GPIO Input | Active High, Pull-Up | Scrubber Differential Pressure | High (Flow OK) |
| `HAL_GPIO_OUT_DISINFECT_RELAY` | `PA8` | GPIO Output PP | Opto-Isolated Relay | Disinfection Generator / Solenoid | Low (De-energized) |
| `HAL_GPIO_OUT_EXHAUST_FAN` | `PA9` | GPIO Output PP | Opto-Isolated Relay | Chamber Scrubber Fan Blower | Low (De-energized) |
| `HAL_GPIO_OUT_DOOR_LOCK` | `PA10` | GPIO Output PP | MOSFET Solenoid Driver | Electromagnetic Door Interlock | Low (Unlocked) |
| `HAL_GPIO_OUT_STATUS_LED_GRN` | `PA5` | GPIO Output PP | Direct LED (LD2) | Green Beacon (Idle / Ready) | Low (Off) |
| `HAL_GPIO_OUT_STATUS_LED_YEL` | `PA6` | GPIO Output PP | Transistor Driver | Yellow Beacon (Active / Warning) | Low (Off) |
| `HAL_GPIO_OUT_STATUS_LED_RED` | `PA7` | GPIO Output PP | Transistor Driver | Red Beacon (Critical Fault) | Low (Off) |
| `HAL_GPIO_OUT_BUZZER` | `PB6` | GPIO Output PP | Transistor Driver | Audible Alarm Buzzer | Low (Silent) |
| `HAL_GPIO_OUT_RS485_DE` | `PA1` | GPIO Output PP | RS-485 Transceiver | Driver Enable (DE/!RE) | Low (Receive Mode) |
| `HAL_ADC_CH_CHAMBER_TEMP` | `PA0` | `ADC1_IN0` | 0 - 3.3V Analog Input | Chamber Temperature Sensor | ~25.0 °C (~750 mV) |
| `HAL_ADC_CH_CHAMBER_HUMIDITY` | `PA4` | `ADC1_IN4` | 0 - 3.3V Analog Input | Chamber Humidity Sensor | ~50 %RH (~1650 mV) |
| `HAL_ADC_CH_PROCESS_CONC` | `PB0` (Alt) / `PC0` | `ADC1_IN10` | 0 - 3.3V Analog Input | Process Concentration Sensor | 0 PPM (0 mV) |
| `HAL_ADC_CH_AMBIENT_SAFETY_LEAK` | `PC1` | `ADC1_IN11` | 0 - 3.3V Analog Input | Ambient Safety Leak Detector | 0 PPM (0 mV) |
| `HAL_UART_PORT_MODBUS_TX` | `PA9` / `PB6` | `USART1_TX` | RS-485 Half-Duplex | Modbus RTU RS-485 Transceiver | Idle High |
| `HAL_UART_PORT_MODBUS_RX` | `PA10` / `PB7` | `USART1_RX` | RS-485 Half-Duplex | Modbus RTU RS-485 Transceiver | High-Z |
| `HAL_UART_PORT_DEBUG_TX` | `PA2` | `USART2_TX` | USB VCP Transceiver | ST-LINK Virtual COM Port | 115200 Baud |
| `HAL_UART_PORT_DEBUG_RX` | `PA3` | `USART2_RX` | USB VCP Transceiver | ST-LINK Virtual COM Port | 115200 Baud |
