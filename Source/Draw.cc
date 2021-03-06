/* Base structure and authentic idea PSeudo (Credits: Dennis Koluris) */

#include "Global.h"


CstrDraw draw;

void CstrDraw::init(sh w, sh h, int multiplier) {
    window.h = w;
    window.v = h;
    window.multiplier = multiplier;
    
    // OpenGL
    GLViewport(0, 0, window.h * 2, window.v * 2);
    GLClearColor(0.1, 0.1, 0.1, 0);
    GLClear(GL_COLOR_BUFFER_BIT);
    
    if (window.multiplier > 1) { // Crap
        GLLineWidth(window.multiplier);
    }
    
    // Textures
    GLMatrixMode(GL_TEXTURE);
    GLID();
    GLScalef(1.0 / 256, 1.0 / 256, 1.0);
    GLTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    GLTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 2);
    
    tcache.createTexture(&fb24tex, FRAME_W, FRAME_H); // 24-bit texture
    tcache.createTexture(&fb16tex, FRAME_W, FRAME_H); // 16-bit texture
    
    //tcache.createTexture(&display, 320, 240); // 16-bit texture
}

void CstrDraw::reset() {
    res    = { 0 };
    offset = { 0 };
    
    opaqueClipState(true);
    
    // Redraw
    resize(window.h, window.v);
    swapBuffers();
}

void CstrDraw::swapBuffers() {
    GLMatrixMode(GL_PROJECTION);
    GLID();
    //int sx = ((int)(vs.rx_u - vs.rx_l) * res.h) / 2560;
    GLOrtho(vs.dispOffsetX * 2, res.h + vs.dispOffsetX * 2, res.v + vs.dispOffsetY, vs.dispOffsetY, 1, -1);
//    GLFlush();
    glFinish();
}

void CstrDraw::resize(sh w, sh h) {
#if 0
    w = FRAME_W;
    h = FRAME_H;
#endif
    
    // Not current
    //if (res.h != w || res.v != h) {
        //keepAspectRatio(w, h, window.multiplier);
        //GLMatrixMode(GL_PROJECTION);
        //GLID();
        
#ifdef APPLE_MACOS
        //GLOrtho (0, w, h + vs.verticalStart, vs.verticalStart, 1, -1);
        //GLOrtho (0, w, h, 0, 1, -1);
        //GLClear(GL_COLOR_BUFFER_BIT);
#elif  APPLE_IOS
        GLOrthof(0, w, h, 0, 1, -1);
#endif
        
        // Make current
        res.h = w;
        res.v = h;
    //}
}

void CstrDraw::keepAspectRatio(sh w, sh h, int multiplier) {
    sh iw = w * multiplier;
    sh ih = h * multiplier;
    
    sh ww = window.h * multiplier;
    sh wh = window.v * multiplier;
    
    GLViewport((ww - iw) / 2, (wh - ih) / 2, iw, ih);
}

void CstrDraw::opaqueClipState(bool enable) {
    if (enable) {
        GLEnable(GL_BLEND);
        GLEnable(GL_CLIP_PLANE0);
        GLEnable(GL_CLIP_PLANE1);
        GLEnable(GL_CLIP_PLANE2);
        GLEnable(GL_CLIP_PLANE3);
    }
    else {
        GLDisable(GL_BLEND);
        GLDisable(GL_CLIP_PLANE0);
        GLDisable(GL_CLIP_PLANE1);
        GLDisable(GL_CLIP_PLANE2);
        GLDisable(GL_CLIP_PLANE3);
    }
}

ub CstrDraw::opaqueFunc(ub enabled) {
    if (enabled) {
        GLBlendFunc(bit[texState.abr].src, bit[texState.abr].dst);
        return bit[texState.abr].trans;
    }
    else {
        GLBlendFunc(bit[0].src, bit[0].dst);
        return COLOR_MAX;
    }
}

void CstrDraw::updateTextureState(uw data) {
    texState.tp    = (data) & 0xffffff;
    texState.w     = (data << 6) & 960;
    texState.h     = (data << 4) & 256;
    texState.color = (data >> 7) & 3;
    texState.abr   = (data >> 5) & 3;
    
    GLBlendFunc(bit[texState.abr].src, bit[texState.abr].dst);
}

void CstrDraw::setDrawArea(int plane, uw data) {
    double e1[] = { 1, 0, 0, (double)((data) & 0x3ff) };
    double e2[] = { 0, 1, 0, (double)((data >> 10) & 0x1ff) };
    
    if (plane) {
        e1[0] = -e1[0]; e1[3]++; // ?
        e2[1] = -e2[1]; e2[3]++; // ?
    }
    else {
        e1[3] = -e1[3];
        e2[3] = -e2[3];
    }
    
#if defined(APPLE_MACOS) || defined(_WIN32)
    GLClipPlane (GL_CLIP_PLANE0 + (plane + 0), e1);
    GLClipPlane (GL_CLIP_PLANE0 + (plane + 1), e2);
#elif APPLE_IOS
    GLClipPlanef(GL_CLIP_PLANE0 + (plane + 0), (float *)e1);
    GLClipPlanef(GL_CLIP_PLANE0 + (plane + 1), (float *)e2);
#endif
}

