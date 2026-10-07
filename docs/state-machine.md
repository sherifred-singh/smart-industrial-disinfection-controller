# Finite State Machine (FSM) Specification

## 1. State Definitions

| State ID | Name | Description | Actuator Outputs | Exit Condition / Trigger |
| :--- | :--- | :--- | :--- | :--- |
| `0` | **`STATE_INIT`** | Hardware bring-up, peripheral init, NVM self-test | All Relays OFF | 500ms startup self-test timeout |
| `1` | **`STATE_IDLE`** | Standby mode, Green beacon ON, door unlocked | Green LED ON, All Relays OFF | Door Closed $\rightarrow$ `READY`; Service Key $\rightarrow$ `SERVICE` |
| `2` | **`STATE_READY`** | System interlocks closed, ready for cycle command | Green LED ON, Relays OFF | `START_CMD` $\rightarrow$ `PRE_PURGE`; Door Open $\rightarrow$ `IDLE` |
| `3` | **`STATE_PRE_PURGE`** | Scrubber fan running, differential airflow verified | Scrubber ON, Door Locked, Yellow LED | Pre-Purge duration elapsed (30s) $\rightarrow$ `DISINFECTING` |
| `4` | **`STATE_DISINFECTING`** | Active chemical dispersal / UV-C generation | Disinfect Relay ON, Door Locked, Yellow LED | Disinfect duration elapsed (300s) $\rightarrow$ `DWELL_SOAK` |
| `5` | **`STATE_DWELL_SOAK`** | Sealed contact dwell/soak time | All Relays OFF, Door Locked, Yellow LED | Dwell duration elapsed (120s) $\rightarrow$ `POST_PURGE` |
| `6` | **`STATE_POST_PURGE`** | Active chamber ventilation & chemical scrubbing | Scrubber ON, Door Locked, Yellow LED | Post-Purge duration elapsed (60s) $\rightarrow$ `COMPLETE` |
| `7` | **`STATE_COMPLETE`** | Decontamination successful, audible completion chime | Door Unlocked, Green LED, 5s Chime | 5s chime elapsed or button press $\rightarrow$ `READY` / `IDLE` |
| `8` | **`STATE_WARNING`** | Non-critical advisory condition | Blinking Yellow LED, 2s Chirp | Parameter normalized |
| `9` | **`STATE_FAULT_LATCH`** | Critical sensor fault / hardware error | Red LED Solid, Fast Beep, Scrubber ON | Manual Reset Command + Fault Condition Cleared |
| `10`| **`STATE_EMERGENCY_STOP`**| E-Stop tripped or door opened during active cycle | Red LED Solid, Fast Beep, Scrubber ON | E-Stop Released + Manual Reset Command |
| `11`| **`STATE_SERVICE_MODE`** | Technician manual maintenance & calibration | Relays OFF | Service keyswitch disengaged $\rightarrow$ `IDLE` |

---

## 2. Transition State Diagram

```mermaid
stateDiagram-v2
    [*] --> STATE_INIT
    STATE_INIT --> STATE_IDLE: Self-Test Passed (Door Open)
    STATE_INIT --> STATE_READY: Self-Test Passed (Door Closed)
    STATE_INIT --> STATE_FAULT_LATCH: Self-Test Failed

    STATE_IDLE --> STATE_READY: Door Closed
    STATE_READY --> STATE_IDLE: Door Opened
    STATE_READY --> STATE_SERVICE_MODE: Service Key Engaged
    STATE_IDLE --> STATE_SERVICE_MODE: Service Key Engaged

    STATE_READY --> STATE_PRE_PURGE: START Command (Button / Modbus)

    STATE_PRE_PURGE --> STATE_DISINFECTING: Pre-Purge Timer Expired (30s)
    STATE_PRE_PURGE --> STATE_FAULT_LATCH: Airflow Lost (Differential Switch Drop)
    STATE_PRE_PURGE --> STATE_EMERGENCY_STOP: E-Stop / Door Breach

    STATE_DISINFECTING --> STATE_DWELL_SOAK: Disinfect Timer Expired (300s)
    STATE_DISINFECTING --> STATE_EMERGENCY_STOP: E-Stop / Door Breach / Gas Leak
    STATE_DISINFECTING --> STATE_FAULT_LATCH: Over-Temp (>45°C) / Sensor Fault
    STATE_DISINFECTING --> STATE_POST_PURGE: STOP Command (Safe Cancel)

    STATE_DWELL_SOAK --> STATE_POST_PURGE: Dwell Timer Expired (120s)
    STATE_DWELL_SOAK --> STATE_EMERGENCY_STOP: E-Stop / Door Breach

    STATE_POST_PURGE --> STATE_COMPLETE: Post-Purge Timer Expired (60s)
    STATE_POST_PURGE --> STATE_FAULT_LATCH: Airflow Lost
    STATE_POST_PURGE --> STATE_EMERGENCY_STOP: E-Stop Tripped

    STATE_COMPLETE --> STATE_READY: 5s Chime Elapsed (Door Closed)
    STATE_COMPLETE --> STATE_IDLE: Door Opened

    STATE_EMERGENCY_STOP --> STATE_READY: E-Stop Released & Reset Acknowledged
    STATE_FAULT_LATCH --> STATE_READY: Sensor Normal & Reset Acknowledged
    STATE_SERVICE_MODE --> STATE_IDLE: Service Key Turned Off
```
