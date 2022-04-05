#include <mpu9250/rpi_driver.hpp>

#include <pigpio.h>

#include <stdexcept>
#include <limits>

using namespace mpu9250;

// CONSTRUCTORS
rpi_driver::rpi_driver(uint32_t i2c_bus, uint32_t i2c_address, uint8_t interrupt_pin)
    : m_i2c_bus(i2c_bus),
      m_i2c_address(i2c_address),
      m_interrupt_pin(interrupt_pin),
      m_i2c_handle_mpu9250(-1),
      m_i2c_handle_ak8963(-1)
{}

// INITIALIZATION
void rpi_driver::initialize()
{
    // Call base class method with I2C configuration.
    driver::initialize(rpi_driver::m_i2c_bus, rpi_driver::m_i2c_address);
}

// CONTROL
void rpi_driver::start()
{
    // Enable pigpio interrupt.
    switch(gpioSetISRFuncEx(rpi_driver::m_interrupt_pin, RISING_EDGE, std::numeric_limits<int32_t>::max(), &rpi_driver::interrupt, this))
    {
        case 0:
        {
            // Do nothing, succeeded.
            break;
        }
        case PI_BAD_GPIO:
        {
            throw std::runtime_error("invalid gpio pin: " + std::to_string(rpi_driver::m_interrupt_pin));
        }
        case PI_BAD_ISR_INIT:
        {
            throw std::runtime_error("initialize interrupt failed for pin: " + std::to_string(rpi_driver::m_interrupt_pin));
        }
        default:
        {
            throw std::runtime_error("unknown error");
        }
    }
}
void rpi_driver::stop()
{
    // Disable pigpio interrupt.
    gpioSetISRFuncEx(rpi_driver::m_interrupt_pin, RISING_EDGE, std::numeric_limits<int32_t>::max(), nullptr, this);
}

// OVERRIDES
void rpi_driver::initialize_i2c(uint32_t i2c_bus, uint32_t i2c_address)
{
    // Open an I2C connection with the MPU9250.
    int32_t result = i2cOpen(rpi_driver::m_i2c_bus, rpi_driver::m_i2c_address, 0);
    if(result >= 0)
    {
        // Store I2C handle.
        rpi_driver::m_i2c_handle_mpu9250 = result;
    }
    else
    {
        // Error occured.
        switch(result)
        {
            case PI_BAD_I2C_BUS:
            {
                throw std::runtime_error("invalid i2c bus: " + std::to_string(rpi_driver::m_i2c_bus));
            }
            case PI_BAD_I2C_ADDR:
            {
                throw std::runtime_error("invalid mpu9250 i2c address: " + std::to_string(rpi_driver::m_i2c_address));
            }
            case PI_BAD_FLAGS:
            {
                throw std::runtime_error("invalid mpu9250 i2c flags");
            }
            case PI_NO_HANDLE:
            {
                throw std::runtime_error("failed to get mpu9250 i2c handle");
            }
            case PI_I2C_OPEN_FAILED:
            {
                throw std::runtime_error("failed to open mpu9250 i2c");
            }
            default:
            {
                throw std::runtime_error("mpu9250 i2c unknown error");
            }
        }
    }

    // Open an I2C connection with the AK8963.
    // NOTE: The AK8963 has a hardcoded I2C address of 0x0C.
    result = i2cOpen(rpi_driver::m_i2c_bus, 0x0C, 0);
    if(result >= 0)
    {
        // Store I2C handle.
        rpi_driver::m_i2c_handle_ak8963 = result;
    }
    else
    {
        // Error occured.
        switch(result)
        {
            case PI_BAD_I2C_BUS:
            {
                throw std::runtime_error("invalid i2c bus: " + std::to_string(rpi_driver::m_i2c_bus));
            }
            case PI_BAD_I2C_ADDR:
            {
                throw std::runtime_error("invalid ak8963 i2c address: 0x0C");
            }
            case PI_BAD_FLAGS:
            {
                throw std::runtime_error("invalid ak8963 i2c flags");
            }
            case PI_NO_HANDLE:
            {
                throw std::runtime_error("failed to get ak8963 i2c handle");
            }
            case PI_I2C_OPEN_FAILED:
            {
                throw std::runtime_error("failed to open ak8963 i2c");
            }
            default:
            {
                throw std::runtime_error("ak8963 i2c unknown error");
            }
        }
    }
}
void rpi_driver::deinitialize_i2c()
{
    // Close MPU9250 I2C connection.
    if(i2cClose(rpi_driver::m_i2c_handle_mpu9250) == 0)
    {
        // Clear internal handle.
        rpi_driver::m_i2c_handle_mpu9250 = -1;
    }

    // Close AK8963 I2C connection.
    if(i2cClose(rpi_driver::m_i2c_handle_ak8963) == 0)
    {
        // Clear internal handle.
        rpi_driver::m_i2c_handle_ak8963 = -1;
    }
}
void rpi_driver::write_mpu9250_register(registers::mpu9250 address, uint8_t value)
{
    rpi_driver::write_register(rpi_driver::m_i2c_handle_mpu9250, static_cast<uint8_t>(address), value);
}
uint8_t rpi_driver::read_mpu9250_register(registers::mpu9250 address)
{
    return rpi_driver::read_register(rpi_driver::m_i2c_handle_mpu9250, static_cast<uint8_t>(address));
}
void rpi_driver::read_mpu9250_registers(registers::mpu9250 start_address, uint32_t length, uint8_t* buffer)
{
    rpi_driver::read_registers(rpi_driver::m_i2c_handle_mpu9250, static_cast<uint8_t>(start_address), length, buffer);
}
void rpi_driver::write_ak8963_register(registers::ak8963 address, uint8_t value)
{
    rpi_driver::write_register(rpi_driver::m_i2c_handle_ak8963, static_cast<uint8_t>(address), value);    
}
uint8_t rpi_driver::read_ak8963_register(registers::ak8963 address)
{
    return rpi_driver::read_register(rpi_driver::m_i2c_handle_ak8963, static_cast<uint8_t>(address));
}
void rpi_driver::read_ak8963_registers(registers::ak8963 start_address, uint32_t length, uint8_t* buffer)
{
    rpi_driver::read_registers(rpi_driver::m_i2c_handle_ak8963, static_cast<uint8_t>(start_address), length, buffer);
}