template <class T>
void parse(T *components, uw *packets, int points, int step) {
    for (uw i = 0, *p = packets; i < points; i++, p += step) {
        components[i] = *(T *)&p;
    }
}

void CstrDraw::primitive(uw addr, uw *packets) {
    switch((addr >> 5) & 7) {
        case GPU_TYPE_CMD:
            switch(addr) {
                case 0x01: // Reset
                    vs.write(0x1f801814, 0x01000000);
                    return;
                    
                case 0x02: // Rect
                    {
                        // Basic packet components
                        Color  *hue[1];
                        Coords *vx [1];
                        Coords *sz [1];
                        
                        parse(hue, &packets[0], 1, 0);
                        parse( vx, &packets[1], 1, 0);
                        parse( sz, &packets[2], 1, 0);
                        
                        opaqueClipState(false);
                        GLColor4ub(hue[0]->r, hue[0]->c, hue[0]->b, COLOR_MAX);
                        
#if defined(APPLE_MACOS) || defined(_WIN32)
                        GLRecti(NORMALIZE_PT(vx[0]->w),
                                NORMALIZE_PT(vx[0]->h),
                                NORMALIZE_PT(vx[0]->w) + sz[0]->w,
                                NORMALIZE_PT(vx[0]->h) + sz[0]->h);
#elif APPLE_IOS
                        // TODO
#endif
                        opaqueClipState(true);
                    }
                    return;
            }
            
            printx("/// PSeudo GPU_TYPE_CMD: 0x%x", addr);
            return;
            
        case GPU_TYPE_POLYGON:
            {
                POLY *setup = (POLY *)&addr;
                
                // Options
                int step   = setup->textured   ? 2 : 1; // The offset to fetch specific data from packets
                int points = setup->four_point ? 4 : 3;
                
                // Basic packet components
                Color   *hue[points];
                Coords  *vx [points];
                Texture *tex[points];
                
                if (setup->shaded) {
                    // Gouraud
                    parse(hue, &packets[0], points, step + 1);
                    parse( vx, &packets[1], points, step + 1);
                    parse(tex, &packets[2], points, 3);
                }
                else {
                    // Flat
                    parse(hue, &packets[0], points, 0);
                    parse( vx, &packets[1], points, step);
                    parse(tex, &packets[2], points, step);
                }
                
                if (setup->textured) {
                    GLEnable(GL_TEXTURE_2D);
                    updateTextureState(tex[1]->tp);
                    tcache.fetchTexture(texState, tex[0]->tp);
                }
                
                const ub b = opaqueFunc(setup->semi_trans);
                
#if defined(APPLE_MACOS) || defined(_WIN32)
                GLStart(GL_TRIANGLE_STRIP);
                for (int i = 0; i < points; i++) {
                    if (setup->textured && setup->raw_tex) {
                        hue[i]->r = COLOR_HALF;
                        hue[i]->c = COLOR_HALF;
                        hue[i]->b = COLOR_HALF;
                    }
                    
                    // Cast offset
                    vx[i]->w = NORMALIZE_PT(vx[i]->w) + offset.h;
                    vx[i]->h = NORMALIZE_PT(vx[i]->h) + offset.v;
                    
                    GLColor4ub  (hue[i]->r, hue[i]->c, hue[i]->b, b);
                    GLTexCoord2s(tex[i]->u, tex[i]->v);
                    GLVertex2s  (vx [i]->w, vx [i]->h);
                }
                GLEnd();
#elif APPLE_IOS
                // TODO
#endif
                GLDisable(GL_TEXTURE_2D);
            }
            return;
            
        case GPU_TYPE_LINE:
            {
                LINE *setup = (LINE *)&addr;
                
                // Options
                int points = setup->multiline ? 256 : 2; // eurasia-001, fuzzion, mups-016, pdx-030, pdx-074, pop-n-pop
                
                // Basic packet components
                Color  *hue[points];
                Coords *vx [points];
                
                if (setup->shaded) {
                    // Gouraud
                    parse(hue, &packets[0], points, 2);
                    parse( vx, &packets[1], points, 2);
                }
                else {
                    // Flat
                    parse(hue, &packets[0], points, 0);
                    parse( vx, &packets[1], points, 1);
                }
                
                const ub b = opaqueFunc(setup->semi_trans);
                
#if defined(APPLE_MACOS) || defined(_WIN32)
                GLStart(GL_LINE_STRIP);
                for (int i = 0; i < points; i++) {
                    if (setup->multiline) { // Special case
                        if (*(uw *)vx[i] == LINE_TERM_CODE || *(uw *)hue[i] == LINE_TERM_CODE) {
                            break;
                        }
                    }
                    
                    // Cast offset
                    vx[i]->w = NORMALIZE_PT(vx[i]->w) + offset.h;
                    vx[i]->h = NORMALIZE_PT(vx[i]->h) + offset.v;
                    
                    GLColor4ub(hue[i]->r, hue[i]->c, hue[i]->b, b);
                    GLVertex2s(vx [i]->w, vx [i]->h);
                }
                GLEnd();
#elif APPLE_IOS
                // TODO
#endif
            }
            return;
            
        case GPU_TYPE_SPRITE:
            {
                SPRT *setup = (SPRT *)&addr;
                
                // Basic packet components
                Color   *hue[1];
                Coords  *vx [1];
                Texture *tex[1];
                Coords  *sz [1];
                
                parse(hue, &packets[0], 1, 0);
                parse( vx, &packets[1], 1, 0);
                parse(tex, &packets[2], 1, 0);
                parse( sz, &packets[setup->textured ? 3 : 2], 1, 0);
                
                // Square size
                int size = spriteSize[setup->size];
                
                struct {
                    sh txw, txh, vxw, vxh;
                } pos;
                
                if (size) { // Fixed size
                    pos.vxw = size;
                    pos.vxh = size;
                    pos.txw = size;
                    pos.txh = size;
                }
                else { // Freeform & Texture Window
                    pos.vxw = sz[0]->w;
                    pos.vxh = sz[0]->h;
                    
                    //tex[0]->u += texWindow.startX;
                    //tex[0]->v += texWindow.startY;
                    
                    pos.txw = pos.vxw;//MIN(texWindow.endX, pos.vxw);
                    pos.txh = pos.vxh;//MIN(texWindow.endY, pos.vxh);
                }
                
                if (setup->textured) {
                    GLEnable(GL_TEXTURE_2D);
                    tcache.fetchTexture(texState, tex[0]->tp);
                    
                    if (setup->raw_tex) { // This is not in specs?
                        hue[0]->r = COLOR_HALF;
                        hue[0]->c = COLOR_HALF;
                        hue[0]->b = COLOR_HALF;
                    }
                }
                
                const ub b = opaqueFunc(setup->semi_trans);
                GLColor4ub(hue[0]->r, hue[0]->c, hue[0]->b, b);
                
#if defined(APPLE_MACOS) || defined(_WIN32)
                GLStart(GL_TRIANGLE_STRIP);
                    // Cast offset
                    vx[0]->w = NORMALIZE_PT(vx[0]->w) + offset.h;
                    vx[0]->h = NORMALIZE_PT(vx[0]->h) + offset.v;
                    
                    GLTexCoord2s(tex[0]->u,           tex[0]->v);
                    GLVertex2s  (vx [0]->w,           vx [0]->h);
                    GLTexCoord2s(tex[0]->u + pos.txw, tex[0]->v);
                    GLVertex2s  (vx [0]->w + pos.vxw, vx [0]->h);
                    GLTexCoord2s(tex[0]->u,           tex[0]->v + pos.txh);
                    GLVertex2s  (vx [0]->w,           vx [0]->h + pos.vxh);
                    GLTexCoord2s(tex[0]->u + pos.txw, tex[0]->v + pos.txh);
                    GLVertex2s  (vx [0]->w + pos.vxw, vx [0]->h + pos.vxh);
                GLEnd();
#elif APPLE_IOS
                // TODO
#endif
                GLDisable(GL_TEXTURE_2D);
            }
            return;
            
        case GPU_TYPE_IMG_MOVE:
            vs.photoMoveWithin(packets);
            return;
            
        case GPU_TYPE_IMG_SEND:
            vs.photoSendTo(packets);
            return;
            
        case GPU_TYPE_IMG_COPY:
            vs.photoReadFrom(packets);
            return;
            
        case GPU_TYPE_ENV:
            switch(addr) {
                case 0xe1: // Texture P.
                    updateTextureState(packets[0]);
                    return;
                    
                case 0xe2: // Texture Window
                    texWindow.startX = ((packets[0] >> 10) & 0x1f) << 3;
                    texWindow.startY = ((packets[0] >> 15) & 0x1f) << 3;
                    texWindow.  endX = 256 - (((packets[0] >> 0) & 0x1f) << 3);
                    texWindow.  endY = 256 - (((packets[0] >> 5) & 0x1f) << 3);
                    vs.info[vs.GPU_INFO_TEX_WINDOW] = packets[0] & 0xfffff;
                    return;
                    
                case 0xe3: // Draw Area Start
                    setDrawArea(0, packets[0]);
                    vs.info[vs.GPU_INFO_DRAW_AREA_START] = packets[0] & 0x3fffff;
                    return;
                    
                case 0xe4: // Draw Area End
                    setDrawArea(2, packets[0]);
                    vs.info[vs.GPU_INFO_DRAW_AREA_END] = packets[0] & 0x3fffff;
                    return;
                    
                case 0xe5: // Draw Offset
                    offset.h = ((sw)packets[0] << 21) >> 21;
                    offset.v = ((sw)packets[0] << 10) >> 21;
                    vs.info[vs.GPU_INFO_DRAW_OFFSET] = packets[0] & 0x7fffff;
                    return;
                    
                case 0xe6: // TODO: STP
                    //printf("/// PSeudo GPU STP: 0x%x\n", packets[0]);
                    return;
            }
            
            printx("/// PSeudo GPU_TYPE_ENV: 0x%x", addr);
            return;
    }
    
    printx("/// PSeudo GPU Primitive: 0x%x / %d", packets[0], ((addr >> 5) & 7));
}

