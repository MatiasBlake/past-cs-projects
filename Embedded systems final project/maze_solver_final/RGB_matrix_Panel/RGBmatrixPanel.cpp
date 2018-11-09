#define DEBUG
#undef DEBUG
#include "RGBmatrixPanel.h"
#include "gamma.h"

#define nPlanes 4

// The fact that the display driver interrupt stuff is tied to the
// singular Timer1 doesn't really take well to object orientation with
// multiple RGBmatrixPanel instances.  The solution at present is to
// allow instances, but only one is active at any given time, via its
// begin() method.  The implementation is still incomplete in parts;
// the prior active panel really should be gracefully disabled, and a
// stop() method should perhaps be added...assuming multiple instances
// are even an actual need.
//static RGBmatrixPanel *activePanel = NULL;

// Code common to both the 16x32 and 32x32 constructors:
void RGBmatrixPanel::init(uint8_t rows, bool dbuf)
{
    nRows = rows; // Number of multiplexed rows; actual height is 2X this
    // Allocate and initialize matrix buffer:
    int buffsize  = 32*nRows*3, // x3 = 3 bytes holds 4 planes "packed"
        allocsize = (dbuf == true) ? (buffsize * 2) : buffsize;
    if(NULL == (matrixbuff[0] = (uint8_t *)malloc(allocsize))) return;
    memset(matrixbuff[0], 0, allocsize);
    // If not double-buffered, both buffers then point to the same address:
    matrixbuff[1] = (dbuf == true) ? &matrixbuff[0][buffsize] : matrixbuff[0];

    plane     = nPlanes - 1;
    row       = nRows   - 1;
    swapflag  = false;
    backindex = 0;     // Array index of back buffer
}

// Constructor for 16x32 panel:
RGBmatrixPanel::RGBmatrixPanel(PinName r1,PinName g1,PinName b1,PinName r2,PinName g2,PinName b2,PinName a,PinName b, PinName c, PinName sclk, PinName latch, PinName oe, bool dbuf)
    :Adafruit_GFX(32, 16),
     _dataBus(r1,g1,b1,r2,g2,b2),
     _rowBus(a,b,c),
     _d(NC),
     _sclk(sclk),
     _latch(latch),
     _oe(oe)
{
    init(8, dbuf);
}

// Constructor for 32x32 panel:
RGBmatrixPanel::RGBmatrixPanel(PinName r1,PinName r2,PinName g1,PinName g2,PinName b1,PinName b2,PinName a,PinName b,PinName c,PinName d,PinName sclk,PinName latch,PinName oe,bool dbuf)
    :Adafruit_GFX(32, 32),
     _dataBus(r1,g1,b1,r2,g2,b2),
     _rowBus(a,b,c,d),
     _d(d),// Init 32x32-specific elements:
     _sclk(sclk),
     _latch(latch),
     _oe(oe)
{
    init(16,dbuf);
}

void RGBmatrixPanel::begin(void)
{

    backindex   = 0;                         // Back buffer
    buffptr     = matrixbuff[1 - backindex]; // -> front buffer
    // activePanel = this;                      // For interrupt hander

    // Set up Timer for interrupt:
#ifndef DEBUG
    _refresh.attach_us(this,(&RGBmatrixPanel::updateDisplay),100);   //updateDisplay() called every 1ms
#else
    _refresh.attach(this,(&RGBmatrixPanel::updateDisplay),0.5);   //updateDisplay() called every 2s
#endif
}

// Original RGBmatrixPanel library used 3/3/3 color.  Later version used
// 4/4/4.  Then Adafruit_GFX (core library used across all Adafruit
// display devices now) standardized on 5/6/5.  The matrix still operates
// internally on 4/4/4 color, but all the graphics functions are written
// to expect 5/6/5...the matrix lib will truncate the color components as
// needed when drawing.  These next functions are mostly here for the
// benefit of older code using one of the original color formats.