// IO
void rpi_driver::write_register(int32_t handle, uint8_t address, uint8_t value)
{
    // Write data and handle result.
    switch(i2cWriteByteData(handle, address, value))
    {
        case 0:
        {
            // Success.
            break;
        }
        case PI_BAD_HANDLE:
        {
            throw std::runtime_error("i2c not open");
        }
        case PI_BAD_PARAM:
        {
            throw std::runtime_error("invalid write command");
        }
        case PI_I2C_WRITE_FAILED:
        {
            throw std::runtime_error("i2c write failed");
        }
    }
}
uint8_t rpi_driver::read_register(int32_t handle, uint8_t address)
{
    // Read data and handle result.
    int32_t result = i2cReadByteData(handle, address);
    if(result >= 0)
    {
        // Read succeeded.
        return static_cast<uint8_t>(result);
    }
    else
    {
        switch(result)
        {
            case PI_BAD_HANDLE:
            {
                throw std::runtime_error("i2c not open");
            }
            case PI_BAD_PARAM:
            {
                throw std::runtime_error("invalid read command");
            }
            case PI_I2C_READ_FAILED:
            {
                throw std::runtime_error("i2c read failed");
            }
            default:
            {
                throw std::runtime_error("unknown error");
            }
        }
    }
}
void rpi_driver::read_registers(int32_t handle, uint8_t start_address, uint8_t length, uint8_t* values)
{
    // Read block and handle result.
    int32_t result = i2cReadI2CBlockData(handle, start_address, reinterpret_cast<char*>(values), length);
    if(result >= 0)
    {
        // Check if enough bytes were read.
        if(result != length)
        {
            throw std::runtime_error("incorrect number of bytes read");
        }
    }
    else
    {
        switch(result)
        {
            case PI_BAD_HANDLE:
            {
                throw std::runtime_error("i2c not open");
            }
            case PI_BAD_PARAM:
            {
                throw std::runtime_error("invalid read command");
            }
            case PI_I2C_READ_FAILED:
            {
                throw std::runtime_error("i2c read failed");
            }
            default:
            {
                throw std::runtime_error("unknown error");
            }
        }
    }
}

// CALLBACKS
void rpi_driver::interrupt(int32_t pin, int32_t level, uint32_t timestamp, void* data)
{
    // Validate level and data.
    if(level == PI_HIGH)
    {
        // Cast rpi_driver pointer to base driver.
        mpu9250::driver* driver = reinterpret_cast<mpu9250::driver*>(data);

        // Instruct the driver to read the latest measurement.
        driver->read_measurement();
    }
}