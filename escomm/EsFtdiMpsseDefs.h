/// Do not include this file into project explicitly, it's for internal use only
///

namespace MPSSE {
  enum {
    CMD_ECHO_1                        = 0xAA,
    CMD_ECHO_2                        = 0xAB,
    BAD_COMMAND_RESPONSE              = 0xFA,

    SET_LOW_BYTE_DATA_BITS_DATA       = 0x13,
    SET_HIGH_BYTE_DATA_BITS_DATA      = 0x0F,

    CMD_SET_CLOCK_FREQUENCY           = 0x86,
    CMD_TURN_ON_LOOPBACK              = 0x84,
    CMD_TURN_OFF_LOOPBACK             = 0x85,
        
    // MPSSE Control Commands   
    CMD_SET_DATA_BITS_LOWBYTE         = 0x80,
    CMD_GET_DATA_BITS_LOWBYTE         = 0x81,
    CMD_SET_DATA_BITS_HIGHBYTE        = 0x82,
    CMD_GET_DATA_BITS_HIGHBYTE        = 0x83,
    
    CMD_SEND_IMMEDIATE                = 0x87,
    CMD_ENABLE_3PHASE_CLOCKING        = 0x8C,
    CMD_DISABLE_3PHASE_CLOCKING       = 0x8D,
    CMD_ENABLE_DRIVE_ONLY_ZERO        = 0x9E,

    // MPSSE Data Commands - bit mode - MSB first
    CMD_DATA_OUT_BITS_POS_EDGE        = 0x12,
    CMD_DATA_OUT_BITS_NEG_EDGE        = 0x13,
    CMD_DATA_IN_BITS_POS_EDGE         = 0x22,
    CMD_DATA_IN_BITS_NEG_EDGE         = 0x26,
    CMD_DATA_BITS_IN_POS_OUT_NEG_EDGE = 0x33,
    CMD_DATA_BITS_IN_NEG_OUT_POS_EDGE = 0x36,

    // MPSSE Data Commands - byte mode - MSB first
    CMD_DATA_OUT_BYTES_POS_EDGE       = 0x10,
    CMD_DATA_OUT_BYTES_NEG_EDGE       = 0x11,
    CMD_DATA_IN_BYTES_POS_EDGE        = 0x20,
    CMD_DATA_IN_BYTES_NEG_EDGE        = 0x24,
    CMD_DATA_BYTES_IN_POS_OUT_NEG_EDGE= 0x31,
    CMD_DATA_BYTES_IN_NEG_OUT_POS_EDGE= 0x34,

    // SCL & SDA directions
    DIRECTION_SCLIN_SDAIN             = 0x10,
    DIRECTION_SCLOUT_SDAIN            = 0x11,
    DIRECTION_SCLIN_SDAOUT            = 0x12,
    DIRECTION_SCLOUT_SDAOUT           = 0x13,

    // SCL & SDA values
    VALUE_SCLLOW_SDALOW               = 0x00,
    VALUE_SCLHIGH_SDALOW              = 0x01,
    VALUE_SCLLOW_SDAHIGH              = 0x02,
    VALUE_SCLHIGH_SDAHIGH             = 0x03,

    // Data size in bits
    DATA_SIZE_8BITS                   = 0x07,
    DATA_SIZE_1BIT                    = 0x00,

    DISABLE_CLOCK_DIVIDE              = 0x8A,
    ENABLE_CLOCK_DIVIDE               = 0x8B,
    
    DISABLE_EVENT                     = 0,
    DISABLE_CHAR                      = 0,
    
    I2C_ENABLE_DRIVE_ONLY_ZERO        = 0x0002,
    I2C_DISABLE_3PHASE_CLOCKING       = 0x0001,
    I2C_ADDRESS_READ_MASK             = 0x01,
    I2C_ADDRESS_WRITE_MASK            = 0xFE,

    START_DURATION_1                  = 10,
    START_DURATION_2                  = 20,

    STOP_DURATION_1                   = 10,
    STOP_DURATION_2                   = 10,
    STOP_DURATION_3                   = 10,

    SEND_ACK                          = 0x00,
    SEND_NACK                         = 0x80,

    /* Bit definition of the Options member of configOptions structure */
    SPI_CONFIG_OPTION_MODE_MASK       = 0x00000003,
    SPI_CONFIG_OPTION_MODE0           = 0x00000000,
    SPI_CONFIG_OPTION_MODE1           = 0x00000001,
    SPI_CONFIG_OPTION_MODE2           = 0x00000002,
    SPI_CONFIG_OPTION_MODE3           = 0x00000003,

    SPI_CONFIG_OPTION_CS_MASK         = 0x0000001C,    /* 111 00 */
    SPI_CONFIG_OPTION_CS_DBUS3        = 0x00000000,    /* 000 00 */
    SPI_CONFIG_OPTION_CS_DBUS4        = 0x00000004,    /* 001 00 */
    SPI_CONFIG_OPTION_CS_DBUS5        = 0x00000008,    /* 010 00 */
    SPI_CONFIG_OPTION_CS_DBUS6        = 0x0000000C,    /* 011 00 */
    SPI_CONFIG_OPTION_CS_DBUS7        = 0x00000010,    /* 100 00 */

    SPI_CONFIG_OPTION_CS_ACTIVELOW    = 0x00000020,

    MAX_IN_BUF_SIZE                   = 4096,

    _6MHZ                             = 6000000,
    _30MHZ                            = 30000000,
    DEVICE_READ_TIMEOUT               = 5000,
    DEVICE_WRITE_TIMEOUT              = 5000,
    DEVICE_TIMEOUT_INFINITE           = 0,
    
    USB_INPUT_BUFFER_SIZE             = 65536,
    USB_OUTPUT_BUFFER_SIZE            = USB_INPUT_BUFFER_SIZE
  };
}
//---------------------------------------------------------------------------
