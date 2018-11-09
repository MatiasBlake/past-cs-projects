#include "mbed.h"
#include "Adafruit_GFX.h"

#ifndef _BV
#define _BV(bit) (1<<(bit))
#endif

#ifdef DEBUG
#define log_debug(format,...)   std::printf(format,##__VA_ARGS__)
#else
#define log_debug(...)
#endif

/** RGBmatrixPanel is class for full color LED matrix
* @code
#include "mbed.h"
#include "RGBmatrixPanel.h" // Hardware-specific library

PinName ub1=p6;
PinName ug1=p7;
PinName ur1=p8;
PinName lb2=p9;
PinName lg2=p10;
PinName lr2=p11;
RGBmatrixPanel matrix(ur1,ug1,ub1,lr2,lg2,lb2,p12,p13,p14,p15,p16,p17,false);
//RGBmatrixPanel(r1, g1, b1, r2, g2, b2, a, b, c, sclk, latch, oe, enable double_buffer);

int main()
{
    matrix.begin();
    while(1) {
        // fill the screen with 'black'
        matrix.fillScreen(matrix.Color333(0, 0, 0));
        // draw a pixel in solid white
        matrix.drawPixel(0, 0, matrix.Color333(7, 7, 7));
        wait_ms(500);

        // fix the screen with green
        matrix.fillRect(0, 0, 32, 16, matrix.Color333(0, 7, 0));
        wait_ms(500);

        // draw a box in yellow
        matrix.drawRect(0, 0, 32, 16, matrix.Color333(7, 7, 0));
        wait_ms(500);

        // draw an 'X' in red
        matrix.drawLine(0, 0, 31, 15, matrix.Color333(7, 0, 0));
        matrix.drawLine(31, 0, 0, 15, matrix.Color333(7, 0, 0));
        wait_ms(500);

        // draw a blue circle
        matrix.drawCircle(7, 7, 7, matrix.Color333(0, 0, 7));
        wait_ms(500);

        // fill a violet circle
        matrix.fillCircle(23, 7, 7, matrix.Color333(7, 0, 7));
        wait_ms(500);

        // fill the screen with 'black'
        matrix.fillScreen(matrix.Color333(0, 0, 0));

        // draw some text!
        matrix.setCursor(1, 0);   // start at top left, with one pixel of spacing
        matrix.setTextSize(1);    // size 1 == 8 pixels high

        // printff each letter with a rainbow color
        matrix.setTextColor(matrix.Color333(7,0,0));
        matrix.putc('1');
        matrix.setTextColor(matrix.Color333(7,4,0));
        matrix.putc('6');
        matrix.setTextColor(matrix.Color333(7,7,0));
        matrix.putc('x');
        matrix.setTextColor(matrix.Color333(4,7,0));
        matrix.putc('3');
        matrix.setTextColor(matrix.Color333(0,7,0));
        matrix.putc('2');

        matrix.setCursor(1, 9);   // next line
        matrix.setTextColor(matrix.Color333(0,7,7));
        matrix.putc('*');
        matrix.setTextColor(matrix.Color333(0,4,7));
        matrix.putc('R');
        matrix.setTextColor(matrix.Color333(0,0,7));
        matrix.putc('G');
        matrix.setTextColor(matrix.Color333(4,0,7));
        matrix.putc('B');
        matrix.setTextColor(matrix.Color333(7,0,4));
        matrix.putc('*');
        wait_ms(500);
    }
}

* @endcode

 */
class RGBmatrixPanel : public Adafruit_GFX
{

public:
    // Constructor for 16x32 panel:
    RGBmatrixPanel(PinName r1,PinName g1,PinName b1,PinName r2,PinName g2,PinName b2,PinName a,PinName b,PinName c,PinName sclk,PinName latch,PinName oe, bool dbuf);

    // Constructor for 32x32 panel (adds 'd' pin):
    RGBmatrixPanel(PinName r1,PinName r2,PinName g1,PinName g2,PinName b1,PinName b2,PinName a,PinName b,PinName c,PinName d,PinName sclk, PinName latch,PinName oe,bool dbuf);

