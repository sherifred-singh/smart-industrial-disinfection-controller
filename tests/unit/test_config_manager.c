/**
 * @file test_config_manager.c
 * @brief Unit Tests for NVM Configuration Store & Corruption Recovery
 * @author Sherifred Singh
 */

#include "unity.h"
#include "config_manager.h"
#include "hal_nvm.h"

extern void hal_nvm_sim_corrupt_byte(uint32_t offset, uint8_t bad_value);

void test_config_manager_default_initialization(void)
{
    hal_nvm_erase();
    bool loaded = config_manager_init();
    TEST_ASSERT_FALSE(loaded);

    const config_data_t *cfg = config_manager_get();
    TEST_ASSERT_EQUAL_UINT(CONFIG_MAGIC_WORD, cfg->magic_word);
    TEST_ASSERT_EQUAL_UINT(30, cfg->pre_purge_duration_s);
    TEST_ASSERT_EQUAL_UINT(300, cfg->disinfect_duration_s);
    TEST_ASSERT_EQUAL_UINT(1, cfg->modbus_slave_id);
}

void test_config_manager_save_and_reload(void)
{
    config_data_t updated = *config_manager_get();
    updated.pre_purge_duration_s = 45;
    updated.disinfect_duration_s = 450;
    updated.target_concentration_ppm = 80;

    TEST_ASSERT_TRUE(config_manager_save(&updated));

    TEST_ASSERT_TRUE(config_manager_init());

    const config_data_t *cfg = config_manager_get();
    TEST_ASSERT_EQUAL_UINT(45, cfg->pre_purge_duration_s);
    TEST_ASSERT_EQUAL_UINT(450, cfg->disinfect_duration_s);
    TEST_ASSERT_EQUAL_UINT(80, cfg->target_concentration_ppm);
}

void test_config_manager_corruption_recovery(void)
{
    hal_nvm_sim_corrupt_byte(8, 0xDE);

    bool valid = config_manager_init();
    TEST_ASSERT_FALSE(valid);

    const config_data_t *cfg = config_manager_get();
    TEST_ASSERT_EQUAL_UINT(30, cfg->pre_purge_duration_s);
    TEST_ASSERT_EQUAL_UINT(300, cfg->disinfect_duration_s);
}

void run_test_config_manager(void)
{
    RUN_TEST(test_config_manager_default_initialization);
    RUN_TEST(test_config_manager_save_and_reload);
    RUN_TEST(test_config_manager_corruption_recovery);
}
