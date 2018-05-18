// Standard C libraries
#import <ctype.h>
#import <stdio.h>
#import <stdlib.h>
#import <string.h>
#import <OpenGL/gl.h>

// Apple macOS
#ifdef MAC_OS_X
    #import "../Platforms/macOS/Global.h"
#endif

// Data
typedef uint64_t ud; // uns doubleword
typedef uint32_t uw; // uns word
typedef uint16_t uh; // uns halfword
typedef uint8_t  ub; // uns bite

typedef int64_t sd; // doubleword
typedef int32_t sw; // word
typedef int16_t sh; // halfword
typedef int8_t  sb; // bite

// Defines
#define TYPEDEF         typedef

#define GLBlendFunc     glBlendFunc
#define GLClear         glClear
#define GLClearColor    glClearColor
#define GLColor4ub      glColor4ub
#define GLEnable        glEnable
#define GLEnd           glEnd
#define GLFlush         glFlush
#define GLID            glLoadIdentity
#define GLMatrixMode    glMatrixMode
#define GLOrtho         glOrtho
#define GLStart         glBegin
#define GLVertex2s      glVertex2s
#define GLViewport      glViewport

// User imports
#import "Bus.h"
#import "Graphics.h"
#import "Hardware.h"
#import "Mem.h"
#import "PSeudo.h"
#import "R3000A.h"
#import "Rootcounters.h"