    /** @fn void RGBmatrixPanel::begin(void)
    *   @bref Attach a updateDisplay() to be called by the Ticker(every 100us)
    */
    void begin(void);
    /** @fn void RGBmatrixPanel::drawPixel(int16_t x, int16_t y, uint16_t c)
    *   @bref drawPixel
    */
    virtual void drawPixel(int16_t x,int16_t y,uint16_t c);
    /** @fn void RGBmatrixPanel::fillScreen(uint16_t c)
    *   @bref fillScreen
    *   @param  c   fill screen 16bit color 0x0000 ~ 0xFFFF
    */
    virtual void fillScreen(uint16_t c);
    /** @fn void RGBmatrixPanel::updateDisplay(void)
    *   @param c    updateDisplay\n
    *               This method is called by the interrupt start at begin().
    */
    void updateDisplay(void);
    /** @fn void RGBmatrixPanel::swapBuffers(bool copy)
    *   @param  copy swap buffer (if you use double-buffer)
    */
    void swapBuffers(bool copy);
    /** @fn void RGBmatrixPanel::dumpMatrix(void)
    *   @bref dump to default USB Serial\n
    *         Declaration is required to use.(#define DEBUG)
    */
    void dumpMatrix(void);

    uint8_t *backBuffer(void);

    /** @fn uint16_t RGBmatrixPanel::Color333(uint8_t r, uint8_t g, uint8_t b)
    *   @bref up convert to 16bit color from 9bit color.
    *   @return 16bit(uint16_t) color value
    */
    uint16_t Color333(uint8_t r, uint8_t g, uint8_t b);
    /** @fn uint16_t RGBmatrixPanel::Color444(uint8_t r, uint8_t g, uint8_t b)
    *   @bref up convert to 16bit color from 12bit color.
    *   @param r 0~7
    *   @param g 0~7
    *   @param b 0~7
    *   @return 16bit(uint16_t) color value
    */
    uint16_t Color444(uint8_t r, uint8_t g, uint8_t b);
    /** @fn uint16_t RGBmatrixPanel::Color888(uint8_t r, uint8_t g, uint8_t b)
    *   @bref down convert to 16bit color from 24bit color.
    *   @return 16bit(uint16_t) color value
    */
    uint16_t Color888(uint8_t r, uint8_t g, uint8_t b);
    /** @fn uint16_t RGBmatrixPanel::Color888(uint8_t r, uint8_t g, uint8_t b, bool gflag)
    *   @bref down convert to 16bit color from 24bit color using the gamma value table.
    *   @return 16bit(uint16_t) color value
    */
    uint16_t Color888(uint8_t r, uint8_t g, uint8_t b, bool gflag);
    /** @fn uint16_t RGBmatrixPanel::ColorHSV(long hue, uint8_t sat, uint8_t val, bool gflag)
    *   @bref convert to 16bit color from (unsigned integer)HSV color using the gamma value table.
    *   @param hue 0~1536(decimal value)
    *   @param sat 0~255(decimal value) Does not make sense that it is not a multiple of 32.
    *   @param val 0~255(decimal value) Does not make sense that it is not a multiple of 32.
    *   @return 16bit(uint16_t) color value
    */
    uint16_t ColorHSV(long hue, uint8_t sat, uint8_t val, bool gflag)
    ;
    /** @fn uint16_t RGBmatrixPanel::ColorHSV(float hue, float sat, float val, bool gflag)
    *   @bref convert to 16bit color from (float)HSV color using the gamma value table.
    *   @param hue Normalized value from 0.0 to 1.0
    *   @param sat Normalized value from 0.0 to 1.0
    *   @param val Normalized value from 0.0 to 1.0
    *   @return 16bit(uint16_t) color value
    */
    uint16_t ColorHSV(float hue, float sat, float val, bool gflag);
private:
    uint8_t *matrixbuff[2];
    uint8_t nRows;
    uint8_t backindex;
    bool swapflag;

    // Init/alloc code common to both constructors:
    void init(uint8_t rows, bool dbuf);

    BusOut _dataBus;
    BusOut _rowBus;
    DigitalOut _d,_sclk, _latch, _oe;
    Ticker _refresh;
    // Counters/pointers for interrupt handler:
    uint8_t row, plane;
    uint8_t *buffptr;
};