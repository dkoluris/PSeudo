class CstrAudio {
    enum {
        SAMPLE_RATE    =  44100,
        MAX_CHANNELS   =     24,
        SBUF_SIZE      =    256,
        MAX_VOLUME     = 0x3fff,
        ALC_BUF_AMOUNT =     16,
    };
    
    const sh f[5][2] = {
        {   0,   0 },
        {  60,   0 },
        { 115, -52 },
        {  98, -55 },
        { 122, -60 },
    };
    
    union {
        sh ish[1024 * 256];
        ub iub[1024 * 256 * 2];
    } spuMem;
    
    struct voice {
        bool on;
        
        union {
            sh ish[USHRT_MAX];
            ub iub[USHRT_MAX * 2];
        } buffer;
        
        uw count;
        sw pos, size;
        sh freq, raddr;
        uw saddr;
        sh volumeL, volumeR;
    } spuVoices[MAX_CHANNELS];
    
    struct {
        sw temp[SBUF_SIZE * 2];
        sh  fin[SBUF_SIZE * 2];
    } sbuf;
    
    sw spuAddr;
    sh spuVolumeL, spuVolumeR;
    bool stereo;
    
    // OpenAL
    ALCdevice *device;
    ALCcontext *ctx;
    ALuint source;
    ALuint bfr[ALC_BUF_AMOUNT];
    
    void stream();
    void depackVAG(voice *);
    void voiceOn(uh);
    void voiceOff(uh);
    void dataWrite(uw, uw);
    
public:
    CstrAudio() {
        // OpenAL
        device = alcOpenDevice(NULL);

        if (!device) {
            printf("ALC Device error\n");
        }

        ctx = alcCreateContext(device, NULL);
        alcMakeContextCurrent(ctx);
        
        if (!ctx) {
            printf("ALC Context error\n");
        }
        
        alGenSources(1, &source);
        alGenBuffers(ALC_BUF_AMOUNT, bfr);
        
        for (int i = 0; i < ALC_BUF_AMOUNT; i++) {
            alBufferData(bfr[i], AL_FORMAT_STEREO16, sbuf.fin, SBUF_SIZE*2*2, SAMPLE_RATE);
        }
        
        alSourceQueueBuffers(source, ALC_BUF_AMOUNT, bfr);
    }
    
    ~CstrAudio() {
        alDeleteSources(1, &source);
        alDeleteBuffers(ALC_BUF_AMOUNT, bfr);
        ALCdevice *device = alcGetContextsDevice(ctx);
        alcMakeContextCurrent(NULL);
        alcCloseDevice(device);
    }
    
    void reset();
    void decodeStream();
    
    // Store
    void write(uw, uh);
    
    // Load
    uh read(uw);
    
    // DMA
    void executeDMA(CstrBus::castDMA *);
};

extern CstrAudio audio;
