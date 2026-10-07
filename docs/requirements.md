# System Requirements Specification (SRS)

## 1. Functional Requirements (FR)

| Req ID | Category | Description | Priority |
| :--- | :--- | :--- | :---: |
| **FR-01** | **State Sequencing** | The controller shall execute a deterministic 4-phase decontamination cycle: Pre-Purge $\rightarrow$ Disinfection $\rightarrow$ Dwell Soak $\rightarrow$ Post-Purge. | HIGH |
| **FR-02** | **Interlock Enforcement** | The controller shall energize the electromagnetic door lock throughout all active phases and refuse cycle start if the safety door is open. | CRITICAL |
| **FR-03** | **Emergency Stop** | The controller shall monitor a normally closed (NC) hardware E-Stop contact and de-energize all generation relays within 1ms upon contact break. | CRITICAL |
| **FR-04** | **Sensor Signal Processing** | The controller shall sample multi-channel analog inputs (temperature, humidity, process gas, ambient leak) at $\ge$100 Hz, applying Median-3 spike rejection and Exponential Moving Average (EMA) filtering. | HIGH |
| **FR-05** | **Safety Leak Trip** | If external ambient room leak exceeds the configurable threshold (default: 5 PPM), the controller shall immediately trip to `STATE_EMERGENCY_STOP` and activate exhaust purge. | CRITICAL |
| **FR-06** | **Parameter Management** | The system shall persist cycle durations and alarm setpoints in NVM protected by a 16-bit CRC-CCITT checksum. Corrupted blocks shall trigger automatic fallback to factory defaults. | HIGH |
| **FR-07** | **Modbus RTU Slave** | The firmware shall implement Modbus RTU over RS-485 (FC01, FC02, FC03, FC04, FC05, FC06, FC16) for SCADA telemetry and parameter editing. | HIGH |
| **FR-08** | **Event Logging** | The system shall record the last 64 operational events and faults in a circular RAM log with millisecond timestamps. | MEDIUM |

---

## 2. Non-Functional & Safety Requirements (NFR)

| Req ID | Category | Metric / Constraint |
| :--- | :--- | :--- |
| **NFR-01** | **Non-Blocking Architecture** | Zero blocking delays allowed in super-loop execution (`while(1)` polling forbidden during operational states). |
| **NFR-02** | **Deterministic Timing** | Task scheduler jitter shall remain $<100\,\mu\text{s}$ for 1ms high-speed slots. |
| **NFR-03** | **Hardware Watchdog** | Watchdog service window set to $2.0\,\text{s}$. Watchdog must be fed strictly at the end of healthy scheduler sweeps. |
| **NFR-04** | **MISRA-C Alignment** | No dynamic memory allocation (`malloc`/`free` prohibited), fixed-width integer types (`stdint.h`), strict `const` correctness. |
| **NFR-05** | **Portability** | Complete separation of hardware drivers via HAL, enabling seamless native compilation on x86_64 host simulators and 32-bit ARM Cortex-M MCUs. |
