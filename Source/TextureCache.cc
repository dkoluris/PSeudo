/* Base structure taken from FPSE open source emulator, and improved upon (Credits: BERO, LDChen) */

#include "Global.h"


#define COLOR_32BIT(a, b, c, r) \
    (((ub)(a)) << 24) | (((ub)(b)) << 16) | (((ub)(c)) << 8) | ((ub)(r))


CstrTextureCache tcache;

void CstrTextureCache::reset() {
    for (auto &tc : cache) {
        GLDeleteTextures(1, &tc.tex);
        tc = { 0 };
        
        createTexture(&tc.tex, 256, 256);
    }
    
    index = 0;
}

uw CstrTextureCache::pixel2texel(uh p) {
    return COLOR_32BIT(p ? 255 : 0, (p >> 10) << 3, (p >> 5) << 3, p << 3);
}

void CstrTextureCache::createTexture(GLuint *tex, int w, int h) {
    GLGenTextures(1, tex);
    GLBindTexture  (GL_TEXTURE_2D, *tex);
    GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    GLTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GLTexPhoto2D   (GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
}

void CstrTextureCache::fetchTexture(CstrDraw::TextureState info, uw clut) {
    uw uid = (clut << 16) | info.tp;
    
    for (auto &tc : cache) {
        if (tc.uid == uid && tc.w == info.w && tc.h == info.h) {
            if (tc.color != info.color || tc.update == true) {
                tc.uid = 0;
                tc.w   = 0;
                tc.h   = 0;
                break;
            }
            
            GLBindTexture(GL_TEXTURE_2D, tc.tex);
            return;
        }
    }
    
    // Basic info
    auto &tc  = cache[index];
    tc.uid    = uid;
    tc.w      = info.w;
    tc.h      = info.h;
    tc.color  = info.color;
    tc.update = false;
    
    // Reset
    uw vramPtr = (clut & 0x7fff) * 16;
    tex = { 0 };
    
    switch(tc.color) {
        case TEX_04BIT: // 16 color palette
            for (int i = 0; i < 16; i++) {
                tex.cc[i] = pixel2texel(vs.vram.ptr[vramPtr]);
                vramPtr++;
            }
            
            for (int h = 0; h < 256; h++) {
                for (int w = 0; w < (256 / 4); w++) {
                    const uh p = vs.vram.ptr[(tc.h + h) * FRAME_W + tc.w + w];
                    tex.bfr[h][w*4 + 0] = tex.cc[(p >> 0x0) & 15];
                    tex.bfr[h][w*4 + 1] = tex.cc[(p >> 0x4) & 15];
                    tex.bfr[h][w*4 + 2] = tex.cc[(p >> 0x8) & 15];
                    tex.bfr[h][w*4 + 3] = tex.cc[(p >> 0xc) & 15];
                }
            }
            break;
            
        case TEX_08BIT: // 256 color palette
            for (int i = 0; i < 256; i++) {
                tex.cc[i] = pixel2texel(vs.vram.ptr[vramPtr]);
                vramPtr++;
            }
            
            for (int h = 0; h < 256; h++) {
                for (int w = 0; w < (256 / 2); w++) {
                    const uh p = vs.vram.ptr[(tc.h + h) * FRAME_W + tc.w + w];
                    tex.bfr[h][w*2 + 0] = tex.cc[(p >> 0) & 255];
                    tex.bfr[h][w*2 + 1] = tex.cc[(p >> 8) & 255];
                }
            }
            break;
            
        case TEX_15BIT:   // No color palette
        case TEX_15BIT_2: // Seen on some rare cases
            for (int h = 0; h < 256; h++) {
                for (int w = 0; w < 256; w++) {
                    const uh p = vs.vram.ptr[(tc.h + h) * FRAME_W + tc.w + w];
                    tex.bfr[h][w] = pixel2texel(p);
                }
            }
            break;
            
        default:
            printx("/// PSeudo Texture Cache: %d", tc.color);
            break;
    }
    
    // Attach texture
    GLBindTexture  (GL_TEXTURE_2D, tc.tex);
    GLTexSubPhoto2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE, tex.bfr);
    
    if ((index + 1) >= TCACHE_MAX) {
        printx("/// PSeudo Texture Cache Full: %d", (index + 1));
    }
    
    index = (index + 1) & (TCACHE_MAX - 1);
}

void CstrTextureCache::invalidate(sh iX, sh iY, sh iW, sh iH) {
    //printf("(%d %d) (%d %d)\n", iX, iY, iW, iH);
    for (auto &tc : cache) {
        //if (((tc.w + 255) >= iX) && (tc.w <= (iW + iX)) &&
        //    ((tc.h + 255) >= iY) && (tc.h <= (iH + iY))) {
            
            tc.update = true;
        //}
    }
}
