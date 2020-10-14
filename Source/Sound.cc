/* Base structure taken from SOPE open source emulator, and improved upon (Credits: SaD, Linuzappz) */

#include "Global.h"

CstrAudio audio;

void CstrAudio::init() {
    spuMemC = (ub *)spuMem;
    
    for (int i = 0; i < (MAXCHAN + 1); i++) {
        spuVoices[i].saddr = spuMemC;
        spuVoices[i].raddr = spuMemC;
        spuVoices[i].paddr = spuMemC;
    }
}

void CstrAudio::reset() {
    spuAddr = 0xffffffff;
    
    for (auto &item : spuVoices) {
        item = { 0 };
    }
}

void CstrAudio::voiceOn(uw data) {
    for (int n = 0; n < (MAXCHAN + 1); n++) {
        if (data & (1 << n) && spuVoices[n].saddr) {
            spuVoices[n].bNew = 1;
            spuVoices[n].endLoop = false;
        }
    }
}

void CstrAudio::voiceOff(uw data) {
    for (int n = 0; n < (MAXCHAN + 1); n++) {
    }
}

int CstrAudio::setVolume(sh data) {
    return ((data & 0x7fff) ^ 0x4000) - 0x4000;
}

void CstrAudio::write(uw addr, uh data) {
    switch(LOW_BITS(addr)) {
        case 0x1c00 ... 0x1d7e: // Channels
            {
                int ch = (addr >> 4) & 0x1f;

                switch(addr & 0xf) {
                    case 0x0: // Volume L
                        spuVoices[ch].volumeL = setVolume(data);
                        return;

                    case 0x2: // Volume R
                        spuVoices[ch].volumeR = setVolume(data);
                        return;

                    case 0x4: // Pitch
                        spuVoices[ch].freq = MIN(data, 0x3fff) << 4;
                        return;

                    case 0x6: // Sound Address
                        spuVoices[ch].saddr = spuMemC + (data << 3);
                        return;

                    case 0xe: // Return Address
                        spuVoices[ch].raddr = spuMemC + (data << 3);
                        spuVoices[ch].endLoop = true;
                        return;

                    /* unused */
                    case 0x8:
                    case 0xa:
                    case 0xc:
                        accessMem(mem.hwr, uh) = data;
                        return;
                }

                printx("/// PSeudo SPU write phase: 0x%x <- 0x%x", (addr & 0xf), data);
                return;
            }

        case 0x1d88: // Sound On 1
            voiceOn(data);
            return;

        case 0x1d8a: // Sound On 2
            voiceOn(data << 16);
            return;

        case 0x1d8c: // Sound Off 1
            voiceOff(data);
            return;

        case 0x1d8e: // Sound Off 2
            voiceOff(data << 16);
            return;

        case 0x1da6: // Transfer Address
            spuAddr = data << 3;
            return;

        case 0x1da8: // Data
            spuMem[spuAddr >> 1] = data;
            spuAddr += 2;
            spuAddr &= 0x7ffff;
            return;

        /* unused */
        case 0x1d80: // Volume L
        case 0x1d82: // Volume R
        case 0x1d84: // Reverb Volume L
        case 0x1d86: // Reverb Volume R
        case 0x1d90: // FM Mode On 1
        case 0x1d92: // FM Mode On 2
        case 0x1d94: // Noise Mode On 1
        case 0x1d96: // Noise Mode On 2
        case 0x1d98: // Reverb Mode On 1
        case 0x1d9a: // Reverb Mode On 2
        case 0x1d9c: // Mute 1
        case 0x1d9e: // Mute 2
        case 0x1daa: // Control
        case 0x1da2: // Reverb Address
        case 0x1dac: // ?
        case 0x1db0: // CD Volume L
        case 0x1db2: // CD Volume R
        case 0x1db4:
        case 0x1db6:
        case 0x1dc0 ... 0x1dfe: // Reverb
            accessMem(mem.hwr, uh) = data;
            return;
    }

    printx("/// PSeudo SPU write: 0x%x <- 0x%x", addr, data);
}

