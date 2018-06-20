#define FRAME_W \
    1024

#define FRAME_H \
    512

class CstrGraphics {
    // Constant enumerations
    enum {
        GPU_REG_DATA,
        GPU_REG_STATUS = 4
    };
    
    enum {
        GPU_STAT_ODDLINES         = 0x80000000,
        GPU_STAT_DMABITS          = 0x60000000,
        GPU_STAT_READYFORCOMMANDS = 0x10000000,
        GPU_STAT_READYFORVRAM     = 0x08000000,
        GPU_STAT_IDLE             = 0x04000000,
        GPU_STAT_DISPLAYDISABLED  = 0x00800000,
        GPU_STAT_INTERLACED       = 0x00400000,
        GPU_STAT_RGB24            = 0x00200000,
        GPU_STAT_PAL              = 0x00100000,
        GPU_STAT_DOUBLEHEIGHT     = 0x00080000,
        GPU_STAT_WIDTHBITS        = 0x00070000,
        GPU_STAT_MASKENABLED      = 0x00001000,
        GPU_STAT_MASKDRAWN        = 0x00000800,
        GPU_STAT_DRAWINGALLOWED   = 0x00000400,
        GPU_STAT_DITHER           = 0x00000200,
    };
    
    enum {
        GPU_DMA_NONE,
        GPU_DMA_MEM2VRAM = 2,
        GPU_DMA_VRAM2MEM
    };
    
    // Width
    const uh resMode[8] = {
        256, 320, 512, 640, 368, 384, 512, 640
    };
    
    // Primitive size
    const ub pSize[256] = {
        0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x00
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x10
        4, 4, 4, 4, 7, 7, 7, 7, 5, 5, 5, 5, 9, 9, 9, 9, // 0x20
        6, 6, 6, 6, 9, 9, 9, 9, 8, 8, 8, 8,12,12,12,12, // 0x30
        3, 3, 3, 3, 0, 0, 0, 0, 254, 254, 254, 254, 254, 254, 254, 254, // 0x40
        4, 4, 4, 4, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255, // 0x50
        3, 3, 3, 3, 4, 4, 4, 4, 2, 2, 2, 2, 3, 3, 3, 3, // 0x60
        2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3, // 0x70
        4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x80
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x90
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xa0
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xb0
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xc0
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xd0
        0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xe0
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xf0
    };
    
    // Command buffer
    struct {
        uw data[256], prim, size, row;
    } pipe;
    
    uw modeDMA;
    uh vpos, vdiff;
    
    int fetchMem(uh *, sw);
    void dataWrite(uw *, sw);
    void dataRead (uw *, sw);
    
public:
    CstrGraphics() {
        vram.ptr = new uh[vram.size = FRAME_W * FRAME_H * 2];
    }
    
    ~CstrGraphics() {
        delete[] vram.ptr;
    }
    
    // VRAM
    struct heap {
        uh *ptr; uw size;
    } vram;
    
    // VRAM operations
    struct {
        bool enabled;
        
        struct {
            sw start, end, p;
        } h, v;
    } vrop;
    
    // Data, Status
    struct {
        bool disabled; uw data, status;
    } ret;
    
    bool isVideoPAL;
    
    void reset();
    void refresh();
    void write(uw, uw);
    uw read(uw);
    void photoRead(uw *);
    void executeDMA(CstrBus::castDMA *);
};

extern CstrGraphics vs;

//union {
//    struct {
//        uw tw         : 4;
//        uw th         : 1;
//        uw abr        : 2;
//        uw tp         : 2;
//        uw dtd        : 1;
//        uw dfe        : 1;
//        uw md         : 1;
//        uw me         : 1;
//        uw reserved   : 1;
//        uw rev        : 1;
//        uw texdisable : 1;
//        uw w          : 3;
//        uw h          : 1;
//        uw video      : 1;
//        uw is24       : 1;
//        uw isinter    : 1;
//        uw den        : 1;
//        uw irq1       : 1;
//        uw dmareq     : 1;
//        uw occupied   : 1;
//        uw photo      : 1;
//        uw com        : 1;
//        uw dmadir     : 2;
//        uw lcf        : 1;
//    };
//    
//    uw raw;
//} stat;

//enum {
//    GPU_STAT_TX0,
//    GPU_STAT_TX1,
//    GPU_STAT_TX2,
//    GPU_STAT_TX3,
//    GPU_STAT_TY,
//    GPU_STAT_ABR0,
//    GPU_STAT_ABR1,
//    GPU_STAT_CLUT_8,
//    GPU_STAT_NO_CLUT,
//    GPU_STAT_DITHER,
//    GPU_STAT_DRAW_ENABLED,
//    GPU_STAT_MASK_DRAWN,
//    GPU_STAT_MASK_ENABLED,
//    GPU_STAT_MIRROR_X,
//    GPU_STAT_MIRROR_Y,
//    GPU_STAT_UNK2,
//    GPU_STAT_WIDTH0,
//    GPU_STAT_WIDTH1,
//    GPU_STAT_WIDTH2,
//    GPU_STAT_HEIGHT,
//    GPU_STAT_PAL,
//    GPU_STAT_RGB24,
//    GPU_STAT_INTERLACED,
//    GPU_STAT_DISPLAY_DISABLED,
//    GPU_STAT_UNK3,
//    GPU_STAT_UNK4,
//    GPU_STAT_IDLE,
//    GPU_STAT_READY_FOR_IMG,
//    GPU_STAT_READY_FOR_CMD,
//    GPU_STAT_DMA_DIRECTION,
//    GPU_STAT_DMA_ENABLED,
//    GPU_STAT_ODD_LINE
//};
