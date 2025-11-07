// File: src/DisplayController.cpp

#include "DisplayController.h"
#include "Config.h"

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

#include <Fonts/FreeMonoBold12pt7b.h>

#ifndef PNG_MAX_BUFFERED_PIXELS
// Allow PNGdec to handle full-resolution 800px-wide truecolor images
#define PNG_MAX_BUFFERED_PIXELS ((800 * 4 + 1) * 2)
#endif
#include <PNGdec.h>

// NOTE: The concrete GxEPD2 display class/template should be included in DisplayController.h,
// e.g. via <GxEPD2_BW.h> and a selection header providing GxEPD2_750_T7. We rely on that here.

// PNG decoder instance
static PNG png;

// Global pointer so the PNG callback can access the display
static GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT>* g_display = nullptr;

// Heap line buffer for PNG rows (RGB565). Size tracks number of pixels.
static uint16_t* s_lineBuf = nullptr;
static int       s_lineBufPixels = 0;

// ------------------------
// PNG callback prototypes (C linkage for PNGdec)
// ------------------------
extern "C" {
    int      PNGDraw(PNGDRAW* pDraw);
    void*    pngOpen(const char* filename, int32_t* size);
    void     pngClose(void* handle);
    int32_t  pngRead(PNGFILE* page, uint8_t* buffer, int32_t length);
    int32_t  pngSeek(PNGFILE* page, int32_t position);
}

// ------------------------
// Utility: ensure line buffer capacity
// ------------------------
static bool ensureLineBuf(int pixels) {
    if (pixels <= 0) return false;
    if (pixels <= s_lineBufPixels) return true;

    void* newBuf = realloc(s_lineBuf, static_cast<size_t>(pixels) * sizeof(uint16_t));
    if (!newBuf) {
        Serial.printf("Display: failed to allocate line buffer for %d pixels (%u bytes)\n",
                      pixels, (unsigned)(pixels * sizeof(uint16_t)));
        return false;
    }
    s_lineBuf = static_cast<uint16_t*>(newBuf);
    s_lineBufPixels = pixels;
    Serial.printf("Display: line buffer resized to %d pixels (%u bytes), free heap=%u\n",
                  s_lineBufPixels, (unsigned)(s_lineBufPixels * sizeof(uint16_t)),
                  (unsigned)ESP.getFreeHeap());
    return true;
}

// ------------------------
// PNG row draw callback
// (This version supports PNGdec variants that DO NOT provide pDraw->iX/pDraw->iY.
// We assume rows start at x=0 and use pDraw->y and pDraw->iWidth.)
// ------------------------
int PNGDraw(PNGDRAW* pDraw) {
    if (!g_display) return 0;

    const int dispW = g_display->width();
    const int dispH = g_display->height();

    const int y    = pDraw->y;
    const int x0   = 0;                // older PNGdec has no iX; assume 0
    const int srcW = pDraw->iWidth;    // number of pixels the decoder will write into our buffer

    // Vertical clip
    if (y < 0 || y >= dispH) return 1;

    // Ensure buffer fits full decoded row to avoid overflow
    if (!ensureLineBuf(srcW)) return 0;

    // Decode one row into RGB565 (big endian) into our buffer
    png.getLineAsRGB565(pDraw, s_lineBuf, PNG_RGB565_BIG_ENDIAN, 0xFFFF);

    // Horizontal clip to display bounds
    int maxDrawable = dispW - x0;
    if (maxDrawable <= 0) return 1;              // row starts beyond right edge
    int drawW = (srcW < maxDrawable) ? srcW : maxDrawable;
    if (drawW <= 0) return 1;

    // Convert to 1-bit BW and draw
    // Luma approximation from RGB565 (weights ~BT.601)
    for (int i = 0; i < drawW; ++i) {
        uint16_t c = s_lineBuf[i];
        uint8_t r = (c >> 11) & 0x1F;  // 5 bits
        uint8_t g = (c >> 5)  & 0x3F;  // 6 bits
        uint8_t b =  c        & 0x1F;  // 5 bits
        uint16_t gray = (r * 8 * 30 + g * 4 * 59 + b * 8 * 11) / 100; // 0..255 approx
        uint16_t bw = (gray < 128) ? GxEPD_BLACK : GxEPD_WHITE;
        g_display->drawPixel(x0 + i, y, bw);
    }

    return 1; // row handled
}

// ------------------------
// File callbacks for PNGdec
// ------------------------
void* pngOpen(const char* filename, int32_t* size) {
    File* file = new File(SD.open(filename, FILE_READ));
    if (!file || !*file) {
        Serial.printf("Display: pngOpen failed for %s\n", filename ? filename : "(null)");
        if (file) { delete file; }
        return nullptr;
    }
    *size = file->size();
    return (void*)file;
}

void pngClose(void* handle) {
    File* file = (File*)handle;
    if (file) {
        file->close();
        delete file;
    }
}

int32_t pngRead(PNGFILE* page, uint8_t* buffer, int32_t length) {
    File* file = (File*)page->fHandle;
    if (!file) return 0;
    return file->read(buffer, (size_t)length);
}