// Promote 3/3/3 RGB to Adafruit_GFX 5/6/5
uint16_t RGBmatrixPanel::Color333(uint8_t r, uint8_t g, uint8_t b)
{
    // RRRrrGGGgggBBBbb
    return ((r & 0x7) << 13) | ((r & 0x6) << 10) |
           ((g & 0x7) <<  8) | ((g & 0x7) <<  5) |
           ((b & 0x7) <<  2) | ((b & 0x6) >>  1);
}

// Promote 4/4/4 RGB to Adafruit_GFX 5/6/5
uint16_t RGBmatrixPanel::Color444(uint8_t r, uint8_t g, uint8_t b)
{
    // RRRRrGGGGggBBBBb
    return ((r & 0xF) << 12) | ((r & 0x8) << 8) |
           ((g & 0xF) <<  7) | ((g & 0xC) << 3) |
           ((b & 0xF) <<  1) | ((b & 0x8) >> 3);
}

// Demote 8/8/8 to Adafruit_GFX 5/6/5
// If no gamma flag passed, assume linear color
uint16_t RGBmatrixPanel::Color888(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 11) | ((g & 0xFC) << 5) | (b >> 3);
}

// 8/8/8 -> gamma -> 5/6/5
uint16_t RGBmatrixPanel::Color888(uint8_t r, uint8_t g, uint8_t b, bool gflag)
{
    if(gflag) { // Gamma-corrected color?
        r = gamma[r]; // Gamma correction table maps
        g = gamma[g]; // 8-bit input to 4-bit output
        b = gamma[b];
        return (r << 12) | ((r & 0x8) << 8) | // 4/4/4 -> 5/6/5
               (g <<  7) | ((g & 0xC) << 3) |
               (b <<  1) | ( b        >> 3);
    } // else linear (uncorrected) color
    return ((r & 0xF8) << 11) | ((g & 0xFC) << 5) | (b >> 3);
}

