# =============================================================================
# Makefile for Smart Industrial Disinfection Controller
# Author: Sherifred Singh
# =============================================================================

CC      ?= gcc
CFLAGS  ?= -std=c99 -Wall -Wextra -pedantic -O2 \
           -I firmware/hal \
           -I firmware/drivers \
           -I firmware/services \
           -I firmware/app \
           -I tests/unity

BIN_DIR ?= build
TEST_BIN = $(BIN_DIR)/test_runner.exe
SIM_BIN  = $(BIN_DIR)/sim_cli.exe

# Core Firmware Source Files
FW_SRCS = \
	firmware/port/host_sim/hal_gpio_sim.c \
	firmware/port/host_sim/hal_timer_sim.c \
	firmware/port/host_sim/hal_adc_sim.c \
	firmware/port/host_sim/hal_uart_sim.c \
	firmware/port/host_sim/hal_nvm_sim.c \
	firmware/port/host_sim/hal_watchdog_sim.c \
	firmware/services/ring_buffer.c \
	firmware/services/crc16.c \
	firmware/services/filter_service.c \
	firmware/services/config_manager.c \
	firmware/services/alarm_fault_manager.c \
	firmware/services/event_logger.c \
	firmware/services/modbus_rtu_slave.c \
	firmware/drivers/sensor_driver.c \
	firmware/drivers/actuator_driver.c \
	firmware/drivers/nvm_driver.c \
	firmware/app/app_fsm.c \
	firmware/app/process_controller.c

# Test Runner Source Files
TEST_SRCS = \
	tests/test_runner.c \
	tests/unity/unity.c \
	tests/unit/test_ring_buffer.c \
	tests/unit/test_crc16.c \
	tests/unit/test_filter_service.c \
	tests/unit/test_config_manager.c \
	tests/unit/test_alarm_fault.c \
	tests/unit/test_modbus_rtu.c \
	tests/unit/test_app_fsm.c \
	tests/integration/test_sensor_to_fsm.c \
	tests/integration/test_modbus_to_config.c \
	tests/system/test_full_cycle.c \
	$(FW_SRCS)

# Simulation CLI Source Files
SIM_SRCS = \
	tools/sim_cli/main_sim.c \
	$(FW_SRCS)

.PHONY: all test sim clean

all: test sim

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

test: $(BIN_DIR)
	@echo "[BUILD] Compiling Test Runner..."
	$(CC) $(CFLAGS) $(TEST_SRCS) -o $(TEST_BIN)
	@echo "[RUN] Executing Automated Unit, Integration & System Test Suites:"
	@$(TEST_BIN)

sim: $(BIN_DIR)
	@echo "[BUILD] Compiling Interactive CLI Simulation Engine..."
	$(CC) $(CFLAGS) $(SIM_SRCS) -o $(SIM_BIN)
	@echo "[READY] Built $(SIM_BIN). Run '$(SIM_BIN)' to launch interactive simulation console."

clean:
	rm -rf $(BIN_DIR)
	@echo "[CLEAN] Build artifacts removed."
