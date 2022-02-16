#include "framebuffer.h"
#include "terminal.h"

unsigned int width, height, pitch, isrgb;   /* dimensions and channel order */
unsigned char *fb;                         /* raw frame buffer address */

/**
 * Set screen resolution to 1024x768
 */
void fb_init()
{
    mbox[0] = 35*4;
    mbox[1] = MBOX_REQUEST;

    mbox[2] = 0x48003;  //set phy wh
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] = 1024;         //FrameBufferInfo.width
    mbox[6] = 768;          //FrameBufferInfo.height

    mbox[7] = 0x48004;  //set virt wh
    mbox[8] = 8;
    mbox[9] = 8;
    mbox[10] = 1024;        //FrameBufferInfo.virtual_width
    mbox[11] = 768;         //FrameBufferInfo.virtual_height

    mbox[12] = 0x48009; //set virt offset
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] = 0;           //FrameBufferInfo.x_offset
    mbox[16] = 0;           //FrameBufferInfo.y.offset

    mbox[17] = 0x48005; //set depth
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = 32;          //FrameBufferInfo.depth

    mbox[21] = 0x48006; //set pixel order
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 1;           //RGB, not BGR preferably

    mbox[25] = 0x40001; //get framebuffer, gets alignment on request
    mbox[26] = 8;
    mbox[27] = 8;
    mbox[28] = 4096;        //FrameBufferInfo.pointer
    mbox[29] = 0;           //FrameBufferInfo.size

    mbox[30] = 0x40008; //get pitch
    mbox[31] = 4;
    mbox[32] = 4;
    mbox[33] = 0;           //FrameBufferInfo.pitch

    mbox[34] = MBOX_TAG_LAST;

    //this might not return exactly what we asked for, could be
    //the closest supported resolution instead
    if(mbox_call(MBOX_CH_PROP) && mbox[20]==32 && mbox[28]!=0) {
        mbox[28]&=0x3FFFFFFF;   //convert GPU address to ARM address
        width=mbox[5];          //get actual physical width
        height=mbox[6];         //get actual physical height
        pitch=mbox[33];         //get number of bytes per line
        isrgb=mbox[24];         //get the actual channel order
        fb=(void*)((unsigned long)mbox[28]);
        uart_puts("Framebuffer initialized successfully\n");
    } else {
        uart_puts("Unable to set screen resolution to 1024x768x32\n");
    }
}

void drawPixel(int x, int y, unsigned char attr)
{
    int offs = (y * pitch) + (x * 4);
    *((unsigned int*)(fb + offs)) = vgapal[attr & 0x0f];
}


void drawRect(int x1, int y1, int x2, int y2, unsigned char attr, int fill)
{
    int y=y1;

    while (y <= y2) {
       int x=x1;
       while (x <= x2) {
	  if ((x == x1 || x == x2) || (y == y1 || y == y2)) drawPixel(x, y, attr);
	  else if (fill) drawPixel(x, y, (attr & 0xf0) >> 4);
          x++;
       }
       y++;
    }
}

void drawLine(int x1, int y1, int x2, int y2, unsigned char attr)  
{  
    int dx, dy, p, x, y;

    dx = x2-x1;
    dy = y2-y1;
    x = x1;
    y = y1;
    p = 2*dy-dx;

    while (x<x2) {
       if (p >= 0) {
          drawPixel(x,y,attr);
          y++;
          p = p+2*dy-2*dx;
       } else {
          drawPixel(x,y,attr);
          p = p+2*dy;
       }
       x++;
    }
}

void drawCircle(int x0, int y0, int radius, unsigned char attr, int fill)
{
    int x = radius;
    int y = 0;
    int err = 0;
 
    while (x >= y) {
	if (fill) {
	   drawLine(x0 - y, y0 + x, x0 + y, y0 + x, (attr & 0xf0) >> 4);
	   drawLine(x0 - x, y0 + y, x0 + x, y0 + y, (attr & 0xf0) >> 4);
	   drawLine(x0 - x, y0 - y, x0 + x, y0 - y, (attr & 0xf0) >> 4);
	   drawLine(x0 - y, y0 - x, x0 + y, y0 - x, (attr & 0xf0) >> 4);
	}
	drawPixel(x0 - y, y0 + x, attr);
	drawPixel(x0 + y, y0 + x, attr);
	drawPixel(x0 - x, y0 + y, attr);
        drawPixel(x0 + x, y0 + y, attr);
	drawPixel(x0 - x, y0 - y, attr);
	drawPixel(x0 + x, y0 - y, attr);
	drawPixel(x0 - y, y0 - x, attr);
	drawPixel(x0 + y, y0 - x, attr);

	if (err <= 0) {
	    y += 1;
	    err += 2*y + 1;
	}
 
	if (err > 0) {
	    x -= 1;
	    err -= 2*x + 1;
	}
    }
}

void drawChar(unsigned char ch, int x, int y, unsigned char attr)
{
    unsigned char *glyph = (unsigned char *)&font + (ch < FONT_NUMGLYPHS ? ch : 0) * FONT_BPG;

    for (int i=0;i<FONT_HEIGHT;i++) {
	for (int j=0;j<FONT_WIDTH;j++) {
	    unsigned char mask = 1 << j;
	    unsigned char col = (*glyph & mask) ? attr & 0x0f : (attr & 0xf0) >> 4;

	    drawPixel(x+j, y+i, col);
	}
	glyph += FONT_BPL;
    }
}

void drawString(int x, int y, char *s, unsigned char attr)
{
    while (*s) {
       if (*s == '\r') {
          x = 0;
       } else if(*s == '\n') {
          x = 0; y += FONT_HEIGHT;
       } else {
	  drawChar(*s, x, y, attr);
          x += FONT_WIDTH;
       }
       s++;
    }
}