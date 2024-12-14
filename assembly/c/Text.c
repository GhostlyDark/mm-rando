#include <z64.h>
#include "Sprite.h"
#include "Util.h"

const static int gTextMaxChars = 256;
const static int gTextBucketCount = 6;
const static int gTextBucketSize = 18;

typedef struct {
    uint32_t c    : 8;
    uint32_t left : 12;
    uint32_t top  : 12;
    ColorRGBA8 color;
} TextChar;

static TextChar* gTextBuf = NULL;
static TextChar* gTextEnd = NULL;

u8 TextFontWidth  = 8;
u8 TextFontHeight = 14;

void Text_Init(void) {
    gTextBuf = Util_HeapAlloc(gTextMaxChars * sizeof(TextChar));
    gTextEnd = gTextBuf;
}

void Text_SetSize(u8 width, u8 height) {
    TextFontWidth  = width;
    TextFontHeight = height;
}

void Text_PrintWithColor(const char* s, int left, int top, ColorRGBA8 color) {
    char c;
    while (c = *(s++)) {
        if (gTextEnd >= gTextBuf + gTextMaxChars)
            break;
        
        gTextEnd->c     = c;
        gTextEnd->left  = left;
        gTextEnd->top   = top;
        gTextEnd->color = color;
        gTextEnd++;
        left           += TextFontWidth;
    }
}

void Text_Print(const char* s, int left, int top) {
    ColorRGBA8 color = { 0xFF, 0xFF, 0xFF, 0xFF };
    Text_PrintWithColor(s, left, top, color);
}

void Text_Flush(DispBuf* db) {
    for (int i = 0; i < gTextBucketCount; i++) {
        Sprite_Load(db, &gSpriteFont, i * gTextBucketSize, gTextBucketSize);

        TextChar* pText = gTextBuf;
        while (pText < gTextEnd) {
            char c           = pText->c;
            int left         = pText->left;
            int top          = pText->top;
            ColorRGBA8 color = pText->color;
            pText++;

            int bucket = (c - 32) / gTextBucketSize;
            if (bucket != i) continue;

            // Apply text color.
            gDPSetPrimColor(db->p++, 0, 0, color.r, color.g, color.b, color.a);

            int tileIndex = (c - 32) % gTextBucketSize;
            Sprite_Draw(db, &gSpriteFont, tileIndex, left, top, TextFontWidth, TextFontHeight);
        }
    }

    gTextEnd = gTextBuf;
    Text_SetSize(8, 14);
}
