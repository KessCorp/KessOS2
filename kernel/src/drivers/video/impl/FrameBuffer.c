#include <drivers/video/FrameBuffer.h>
#include <stdint.h>

// 2022 Ian Moffett <ian@kesscoin.com>

void putChar(framebuffer_t* framebuffer, psf1_font_t* psf1_font, unsigned int color, char chr, unsigned int xOff, unsigned int yOff) {
    unsigned int* pixPtr = (unsigned int*)framebuffer->baseAddr;
    char* fontPtr = psf1_font->glyphBuffer + (chr * psf1_font->header->chsize);
    for (unsigned long y = yOff; y < yOff + 16; y++){
        for (unsigned long x = xOff; x < xOff+8; x++){
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0){
                    *(unsigned int*)(pixPtr + x + (y * framebuffer->ppsl)) = color;
                }

        }
        fontPtr++;
    }
}


static int get_pixel_idx(canvas_t canvas, int x, int y) {
  return x + y * canvas.lfb->ppsl;
}


void put_pix(canvas_t canvas, unsigned int x, unsigned int y, unsigned int color) {
  unsigned int* ptr = (unsigned int*)canvas.lfb->baseAddr;
  ptr[get_pixel_idx(canvas, x, y)] = color;
}


void draw_square(canvas_t canvas, unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int color) {
  for (int sx = x; sx < x + w; ++sx) {
    for (int sy = y; sy < y + h; ++sy) {
      put_pix(canvas, sx, sy, color);
    }
  }
}



void kwrite(canvas_t* canvas, const char* const STR, unsigned int color) {
    for (int i = 0; i < strlen(STR); ++i) {
        if (STR[i] == '\n') {
            canvas->x = canvas->prevX;
            canvas->y += 20;
            continue;
        }

        if (canvas->y >= canvas->lfb->height) {
            canvas->prevX += 300;
            canvas->y = 10;
        }

        putChar(canvas->lfb, canvas->font, color, STR[i], canvas->x, canvas->y);
        canvas->x += 8;
    }
}



void clearScreen(canvas_t* canvas, unsigned int color) {
    uint64_t bytesPerScanline = canvas->lfb->ppsl * 4;

    for (int vsl = 0; vsl < canvas->lfb->height; ++vsl) {
        uint64_t pixPtrBase = (uint64_t)canvas->lfb->baseAddr + (bytesPerScanline * vsl);
        for (uint32_t* pixelPtr = (uint32_t*)pixPtrBase; pixelPtr < (uint32_t*)(pixPtrBase + bytesPerScanline); ++pixelPtr) {
            *pixelPtr = color;
        }
    }

    canvas->prevX = 0;
    canvas->x = 0;
    canvas->y = 55;
}