uint16_t RGBmatrixPanel::ColorHSV(long hue, uint8_t sat, uint8_t val, bool gflag)
{

    uint8_t  r, g, b, lo;
    uint16_t s1, v1;

    // Hue
    hue %= 1536;             // -1535 to +1535
    if(hue < 0) hue += 1536; //     0 to +1535
    lo = hue & 255;          // Low byte  = primary/secondary color mix
    switch(hue >> 8) {       // High byte = sextant of colorwheel
        case 0 :
            r = 255     ;
            g =  lo     ;
            b =   0     ;
            break; // R to Y
        case 1 :
            r = 255 - lo;
            g = 255     ;
            b =   0     ;
            break; // Y to G
        case 2 :
            r =   0     ;
            g = 255     ;
            b =  lo     ;
            break; // G to C
        case 3 :
            r =   0     ;
            g = 255 - lo;
            b = 255     ;
            break; // C to B
        case 4 :
            r =  lo     ;
            g =   0     ;
            b = 255     ;
            break; // B to M
        default:
            r = 255     ;
            g =   0     ;
            b = 255 - lo;
            break; // M to R
    }

    // Saturation: add 1 so range is 1 to 256, allowig a quick shift operation
    // on the result rather than a costly divide, while the type upgrade to int
    // avoids repeated type conversions in both directions.
    s1 = sat + 1;
    r  = 255 - (((255 - r) * s1) >> 8);
    g  = 255 - (((255 - g) * s1) >> 8);
    b  = 255 - (((255 - b) * s1) >> 8);

    // Value (brightness) & 16-bit color reduction: similar to above, add 1
    // to allow shifts, and upgrade to int makes other conversions implicit.
    v1 = val + 1;
    if(gflag) { // Gamma-corrected color?
        r = gamma[(r * v1) >> 8]; // Gamma correction table maps
        g = gamma[(g * v1) >> 8]; // 8-bit input to 4-bit output
        b = gamma[(b * v1) >> 8];
        //before pgm_read_byte(&gamma[(b * v1) >> 8])
    } else { // linear (uncorrected) color
        r = (r * v1) >> 12; // 4-bit results
        g = (g * v1) >> 12;
        b = (b * v1) >> 12;
    }
    return (r << 12) | ((r & 0x8) << 8) | // 4/4/4 -> 5/6/5
           (g <<  7) | ((g & 0xC) << 3) |
           (b <<  1) | ( b        >> 3);
}
uint16_t RGBmatrixPanel::ColorHSV(float hue, float sat, float val, bool gflag)
{
    int i = floor(hue * 6);
    float f = hue*6-i;
    float p = val*(1-sat);
    float q = val*(1-f*sat);
    float t = val*(1-(1-f)*sat);
    float r = 0,g=0,b=0;
    switch(i % 6) {
        case 0:
            r = val;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = val;
            b = p;
            break;
        case 2:
            r = p;
            g = val;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = val;
            break;
        case 4:
            r = t;
            g = p;
            b = val;
            break;
        case 5:
            r = val;
            g = p;
            b = q;
            break;
    }
    return Color888(r*255,g*255,b*255,gflag);
}
void RGBmatrixPanel::drawPixel(int16_t x, int16_t y, uint16_t c)
{
    uint8_t r, g, b, bit, limit, *ptr;
    if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
    switch(rotation) {
        case 1:
            swap(x, y);
            x = _rawWidth  - 1 - x;
            break;
        case 2:
            x = _rawWidth  - 1 - x;
            y = _rawHeight - 1 - y;
            break;
        case 3:
            swap(x, y);
            y = _rawHeight - 1 - y;
            break;
    }

    // Adafruit_GFX uses 16-bit color in 5/6/5 format, while matrix needs
    // 4/4/4.  Pluck out relevant bits while separating into R,G,B:
    r =  c >> 12;        // RRRRrggggggbbbbb
    g = (c >>  7) & 0xF; // rrrrrGGGGggbbbbb
    b = (c >>  1) & 0xF; // rrrrrggggggBBBBb
    // Loop counter stuff
    bit   = 2;
    limit = 1 << nPlanes;
    if(y < nRows) {
        // Data for the upper half of the display is stored in the lower bits of each byte.
        ptr = &matrixbuff[backindex][y*_rawWidth*(nPlanes-1) + x]; // Base addr
        // Plane 0 is a tricky case -- its data is spread about, stored in least two bits not used by the other planes.
        ptr[64] &= ~(_BV(1)|_BV(0));    // Plane 0 R,G mask(0b11111100) out in one op
        if(r & 1) ptr[64] |=  _BV(0);  // Plane 0 R: 64 bytes ahead, bit 0
        if(g & 1) ptr[64] |=  _BV(1);  // Plane 0 G: 64 bytes ahead, bit 1
        if(b & 1) ptr[32] |=  _BV(0);  // Plane 0 B: 32 bytes ahead, bit 0
        else      ptr[32] &= ~_BV(0);  // Plane 0 B unset; mask out
        // The remaining three image planes are more normal-ish.
        // Data is stored in the high 6 bits so it can be quickly
        // copied to the DATAPORT register w/6 output lines.
        for(; bit < limit; bit <<= 1) {
            *ptr &= ~(_BV(4)|_BV(3)|_BV(2)) ;  // Mask(0b11100011) out R,G,B in one op
            if(r & bit) *ptr |= _BV(2); // Plane N R: bit 2
            if(g & bit) *ptr |= _BV(3); // Plane N G: bit 3
            if(b & bit) *ptr |= _BV(4); // Plane N B: bit 4
            ptr  += _rawWidth;          // Advance to next bit plane
        }
    } else {
        // Data for the lower half of the display is stored in the upper bits, except for the plane 0 stuff, using 2 least bits.
        ptr = &matrixbuff[backindex][(y-nRows)*_rawWidth*(nPlanes-1) + x];
        *ptr &= ~(_BV(1)|_BV(0));               // Plane 0 G,B mask out in one op
        if(r & 1)  ptr[32] |=  _BV(1); // Plane 0 R: 32 bytes ahead, bit 1
        else       ptr[32] &= ~_BV(1); // Plane 0 R unset; mask out
        if(g & 1) *ptr     |=  _BV(0); // Plane 0 G: bit 0
        if(b & 1) *ptr     |=  _BV(1); // Plane 0 B: bit 0
        for(; bit < limit; bit <<= 1) {
            *ptr &= ~(_BV(7)|_BV(6)|_BV(5));             // Mask out R,G,B in one op
            if(r & bit) *ptr |= _BV(5);  // Plane N R: bit 5
            if(g & bit) *ptr |= _BV(6);  // Plane N G: bit 6
            if(b & bit) *ptr |= _BV(7);  // Plane N B: bit 7
            ptr  += _rawWidth;                  // Advance to next bit plane
        }
    }
}

