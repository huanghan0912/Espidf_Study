/**
 * @file I2c.cpp
 * @author huanghan0912 (huanghan0912@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-22
 * 
 * 
 */
#include "I2c.h"
/**
 * @brief 
 * 
 * @param port 
 * @param slv_rx_buf_len 
 * @param slv_tx_buf_len 
 * @param intr_alloc_flags 
 */
esp_err_t I2c::Init(i2c_port_t port, size_t I2C_SDA , size_t I2C_SCL , int i2c_master_freqhz ){
    _port=port;
    i2c_config_t conf{};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_SDA;      // select GPIO specific to your project
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_SCL;         // select GPIO specific to your project
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = i2c_master_freqhz; // select frequency specific to your project
    i2c_param_config(port, &conf);
    return i2c_driver_install(port, conf.mode, 0,0, 0);
}

uint8_t I2c::ReadByte(uint8_t dev_addr, uint8_t reg_addr)
{
    uint8_t rxBuf{};
    i2c_master_write_read_device(_port, dev_addr, &reg_addr, 1, &rxBuf, 1, pdMS_TO_TICKS(1000));
    return rxBuf;
}

esp_err_t I2c::WriteByte(uint8_t dev_addr, uint8_t reg_addr, uint8_t txData){
    const uint8_t txBuf[2] {reg_addr, txData};
    return i2c_master_write_to_device(_port, dev_addr, txBuf, 2, pdMS_TO_TICKS(1000));
}

esp_err_t I2c::ReadMultipleDatas(uint8_t dev_addr, uint8_t reg_addr, uint8_t *rx_data, int length)
{
    return i2c_master_write_read_device(_port, dev_addr, &reg_addr, 1, rx_data, length, pdMS_TO_TICKS(1000));
}

esp_err_t I2c::WriteMultipleDatas(uint8_t dev_addr, uint8_t reg_addr, uint8_t *tx_data, int length)
{
    esp_err_t status = ESP_OK;
    uint8_t buffer[I2C_LINK_RECOMMENDED_SIZE(3)] = { 0 };

    i2c_cmd_handle_t _handle = i2c_cmd_link_create_static(buffer, sizeof(buffer));
    status |= i2c_master_start(_handle);
    status |= i2c_master_write_byte(_handle, (dev_addr << 1) | I2C_MASTER_WRITE, true);
    status |= i2c_master_write_byte(_handle, reg_addr, true);
    status |= i2c_master_write(_handle, tx_data, length, true);
    status |= i2c_master_stop(_handle);
    status |= i2c_master_cmd_begin(_port, _handle, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete_static(_handle);

    return status;
}