uh CstrAudio::read(uw addr) {
    switch(LOW_BITS(addr)) {
        case 0x1c00 ... 0x1d7e: // Channels
            {
                int ch = (addr >> 4) & 0x1f;

                switch(addr & 0xf) {
                    case 0xc: // Hack
                        if (spuVoices[ch].bNew) {
                            return 1;
                        }
                        return 0;
                        
                    case 0xe: // Madman
                        if (spuVoices[ch].raddr) {
                            return (spuVoices[ch].raddr - spuMemC) >> 3;
                        }
                        return 0;

                    /* unused */
                    case 0x0:
                    case 0x2:
                    case 0x4:
                    case 0x6:
                    case 0x8:
                    case 0xa:
                        return accessMem(mem.hwr, uh);
                }

                printx("/// PSeudo SPU read phase: 0x%x", (addr & 0xf));
                return 0;
            }

        case 0x1da6: // Transfer Address
            return spuAddr >> 3;

        /* unused */
        case 0x1d88: // Sound On 1
        case 0x1d8a: // Sound On 2
        case 0x1d8c: // Sound Off 1
        case 0x1d8e: // Sound Off 2
        case 0x1d94: // Noise Mode On 1
        case 0x1d96: // Noise Mode On 2
        case 0x1d98: // Reverb Mode On 1
        case 0x1d9a: // Reverb Mode On 2
        case 0x1d9c: // Voice Status 0 - 15
        case 0x1daa: // Control
        case 0x1dac: // ?
        case 0x1dae: // Status
        case 0x1db8:
        case 0x1dba:
        case 0x1e00:
        case 0x1e02:
        case 0x1e04:
        case 0x1e06:
        case 0x1e08:
        case 0x1e0a:
        case 0x1e0c:
        case 0x1e0e:
            return accessMem(mem.hwr, uh);
    }

    printx("/// PSeudo SPU read: 0x%x", addr);
    return 0;
}

#define audioSet(a, b) \
    rest = (*ch.paddr & a) << b; \
    if (rest & 0x8000) rest |= 0xffff0000; \
    rest = (rest >> shift) + ((s[0] * f[predict][0] + s[1] * f[predict][1] + 32) >> 6); \
    s[1] = s[0]; \
    s[0] = MIN(MAX(rest, SHRT_MIN), SHRT_MAX); \
    ch.bfr[i++] = s[0]

void CstrAudio::decodeStream() {
    while(!psx.suspended) {
        memset(&sbuf, 0, SPU_SAMPLE_SIZE);
        
        for (int n = 0; n < (MAXCHAN + 1); n++) {
            auto &ch = spuVoices[n];
            
            if (ch.bNew) {
                ch.paddr = ch.saddr;
                ch.s_1 = 0;
                ch.s_2 = 0;
                ch.iSBPos = 28;
                ch.bNew = 0;
                ch.on = true;
                ch.spos = 0x10000;
                ch.sample = 0;
            }
            
            if (ch.on == false) {
                continue;
            }
            
            for (int ns = 0; ns < SPU_SAMPLE_COUNT; ns++) {
                for (; ch.spos >= 0x10000; ch.spos -= 0x10000) {
                    if (ch.iSBPos == 28) {
                        if (ch.paddr == (ub *)-1) {
                            ch.on = false;
                            goto SPU_CHANNEL_END;
                        }
                        
                        ub shift   = *ch.paddr & 0xf;
                        ub predict = *ch.paddr++ >> 4;
                        ub op      = *ch.paddr++;
                        
                        sw s[2] = {
                            ch.s_1,
                            ch.s_2,
                        };
                        
                        ch.iSBPos = 0;

                        for (int i = 0, rest; i < 28; ch.paddr++) {
                            audioSet(0x0f, 0xc);
                            audioSet(0xf0, 0x8);
                        }

                        if ((op & 4) && (!ch.endLoop)) {
                            ch.raddr = ch.paddr - 16;
                        }
                        
                        if (op & 1) {
                            if (op != 3 || ch.raddr == NULL) {
                                ch.paddr = (ub *)-1;
                            }
                            else {
                                ch.paddr = ch.raddr;
                            }
                        }
                        
                        ch.s_1 = s[0];
                        ch.s_2 = s[1];
                    }
                    
                    ch.sample = ch.bfr[ch.iSBPos++] >> 2;
                }
                
                sbuf[(ns * 2) + 0] += (ch.sample * ch.volumeL) >> 14;
                sbuf[(ns * 2) + 1] += (ch.sample * ch.volumeR) >> 14;
                
                ch.spos += ch.freq;
            }
            
            SPU_CHANNEL_END: ;
        }
        
        output();
    }
}