void RGBmatrixPanel::fillScreen(uint16_t c)
{
    if((c == 0x0000) || (c == 0xffff)) {
        // For black or white, all bits in frame buffer will be identically
        // set or unset (regardless of weird bit packing), so it's OK to just
        // quickly memset the whole thing:
        memset(matrixbuff[backindex], c, 32 * nRows * 3);
    } else {
        // Otherwise, need to handle it the long way:
        Adafruit_GFX::fillScreen(c);
    }
}

// Return address of back buffer -- can then load/store data directly
uint8_t *RGBmatrixPanel::backBuffer()
{
    return matrixbuff[backindex];
}

// For smooth animation -- drawing always takes place in the "back" buffer;
// this method pushes it to the "front" for display.  Passing "true", the
// updated display contents are then copied to the new back buffer and can
// be incrementally modified.  If "false", the back buffer then contains
// the old front buffer contents -- your code can either clear this or
// draw over every pixel.  (No effect if double-buffering is not enabled.)
void RGBmatrixPanel::swapBuffers(bool copy)
{
    if(matrixbuff[0] != matrixbuff[1]) {
        // To avoid 'tearing' display, actual swap takes place in the interrupt
        // handler, at the end of a complete screen refresh cycle.
        swapflag = true;                  // Set flag here, then...
        while(swapflag == true) wait_ms(1); // wait for interrupt to clear it
        if(copy == true) {
            memcpy(matrixbuff[backindex], matrixbuff[1-backindex], 32 * nRows * 3);
        }
    }
}

// Dump display contents to the Serial Monitor, adding some formatting to
// simplify copy-and-paste of data as a PROGMEM-embedded image for another
// sketch.  If using multiple dumps this way, you'll need to edit the
// output to change the 'img' name for each.  Data can then be loaded
// back into the display using a pgm_read_byte() loop.
void RGBmatrixPanel::dumpMatrix(void)
{
#ifdef DEBUG
    log_debug("\r\ncall dumpMatrix%s","\r\n");
    int buffsize=32*nRows*3;
    for(int item=0; item<buffsize; item++) {
        log_debug("0x%02X",matrixbuff[backindex][item]);
        if((item%32)==31)    log_debug(",\r\n");
        else                log_debug(",");
    }
    log_debug("%s","\r\n\r\n");
#endif

}

void RGBmatrixPanel::updateDisplay(void)
{
    _oe=1;
    _latch=1;
    if(++plane >= nPlanes) {        // Advance plane counter.  Maxed out?
        plane = 0;                  // Yes, reset to plane 0, and
        if(++row >= nRows) {        // advance row counter.  Maxed out?
            row= 0;                 // Yes, reset row counter, then...
            if(swapflag == true) {  // Swap front/back buffers if requested
                backindex = 1 - backindex;
                swapflag  = false;
            }
            buffptr = matrixbuff[1-backindex]; // Reset into front buffer
        }
    } else if(plane == 1) {
        _rowBus.write(row);
    }
    _oe=0;
    _latch=0;
    if(plane > 0) {
        for(int i=0; i<32; i++) {
            _dataBus.write((*(buffptr+i)>>2));
            _sclk=1;
            _sclk=0;
        }
        buffptr += _rawWidth;
    } else {
        for(int i=0; i<32; i++) {
            _dataBus.write(((buffptr[i]<<4)|((buffptr[i+32]<<2)&0x0C)|((buffptr[i+64])&0x03)));
            _sclk=1;
            _sclk=0;
        }
    }

}

