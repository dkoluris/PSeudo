class CstrSerial {
    enum {
        PAD_BTN_SELECT,
        PAD_BTN_START = 3,
        PAD_BTN_UP,
        PAD_BTN_RIGHT,
        PAD_BTN_DOWN,
        PAD_BTN_LEFT,
        PAD_BTN_L2,
        PAD_BTN_R2,
        PAD_BTN_L1,
        PAD_BTN_R1,
        PAD_BTN_TRIANGLE,
        PAD_BTN_CIRCLE,
        PAD_BTN_CROSS,
        PAD_BTN_SQUARE
    };
    
    enum {
        SIO_STAT_TX_READY = 0x001,
        SIO_STAT_RX_READY = 0x002,
        SIO_STAT_TX_EMPTY = 0x004,
        SIO_STAT_IRQ      = 0x200,
    };
    
    enum {
        SIO_CTRL_DTR         = 0x002,
        SIO_CTRL_RESET_ERROR = 0x010,
        SIO_CTRL_RESET       = 0x040,
    };
    
    uh btnState;
    ub bfr[5], index, step;
    
public:
    void reset();
    void padListener(int, bool);
    
    void write16(uw, uh);
    void write08(uw, ub);
    
    uh read16(uw);
    ub read08(uw);
};

extern CstrSerial sio;