void CstrAudio::output() {
    // OpenAL
    ALint processed;
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

    if (processed >= SPU_ALC_BUF_AMOUNT) {
        // We have to free buffers
        printf("/// PSeudo Inadequent ALC buffer size -> %d\n", processed);
    }

    while(--processed < 0) {
        stream();
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
    }

    ALuint buffer;
    alSourceUnqueueBuffers(source, 1, &buffer);
    alBufferData(buffer, AL_FORMAT_STEREO16, sbuf, SPU_SAMPLE_SIZE, SPU_SAMPLE_RATE);
    alSourceQueueBuffers(source, 1, &buffer);
    stream();
}

void CstrAudio::stream() {
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);

    if (state != AL_PLAYING) {
        alSourceStream(source);
    }
}

void CstrAudio::executeDMA(CstrBus::castDMA *dma) {
    uh *p   = (uh *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)];
    uw size = (dma->bcr >> 16) * (dma->bcr & 0xffff) * 2;

    switch(dma->chcr) {
        case 0x01000201: // Write
            for (uw i = 0; i < size; i++) {
                spuMem[spuAddr >> 1] = *p++;
                spuAddr += 2;
                spuAddr &= 0x7ffff;
            }
            return;

        case 0x01000200: // Read
            for (uw i = 0; i < size; i++) {
                *p++ = spuMem[spuAddr >> 1];
                spuAddr += 2;
                spuAddr &= 0x7ffff;
            }
            return;
    }

    printx("/// PSeudo SPU DMA: 0x%x", dma->chcr);
}

