# System Overview

## 1. Executive Summary

The **Smart Industrial Disinfection & Environment Monitoring Controller** is an open-source, production-grade embedded firmware system engineered for industrial automated sterilization chambers, cleanrooms, and medical enclosure decontamination units.

The controller oversees the entire automated disinfection cycle—enforcing strict pre-purge ventilation, chemical/UV-C active dispersal, contact soak dwell timing, and post-purge atmospheric scrubbing—while supervising multi-tier safety interlocks, environmental gas concentrations, and external room leak boundaries.

---

## 2. Core Problem Solved

Industrial and laboratory decontamination equipment requires deterministic, fail-safe sequencing:
1. **Safety Interlocking:** Preventing human exposure by locking chamber doors during active cycles and guaranteeing immediate (<10 µs) actuator de-energization if an interlock is breached or Emergency Stop is pressed.
2. **Atmospheric Clearance Assurance:** Measuring chamber and exhaust scrubber concentration to ensure active chemical levels drop below OSHA/safety thresholds prior to releasing the electromagnetic door lock.
3. **Parameter Adaptability & SCADA Integration:** Providing plant managers and PLC/SCADA automation systems with bidirectional Modbus RTU communication over RS-485 to configure cycle profiles, monitor real-time telemetry, and log safety events.
4. **NVM Data Integrity:** Guaranteeing that critical process recipes and safety limits stored in non-volatile memory cannot be corrupted by power dips or flash wear.

---

## 3. High-Level Operational Flow

```text
[ Power-On Reset ]
       ↓
[ Hardware Self-Test (500ms) ]
       ↓
[ IDLE / READY ] (Green Beacon ON, Interlocks Verified)
       ↓ (Start Button Pressed / Modbus Command)
[ PRE-PURGE (30s) ] (Exhaust Fan ON, Airflow Differential Verified, Door Locked)
       ↓
[ DISINFECTING (300s) ] (Disinfection Actuator ON, Yellow Beacon, Temp/Leak Monitored)
       ↓
[ DWELL / SOAK (120s) ] (Actuators OFF, Sealed Chamber Contact Time)
       ↓
[ POST-PURGE (60s) ] (Scrubber Fan ON, Gas Concentration Evacuated)
       ↓
[ COMPLETE (5s) ] (Door Unlocked, Audible Completion Chime)
       ↓
[ Return to READY ]
```

---

## 4. Educational & Simulation Safety Notice

> [!IMPORTANT]
> This firmware implementation is designed strictly as an educational and portfolio reference demonstrating senior-level embedded architecture, state machines, signal processing, and communication stacks. It controls safe simulated actuators and digital relays and is not intended for direct uncontrolled connection to real hazardous gas equipment without certification.
