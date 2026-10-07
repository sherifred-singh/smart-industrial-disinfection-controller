# Changelog

All notable changes to the **Smart Industrial Disinfection & Environment Monitoring Controller** firmware will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2026-09-22

### Added
- **Layered Architecture:** 4-tier modular firmware architecture (HAL $\rightarrow$ Drivers $\rightarrow$ Services $\rightarrow$ Application).
- **Deterministic Multi-Rate Scheduler:** Cooperative non-blocking time slots (1ms, 10ms, 100ms, 1s) with monotonic millisecond tick.
- **12-State Process FSM:** Event-driven state machine handling `INIT`, `IDLE`, `READY`, `PRE_PURGE`, `DISINFECTING`, `DWELL_SOAK`, `POST_PURGE`, `COMPLETE`, `WARNING`, `FAULT_LATCH`, `EMERGENCY_STOP`, and `SERVICE_MODE`.
- **Digital Signal Conditioning:** Dual-stage cascaded Median-3 spike rejection filter and fixed-point Exponential Moving Average (EMA) filter.
- **NVM Configuration Manager:** Parameter storage with magic ID (`0x53494443`), schema versioning, CRC-16-CCITT integrity validation, and automated factory default recovery.
- **Modbus RTU Slave Stack:** RS-485 communications supporting Function Codes 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, and 0x10 with 3.5-character silence timeout.
- **Safety Supervisory:** Immediate ($<10\,\mu\text{s}$) actuator cutoff on door breach, E-stop, or chamber over-temperature with hardware latching and physical reset verification.
- **Hardware Abstraction Ports:** Native Host Simulator (x86/MinGW) and 32-bit ARM Cortex-M (STM32F4) HAL implementations.
- **Automated Test Suite:** 10 test suites covering unit tests, integration tests, and deterministic time-travel system tests with 100% pass rate.
- **Interactive CLI Simulator:** Terminal console with live ANSI telemetry dashboard and keyboard-driven sensor/fault stimulus controls.
- **GitHub Actions CI/CD:** Automated compilation, static analysis (cppcheck), and unit test execution on push/PR.
