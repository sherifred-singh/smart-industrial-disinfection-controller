# Verification & Testing Strategy

## 1. Multi-Tier Testing Pyramid

The project employs a comprehensive test-driven engineering approach across 3 tiers:

```text
                  / \
                 /   \
                / SYS \       Tier 3: End-to-End System Tests (Deterministic Time Travel)
               /-------\
              / INTEGR. \     Tier 2: Multi-Module Integration (Sensor -> FSM -> Actuator)
             /-----------\
            /  UNIT TESTS \   Tier 1: Isolated Algorithmic Units (Ring Buffer, CRC, EMA)
           /---------------\
```

---

## 2. Test Execution Matrix

| Test Suite | Test Case ID | Scope / Target Under Test | Pass Criteria |
| :--- | :--- | :--- | :--- |
| **01. Ring Buffer** | `TC_RB_01` | SPSC push/pop operations | FIFO sequence preservation, zero data loss |
| | `TC_RB_02` | Buffer overflow boundary check | Rejects byte when full, preserves existing data |
| **02. Checksum** | `TC_CRC_01`| CRC-16-CCITT algorithm | Known ASCII test vector "123456789" = `0x29B1` |
| | `TC_CRC_02`| CRC-16-Modbus algorithm | Standard Modbus frame vector = `0xCDC5` |
| **03. Filters** | `TC_FLT_01`| Fixed-point EMA filter step response | Mathematical convergence at 50% alpha |
| | `TC_FLT_02`| Median-3 impulse noise rejection | 1000mV electrical noise spike completely filtered out |
| **04. NVM Store** | `TC_CFG_01`| Virgin EEPROM initialization | Loads defaults, writes to flash with valid CRC |
| | `TC_CFG_02`| Persistence & reboot reload | Parameter updates survive simulated power reset |
| | `TC_CFG_03`| Corrupted byte recovery | CRC mismatch triggers auto-restore of defaults |
| **05. Alarm Manager**| `TC_ALM_01`| Severity priority handling | Critical fault overrides warning, sets Red LED |
| | `TC_ALM_02`| E-Stop unlatch lock | Cannot clear latched fault while E-Stop contact is open |
| **06. Modbus RTU** | `TC_MB_01` | FC 0x03 Read Holding Registers | Returns 13-byte response frame with valid CRC |
| | `TC_MB_02` | FC 0x06 Write Single Register | Updates target parameter and persists to NVM |
| | `TC_MB_03` | CRC Error Frame Drop | Drops malformed frames without replying |
| **07. Process FSM** | `TC_FSM_01`| Normal 4-phase progression | Pre-purge $\rightarrow$ Disinfect $\rightarrow$ Dwell $\rightarrow$ Post-purge |
| | `TC_FSM_02`| Emergency Stop override | Immediate cut of disinfection relay in $<1\text{ms}$ |
| **08. Integration** | `TC_INT_01`| Door breach during active dispersal | Door opening trips E-Stop and de-energizes relay |
| | `TC_INT_02`| Chamber over-temperature trip | Temp $>45^\circ\text{C}$ triggers `STATE_FAULT_LATCH` |
| **09. Integration** | `TC_INT_03`| Modbus FC 0x10 batch configuration | Multi-register write saved to NVM and validated |
| **10. System Test** | `TC_SYS_01`| Complete automated decontamination | Full multi-minute cycle execution with timer ticks |

---

## 3. Automated CI/CD Test Results

All 10 test suites compile cleanly under native GCC with `-Wall -Wextra -Werror -pedantic` and execute in under $50\,\text{ms}$ in GitHub Actions CI.
