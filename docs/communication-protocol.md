# Industrial Modbus RTU Communication Protocol

The controller integrates standard **Modbus RTU Slave over RS-485** (Half-Duplex, 8-N-1, Default 19200 Baud, Node ID 1).

---

## 1. Modbus Register Address Map

### Discrete Inputs (Read-Only Binary - Function Code `0x02`)

| Address | Protocol Address | Description | Values |
| :---: | :---: | :--- | :--- |
| `10001` | `0x0000` | Emergency Stop Status | `1` = Normal / Closed, `0` = Tripped / Active |
| `10002` | `0x0001` | Safety Door Interlock | `1` = Closed & Locked, `0` = Open |
| `10003` | `0x0002` | Scrubber Airflow Switch | `1` = Differential Flow OK, `0` = Airflow Lost |
| `10004` | `0x0003` | Maintenance Service Keyswitch | `1` = Service Mode, `0` = Normal Mode |

---

### Coils (Read/Write Binary - Function Codes `0x01`, `0x05`)

| Address | Protocol Address | Description | Read/Write | Function / Values |
| :---: | :---: | :--- | :---: | :--- |
| `00001` | `0x0000` | Disinfection Relay | Read-Only | `1` = Active Dispersal ON, `0` = OFF |
| `00002` | `0x0001` | Exhaust Scrubber Fan | Read-Only | `1` = Blower Fan ON, `0` = OFF |
| `00003` | `0x0002` | Electromagnetic Door Lock | Read-Only | `1` = Locked, `0` = Unlocked |
| `00004` | `0x0003` | Remote Cycle Start Command | Write-Only | Write `0xFF00` to trigger Cycle Start |
| `00005` | `0x0004` | Remote Cycle Stop Command | Write-Only | Write `0xFF00` to trigger Cycle Cancel |
| `00006` | `0x0005` | Fault Acknowledge & Reset | Write-Only | Write `0xFF00` to Reset Latched Faults |

---

### Input Registers (Read-Only Telemetry - Function Code `0x04`)

| Address | Protocol Address | Parameter Name | Engineering Units | Scale / Format |
| :---: | :---: | :--- | :--- | :--- |
| `30001` | `0x0000` | Current System State | State ID (0 - 11) | `0`=Init, `1`=Idle, `2`=Ready, `4`=Disinfect... |
| `30002` | `0x0001` | Chamber Temperature | $0.1\,^\circ\text{C}$ | `254` = $25.4\,^\circ\text{C}$ |
| `30003` | `0x0002` | Chamber Relative Humidity | $\% \text{RH}$ | `0` - `100` $\% \text{RH}$ |
| `30004` | `0x0003` | Process Gas Concentration | $\text{PPM}$ | `0` - `200` $\text{PPM}$ |
| `30005` | `0x0004` | External Room Leak Sensor | $\text{PPM}$ | `0` - `50` $\text{PPM}$ |
| `30006` | `0x0005` | Phase Elapsed Time | Seconds | Integer seconds |
| `30007` | `0x0006` | Phase Remaining Time | Seconds | Integer seconds |
| `30008` | `0x0007` | Active Fault Bitmask | Bitmask Flags | Bit 0: E-Stop, Bit 1: Door, Bit 2: Temp... |
| `30009` | `0x0008` | System Uptime (High Word) | Seconds | Upper 16 bits of 32-bit uptime |
| `30010` | `0x0009` | System Uptime (Low Word) | Seconds | Lower 16 bits of 32-bit uptime |

---

### Holding Registers (Read/Write Configuration - Function Codes `0x03`, `0x06`, `0x10`)

| Address | Protocol Address | Parameter Name | Default Value | Safety Range |
| :---: | :---: | :--- | :---: | :--- |
| `40001` | `0x0000` | Pre-Purge Duration | $30\,\text{s}$ | $5 - 600\,\text{s}$ |
| `40002` | `0x0001` | Disinfection Duration | $300\,\text{s}$ | $10 - 7200\,\text{s}$ |
| `40003` | `0x0002` | Dwell Soak Duration | $120\,\text{s}$ | $0 - 3600\,\text{s}$ |
| `40004` | `0x0003` | Post-Purge Duration | $60\,\text{s}$ | $10 - 1800\,\text{s}$ |
| `40005` | `0x0004` | Target Concentration | $50\,\text{PPM}$ | $10 - 150\,\text{PPM}$ |
| `40006` | `0x0005` | Maximum Chamber Temperature | $45\,^\circ\text{C}$ | $20 - 85\,^\circ\text{C}$ |
| `40007` | `0x0006` | External Leak Trip Threshold | $5\,\text{PPM}$ | $1 - 50\,\text{PPM}$ |
| `40008` | `0x0007` | Minimum Humidity Threshold | $40\,\%\text{RH}$ | $10 - 90\,\%\text{RH}$ |
| `40009` | `0x0008` | Door Interlock Enforcement | `1` (Enabled) | `1` = Enforced, `0` = Bypassed |

---

## 2. Timing & Silence Detection

* **Inter-Character Timeout ($t_{1.5}$):** $750\,\mu\text{s}$ at 19200 baud.
* **Frame Delimiter Timeout ($t_{3.5}$):** $1.75\,\text{ms}$ at 19200 baud ($5.0\,\text{ms}$ software margin implemented via non-blocking hardware timer).
* **RS-485 Transceiver Control:** `HAL_GPIO_OUT_RS485_DE` is pulled HIGH prior to transmission and returned LOW only after the UART Transmission Complete (`TC`) hardware flag is asserted.