int32_t pngSeek(PNGFILE* page, int32_t position) {
    File* file = (File*)page->fHandle;
    if (!file) return 0;
    return file->seek(position) ? 1 : 0;
}

// ------------------------
// DisplayController methods
// ------------------------

void DisplayController::init() {
    pinMode(EPD_PWR, OUTPUT);
    digitalWrite(EPD_PWR, LOW);
    isPowered = false;

    display = new GxEPD2_BW<GxEPD2_750_T7, GxEPD2_750_T7::HEIGHT>(
        GxEPD2_750_T7(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY)
    );

    g_display = display;  // set global for PNG callback

    Serial.println("Display: controller initialized");
}

void DisplayController::powerOn() {
    if (!isPowered) {
        // Deselect SD to avoid bus contention
        digitalWrite(SD_CS, HIGH);
        digitalWrite(EPD_PWR, HIGH);
        delay(100);
        display->init(115200);
        isPowered = true;
        Serial.println("Display: powered ON");
    }
}

void DisplayController::powerOff() {
    if (isPowered) {
        display->hibernate();
        digitalWrite(EPD_PWR, LOW);
        isPowered = false;
        Serial.println("Display: powered OFF");
    }
    // Free any line buffer if allocated
    if (s_lineBuf) {
        free(s_lineBuf);
        s_lineBuf = nullptr;
        s_lineBufPixels = 0;
        Serial.println("Display: freed line buffer");
    }
}

void DisplayController::showMessage(const char* message) {
    powerOn();
    // Ensure SD is deselected while driving EPD
    digitalWrite(SD_CS, HIGH);

    display->setRotation(1);
    display->setFullWindow();

    // Clear first (paged clear)
    display->firstPage();
    do {
        display->fillScreen(GxEPD_WHITE);
    } while (display->nextPage());

    delay(100);

    // Draw message (paged)
    display->firstPage();
    do {
        display->fillScreen(GxEPD_WHITE);
        display->setTextColor(GxEPD_BLACK);
        display->setFont(&FreeMonoBold12pt7b);
        display->setCursor(10, 30);
        display->println(message);
    } while (display->nextPage());

    Serial.printf("Display: message shown: %s\n", message ? message : "(null)");
}

void DisplayController::clear() {
    powerOn();
    digitalWrite(SD_CS, HIGH);  // Deselect SD

    display->setFullWindow();
    display->firstPage();
    do {
        display->fillScreen(GxEPD_WHITE);
    } while (display->nextPage());

    Serial.println("Display: cleared");
}

void DisplayController::displayImageFromSD(const String& filename) {
    Serial.printf("Display: loading PNG from SD: %s\n", filename.c_str());

    // Make sure display is not selected while touching SD
    digitalWrite(EPD_CS, HIGH);

    // Check file existence
    if (!SD.exists(filename.c_str())) {
        Serial.println("Display: PNG file not found");
        showMessage("Error: Image file\nnot found");
        return;
    }

    // Get size early for logging
    File f = SD.open(filename.c_str(), FILE_READ);
    if (!f) {
        Serial.println("Display: failed to open PNG");
        showMessage("Error: Cannot\nopen image");
        return;
    }
    size_t fileSize = f.size();
    f.close();
    Serial.printf("Display: PNG size = %u bytes, free heap = %u\n",
                  (unsigned)fileSize, (unsigned)ESP.getFreeHeap());

    // Power on and set up display
    powerOn();
    digitalWrite(SD_CS, HIGH);   // Deselect SD when not actively reading

    display->setRotation(1);
    display->setFullWindow();

    const int dispW = display->width();
    const int dispH = display->height();
    Serial.printf("Display: begin paged draw, W x H = %d x %d\n", dispW, dispH);

    int pass = 0;
    display->firstPage();
    do {
        ++pass;
        Serial.printf("Display: page pass %d, free heap=%u\n", pass, (unsigned)ESP.getFreeHeap());

        // Clear this page to white
        display->fillScreen(GxEPD_WHITE);

        // Open PNG for this pass (PNGdec expects an open per decode)
        int rc = png.open(filename.c_str(), pngOpen, pngClose, pngRead, pngSeek, PNGDraw);
        if (rc != PNG_SUCCESS) {
            Serial.printf("Display: PNG open failed, err=%d\n", rc);
            showMessage("Error: Invalid\nPNG file");
            return;
        }

        const int imgW = png.getWidth();
        const int imgH = png.getHeight();
        Serial.printf("Display: PNG info %d x %d, bpp=%d\n", imgW, imgH, png.getBpp());
        if (imgW > dispW || imgH > dispH) {
            Serial.printf("Display: note - image larger than display, will clip to %d x %d\n", dispW, dispH);
        }

        // Decode; PNGDraw will render each row into current page buffer with clipping
        Serial.println("Display: decoding PNG...");
        rc = png.decode(nullptr, 0);
        png.close();

        if (rc != PNG_SUCCESS) {
            Serial.printf("Display: PNG decode failed, err=%d\n", rc);
            showMessage("Error: Decode\nfailed");
            return;
        }

    } while (display->nextPage());

    Serial.println("Display: paged render complete, refreshing");
    display->refresh();
    Serial.println("Display: PNG displayed successfully");
}
