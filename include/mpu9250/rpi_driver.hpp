/// \file mpu9250/rpi_driver.hpp
/// \brief Defines the mpu9250::rpi_driver class.
#ifndef MPU9250___RPI_DRIVER_H
#define MPU9250___RPI_DRIVER_H

#include <mpu9250/driver.hpp>

namespace mpu9250 {

/// \brief A Raspberry Pi driver for the MPU9250.
class rpi_driver
    : public mpu9250::driver
{
public:
    // CONSTRUCTORS
    /// \brief Creates a new rpi_driver instance.
    /// \param i2c_bus The I2C bus to communicate with the MPU9250 over.
    /// \param i2c_address The I2C address of the MPU9250.
    /// \param interrupt_pin The Broadcom / BCM GPIO pin that is attached to the MPU9250 interrupt.
    rpi_driver(uint32_t i2c_bus, uint32_t i2c_address, uint8_t interrupt_pin);

    // INITIALIZATION
    /// \brief Initializes the MPU9250.
    /// \exception runtime_error if the initialization fails.
    void initialize();

    // CONTROL
    /// \brief Starts measurement collection on the MPU9250.
    void start();
    /// \brief Stops measurement collection on the MPU9250.
    void stop();

private:
    // VARIABLES
    /// \brief The I2C bus to interface with the MPU9250 over.
    const uint32_t m_i2c_bus;
    /// \brief The I2C address of the MPU9250.
    const uint32_t m_i2c_address;
    /// \brief The BCM GPIO pin that is attached to the MPU9250 interrupt.
    const uint8_t m_interrupt_pin;
    /// \brief A handle to the MPU9250 I2C connection.
    int32_t m_i2c_handle_mpu9250;
    /// \brief A handle to the AK8963 I2C connection.
    int32_t m_i2c_handle_ak8963;
    
    // OVERRIDES
    void initialize_i2c(uint32_t i2c_bus, uint32_t i2c_address) override;
    void deinitialize_i2c() override;
    void write_mpu9250_register(registers::mpu9250 address, uint8_t value) override;
    uint8_t read_mpu9250_register(registers::mpu9250 address) override;
    void read_mpu9250_registers(registers::mpu9250 start_address, uint32_t length, uint8_t* buffer) override;
    void write_ak8963_register(registers::ak8963 address, uint8_t value) override;
    uint8_t read_ak8963_register(registers::ak8963 address) override;
    void read_ak8963_registers(registers::ak8963 start_address, uint32_t length, uint8_t* buffer) override;

    // IO
    /// \brief Writes a byte to an I2C register.
    /// \param handle The I2C handle to write to.
    /// \param address The address of the register to write.
    /// \param value The value to write.
    void write_register(int32_t handle, uint8_t address, uint8_t value);
    /// \brief Reads a byte from an I2C register.
    /// \param handle The I2C handle to read from.
    /// \param address The address of the register to read.
    /// \returns The read value.
    uint8_t read_register(int32_t handle, uint8_t address);
    /// \brief Reads from a contiguous block of I2C registers.
    /// \param handle The I2C handle to read from.
    /// \param start_address The address of the register to start reading from.
    /// \param length The number of bytes to read.
    /// \param values The byte array to read values into.
    void read_registers(int32_t handle, uint8_t start_address, uint8_t length, uint8_t* values);

    // VISIBILITY
    using driver::initialize;
    using driver::read_measurement;

    // CALLBACKS
    /// \brief The edge-detection interrupt for the MPU9250 interrupt pin.
    /// \param pin The BCM GPIO pin number that an edge was detected for.
    /// \param level The type of edge detected.
    /// \param timestamp The timestamp of the detected edge.
    /// \param data A pointer to the rpi_driver instance.
    static void interrupt(int32_t pin, int32_t level, uint32_t timestamp, void* data);
};

}

#endif