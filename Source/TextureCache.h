class CstrTextureCache {
    enum {
        // Texture modes
        TEX_04BIT,
        TEX_08BIT,
        TEX_15BIT,
        TEX_15BIT_2,
        
        // Maximum texture cache
        TCACHE_MAX = 384
    };
    
    struct { // Mem position of texture and color lookup table
        GLuint tex;
        uw uid, w, h, color;
        bool update;
    } cache[TCACHE_MAX];
    
    struct {
        // Texture and color lookup table buffer
        uw bfr[256][256], cc[256];
    } tex;
    
    uh index;
    
public:
    ~CstrTextureCache() {
        for (auto &tc : cache) {
            GLDeleteTextures(1, &tc.tex);
        }
    }
    
    void reset();
    uw pixel2texel(uh);
    void createTexture(GLuint *, int, int);
    void fetchTexture(CstrDraw::TextureState, uw);
    void invalidate(sh, sh, sh, sh);
};

extern CstrTextureCache tcache;