void CstrDraw::outputVRAM(uw *raw, sh X, sh Y, sh W, sh H, bool video24Bit) {
    // Disable state
    opaqueClipState(false);
    
    // Accommodate VRAM's texture size
    GLMatrixMode(GL_TEXTURE);
    GLID();
    GLScalef(1.0 / FRAME_W, 1.0 / FRAME_H, 1.0);
    
    GLEnable(GL_TEXTURE_2D);
    GLColor4ub(COLOR_HALF, COLOR_HALF, COLOR_HALF, COLOR_MAX);
    
    if (video24Bit) {
        X = (X * 2) / 3;
        W = (W * 2) / 3;
        GLBindTexture  (GL_TEXTURE_2D, fb24tex);
        GLTexSubPhoto2D(GL_TEXTURE_2D, 0, 0, 0, W, H, GL_RGB , GL_UNSIGNED_BYTE, raw);
    }
    else {
        GLBindTexture  (GL_TEXTURE_2D, fb16tex);
        GLTexSubPhoto2D(GL_TEXTURE_2D, 0, 0, 0, W, H, GL_RGBA, GL_UNSIGNED_BYTE, raw);
    }
    
#if defined(APPLE_MACOS) || defined(_WIN32)
    GLStart(GL_TRIANGLE_STRIP);
        GLTexCoord2s(0, 0); GLVertex2s(X,     Y);
        GLTexCoord2s(W, 0); GLVertex2s(X + W, Y);
        GLTexCoord2s(0, H); GLVertex2s(X,     Y + H);
        GLTexCoord2s(W, H); GLVertex2s(X + W, Y + H);
    GLEnd();
#elif APPLE_IOS
    // TODO
#endif
    
    GLDisable(GL_TEXTURE_2D);
    
    // Revert to normal texture mode
    GLID();
    GLScalef(1.0 / 256, 1.0 / 256, 1.0);
    
    // Enable state
    opaqueClipState(true);
}