//void CstrAudio::reset() {
//    // Mem
//    memset(&spuMem, 0, sizeof(spuMem));
//    spuAddr = 0xffffffff;
//
//    // Channels reset
//    for (auto &item : spuVoices) {
//        item = { 0 };
//    }
//}
//
//sh CstrAudio::setVolume(sh data) { // Sweep mode
//    return ((data & 0x7fff) ^ 0x4000) - 0x4000;
//}
//
//#define audioSet(a, b) \
//    rest = (*p & a) << b; \
//    if (rest & 0x8000) rest |= 0xffff0000; \
//    rest = (rest >> shift) + ((s[0] * f[predict][0] + s[1] * f[predict][1] + 32) >> 6); \
//    s[1] = s[0]; \
//    s[0] = MIN(MAX(rest, SHRT_MIN), SHRT_MAX); \
//    chn.bfr[chn.size++] = s[0]
//
//void CstrAudio::voiceOn(uw data) {
//    for (int n = 0; n < SPU_CHANNELS; n++) {
//        if (data & (1 << n)) {
//            auto &chn = spuVoices[n];
//            chn.count = 0;
//            chn.pos   = 0;
//            chn.raddr = 0;
//            chn.size  = 0;
//
//            { // Depack VAG
//                ub *p = (ub *)&spuMem[chn.saddr >> 1];
//
//                static sw s[2] = {
//                    0,
//                    0,
//                };
//
//                for(; chn.size < SPU_CHANNEL_BUF_SIZE - 28;) {
//                    ub shift   = *p & 0xf;
//                    ub predict = *p++ >> 4;
//                    ub op      = *p++;
//
//                    for (int i = chn.size, rest; chn.size < i + 28; p++) {
//                        audioSet(0x0f, 0xc);
//                        audioSet(0xf0, 0x8);
//                    }
//
//                    switch(op) {
//                        case 3: // End
//                        case 7:
//                            redirect NEXT_CHANNEL;
//
//                        case 6: // Repeat
//                            chn.raddr = chn.size;
//                    }
//                }
//
//                printf("/// PSeudo SPU Channel size overflow\n");
//            }
//        }
//        NEXT_CHANNEL: ;
//    }
//}
//
//void CstrAudio::voiceOff(uw data) {
//    for (int n = 0; n < SPU_CHANNELS; n++) {
//        if (data & (1 << n)) {
//        }
//    }
//}
//
//void CstrAudio::stream() {
//    ALint state;
//    alGetSourcei(source, AL_SOURCE_STATE, &state);
//
//    if (state != AL_PLAYING) {
//        alSourceStream(source);
//    }
//}
//
//void CstrAudio::decodeStream() {
//    while(!psx.suspended) {
//        sw temp[SPU_SAMPLE_SIZE] = { 0 };
//
//        // Clear
//        memset(&sbuf, 0, sizeof(sbuf));
//
//        for (auto &chn : spuVoices) {
//            // Channel on?
//            if (chn.size < 1) {
//                continue;
//            }
//
//            for (int i = 0; i < SPU_SAMPLE_SIZE; i += 2) {
//                chn.count += chn.freq;
//
//                if (chn.count >= SPU_SAMPLE_RATE) {
//                    chn.pos   += chn.count / SPU_SAMPLE_RATE;
//                    chn.count %= SPU_SAMPLE_RATE;
//                }
//
//                // Mix Channel Samples
//                temp[i + 0] += (+chn.bfr[chn.pos] * chn.volumeL) / SPU_MAX_VOLUME;
//                temp[i + 1] += (-chn.bfr[chn.pos] * chn.volumeR) / SPU_MAX_VOLUME;
//
//                // End of Sample
//                if (chn.pos >= chn.size) {
//                    if (chn.raddr > 0) { // Repeat?
//                        chn.pos   = chn.raddr;
//                        chn.count = 0;
//                        continue;
//                    }
//
//                    chn.size = 0;
//                    break;
//                }
//            }
//        }
//
//        // Volume Mix
//        for (int i = 0; i < SPU_SAMPLE_SIZE; i += 2) {
//            sbuf[i + 0] = MIN(MAX(+(temp[i + 0] / 2), SHRT_MIN), SHRT_MAX);
//            sbuf[i + 1] = MIN(MAX(-(temp[i + 1] / 2), SHRT_MIN), SHRT_MAX);
//        }
//
//        // OpenAL
//        ALint processed;
//        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
//
//        if (processed >= SPU_ALC_BUF_AMOUNT) {
//            // We have to free buffers
//            printf("/// PSeudo Inadequent ALC buffer size -> %d\n", processed);
//        }
//
//        while(--processed < 0) {
//            stream();
//            alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
//        }
//
//        ALuint buffer;
//        alSourceUnqueueBuffers(source, 1, &buffer);
//        alBufferData(buffer, AL_FORMAT_STEREO16, sbuf, SPU_SAMPLE_SIZE * 2, SPU_SAMPLE_RATE);
//        alSourceQueueBuffers(source, 1, &buffer);
//        stream();
//    }
//}
//
//#define spuChannel(addr) \
//    (addr >> 4) & 0x1f
//
//void CstrAudio::write(uw addr, uh data) {
//    switch(LOW_BITS(addr)) {
//        case 0x1c00 ... 0x1d7e: // Channels
//            {
//                ub n = spuChannel(addr);
//
//                switch(addr & 0xf) {
//                    case 0x0: // Volume L
//                        spuVoices[n].volumeL = setVolume(data);
//                        return;
//
//                    case 0x2: // Volume R
//                        spuVoices[n].volumeR = setVolume(data);
//                        return;
//
//                    case 0x4: // Pitch
//                        spuVoices[n].freq = MAX((data * SPU_SAMPLE_RATE) / 4096, 1);
//                        return;
//
//                    case 0x6: // Sound Address
//                        spuVoices[n].saddr = data << 3;
//                        return;
//
//                    case 0xe: // Return Address
//                        spuVoices[n].raddr = data << 3;
//                        return;
//
//                    /* unused */
//                    case 0x8:
//                    case 0xa:
//                    case 0xc:
//                        accessMem(mem.hwr, uh) = data;
//                        return;
//                }
//
//                printx("/// PSeudo SPU write phase: 0x%x <- 0x%x", (addr & 0xf), data);
//                return;
//            }
//
//        case 0x1d88: // Sound On 1
//            voiceOn(data);
//            return;
//
//        case 0x1d8a: // Sound On 2
//            voiceOn(data << 16);
//            return;
//
//        case 0x1d8c: // Sound Off 1
//            voiceOff(data);
//            return;
//
//        case 0x1d8e: // Sound Off 2
//            voiceOff(data << 16);
//            return;
//
//        case 0x1da6: // Transfer Address
//            spuAddr = data << 3;
//            return;
//
//        case 0x1da8: // Data
//            spuMem[spuAddr >> 1] = data;
//            spuAddr += 2;
//            spuAddr &= 0x7ffff;
//            return;
//
//        /* unused */
//        case 0x1d80: // Volume L
//        case 0x1d82: // Volume R
//        case 0x1d84: // Reverb Volume L
//        case 0x1d86: // Reverb Volume R
//        case 0x1d90: // FM Mode On 1
//        case 0x1d92: // FM Mode On 2
//        case 0x1d94: // Noise Mode On 1
//        case 0x1d96: // Noise Mode On 2
//        case 0x1d98: // Reverb Mode On 1
//        case 0x1d9a: // Reverb Mode On 2
//        case 0x1d9c: // Mute 1
//        case 0x1d9e: // Mute 2
//        case 0x1daa: // Control
//        case 0x1da2: // Reverb Address
//        case 0x1dac: // ?
//        case 0x1db0: // CD Volume L
//        case 0x1db2: // CD Volume R
//        case 0x1db4:
//        case 0x1db6:
//        case 0x1dc0 ... 0x1dfe: // Reverb
//            accessMem(mem.hwr, uh) = data;
//            return;
//    }
//
//    printx("/// PSeudo SPU write: 0x%x <- 0x%x", addr, data);
//}
//
//uh CstrAudio::read(uw addr) {
//    switch(LOW_BITS(addr)) {
//        case 0x1c00 ... 0x1d7e: // Channels
//            {
//                ub n = spuChannel(addr);
//
//                switch(addr & 0xf) {
//                    case 0xc: // Hack
//                        if (spuVoices[n].size) {
//                            return 1;
//                        }
//                        return 0;
//
//                    /* unused */
//                    case 0x0:
//                    case 0x2:
//                    case 0x4:
//                    case 0x6:
//                    case 0x8:
//                    case 0xa:
//                    case 0xe: // madman
//                        return accessMem(mem.hwr, uh);
//                }
//
//                printx("/// PSeudo SPU read phase: 0x%x", (addr & 0xf));
//                return 0;
//            }
//
//        case 0x1da6: // Transfer Address
//            return spuAddr >> 3;
//
//        /* unused */
//        case 0x1d88: // Sound On 1
//        case 0x1d8a: // Sound On 2
//        case 0x1d8c: // Sound Off 1
//        case 0x1d8e: // Sound Off 2
//        case 0x1d94: // Noise Mode On 1
//        case 0x1d96: // Noise Mode On 2
//        case 0x1d98: // Reverb Mode On 1
//        case 0x1d9a: // Reverb Mode On 2
//        case 0x1d9c: // Voice Status 0 - 15
//        case 0x1daa: // Control
//        case 0x1dac: // ?
//        case 0x1dae: // Status
//        case 0x1db8:
//        case 0x1dba:
//        case 0x1e00:
//        case 0x1e02:
//        case 0x1e04:
//        case 0x1e06:
//        case 0x1e08:
//        case 0x1e0a:
//        case 0x1e0c:
//        case 0x1e0e:
//            return accessMem(mem.hwr, uh);
//    }
//
//    printx("/// PSeudo SPU read: 0x%x", addr);
//    return 0;
//}
//
//void CstrAudio::executeDMA(CstrBus::castDMA *dma) {
//    uh *p   = (uh *)&mem.ram.ptr[dma->madr & (mem.ram.size - 1)];
//    uw size = (dma->bcr >> 16) * (dma->bcr & 0xffff) * 2;
//
//    switch(dma->chcr) {
//        case 0x01000201: // Write
//            for (uw i = 0; i < size; i++) {
//                spuMem[spuAddr >> 1] = *p++;
//                spuAddr += 2;
//                spuAddr &= 0x7ffff;
//            }
//            return;
//
//        case 0x01000200: // Read
//            for (uw i = 0; i < size; i++) {
//                *p++ = spuMem[spuAddr >> 1];
//                spuAddr += 2;
//                spuAddr &= 0x7ffff;
//            }
//            return;
//    }
//
//    printx("/// PSeudo SPU DMA: 0x%x", dma->chcr);
//}
