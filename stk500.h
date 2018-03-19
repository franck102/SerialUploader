
enum class Stk : char
{
    OK = 0x10,
    FAILED = 0x11,  // Not used
    UNKNOWN = 0x12,  // Not used
    NODEVICE = 0x13,  // Not used
    INSYNC = 0x14,  // ' '
    NOSYNC = 0x15,  // Not used
    ADC_CHANNEL_ERROR = 0x16,  // Not used
    ADC_MEASURE_OK = 0x17,  // Not used
    PWM_CHANNEL_ERROR = 0x18,  // Not used
    PWM_ADJUST_OK = 0x19,  // Not used
    CRC_EOP = 0x20  // 'SPACE'
};

enum class StkCommand
{
    GET_SYNC = 0x30,  // '0'
    GET_SIGN_ON = 0x31,  // '1'
    SET_PARAMETER = 0x40,  // '@'
    GET_PARAMETER = 0x41,  // 'A'
    SET_DEVICE = 0x42,  // 'B'
    SET_DEVICE_EXT = 0x45,  // 'E'
    ENTER_PROGMODE = 0x50,  // 'P'
    LEAVE_PROGMODE = 0x51,  // 'Q'
    CHIP_ERASE = 0x52,  // 'R'
    CHECK_AUTOINC = 0x53,  // 'S'
    LOAD_ADDRESS = 0x55,  // 'U'
    UNIVERSAL = 0x56,  // 'V'
    PROG_FLASH = 0x60,  // '`'
    PROG_DATA = 0x61,  // 'a'
    PROG_FUSE = 0x62,  // 'b'
    PROG_LOCK = 0x63,  // 'c'
    PROG_PAGE = 0x64,  // 'd'
    PROG_FUSE_EXT = 0x65,  // 'e'
    READ_FLASH = 0x70,  // 'p'
    READ_DATA = 0x71,  // 'q'
    READ_FUSE = 0x72,  // 'r'
    READ_LOCK = 0x73,  // 's'
    READ_PAGE = 0x74,  // 't'
    READ_SIGN = 0x75,  // 'u'
    READ_OSCCAL = 0x76,  // 'v'
    READ_FUSE_EXT = 0x77,  // 'w'
    READ_OSCCAL_EXT = 0x78  // 'x'
};

enum class StkParam : uint8_t
{
    SW_MAJOR = 0x81,
    STK_SW_MINOR = 0x82
};

enum class AvrCommand : uint8_t
{
    AVR_OP_LOAD_EXT_ADDR = 0x4d
};