void CstrDraw::updateVRAMView() {
    // OpenGL
    GLViewport(0, 0, FRAME_W * 2, FRAME_H * 2);
    GLClearColor(0.0, 0.0, 0.0, 0);
    GLClear(GL_COLOR_BUFFER_BIT);
    
    // Textures
    GLMatrixMode(GL_TEXTURE);
    GLID();
    GLScalef(1.0 / FRAME_W, 1.0 / FRAME_H, 1.0);
    GLTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    GLTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 2);
    
    GLEnable(GL_TEXTURE_2D);
    tcache.createTexture(&fbVram, FRAME_W, FRAME_H);
    
    // Ortho 2D
    GLMatrixMode(GL_PROJECTION);
    GLID();
    GLOrtho(0, FRAME_W, FRAME_H, 0, 1, -1);
    
    while(!psx.suspended) {
        GLColor4ub(COLOR_HALF, COLOR_HALF, COLOR_HALF, COLOR_MAX);
        
        GLBindTexture  (GL_TEXTURE_2D, fbVram);
        GLTexSubPhoto2D(GL_TEXTURE_2D, 0, 0, 0, FRAME_W, FRAME_H, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, (GLvoid *)&vs.vram.ptr[0]);
        
#if defined(APPLE_MACOS) || defined(_WIN32)
        GLStart(GL_TRIANGLE_STRIP);
            GLTexCoord2s(0,             0); GLVertex2s(0,       0);
            GLTexCoord2s(FRAME_W,       0); GLVertex2s(FRAME_W, 0);
            GLTexCoord2s(0,       FRAME_H); GLVertex2s(0,       FRAME_H);
            GLTexCoord2s(FRAME_W, FRAME_H); GLVertex2s(FRAME_W, FRAME_H);
        GLEnd();
#elif APPLE_IOS
        // TODO
#endif
        
        GLFlush();
    }
}
