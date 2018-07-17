//class CstrCounters {
//    // Constant enumerations
//    enum {
//        PSX_CLOCK      = 33868800,
//        PSX_VSYNC_NTSC = PSX_CLOCK / 60,
//        PSX_VSYNC_PAL  = PSX_CLOCK / 50,
//        PSX_HSYNC      = PSX_CLOCK / 15734,
//        PSX_BIAS       = 64 // This is uttermost experimental, it's the Achilles' heel
//    };
//    
//    enum {
//        RTC_COUNT  = 0,
//        RTC_MODE   = 4,
//        RTC_TARGET = 8,
//        RTC_BOUND  = 0xffff
//    };
//    
//    uh bounds[3];
//    uw vbk, hbk;
//    
//public:
//    void reset();
//    void update();
//    
//    // Store
//    template <class T>
//    void write(uw, T);
//    
//    // Load
//    template <class T>
//    T read(uw);
//};
//
//extern CstrCounters rootc;

typedef struct {
    uw count, mode, target;
    uw sCycle, Cycle, rate, interrupt;
} psxCounter;

extern psxCounter psxCounters[5];

extern uw psxNextCounter, psxNextsCounter;

void psxRcntInit();
void psxRcntUpdate();
void psxRcntWcount(uw index, uw value);
void psxRcntWmode(uw index, uw value);
void psxRcntWtarget(uw index, uw value);
uw psxRcntRcount(uw index);

void psxUpdateVSyncRate();
