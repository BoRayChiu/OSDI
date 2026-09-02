#include "framebuffer.h"
#include "mailbox.h"

unsigned int width, height, pitch;
unsigned char *fb_ptr;

void framebuffer_init() {
    // setup mailbox buffer for framebuffer configuration
    mailbox[0] = 31 * 4;
    mailbox[1] = REQUEST_CODE;

    // set Physical Display Width/Height
    mailbox[2] = 0x00048003; // Tag identifier
    mailbox[3] = 8; // Value buffer size
    mailbox[4] = 8; // Request size
    mailbox[5] = 1024; // Width
    mailbox[6] = 768;  // Height

    // Set Virtual Buffer Width/Height
    mailbox[7] = 0x00048004; // Tag identifier
    mailbox[8] = 8; // Value buffer size
    mailbox[9] = 8; // Request size
    mailbox[10] = 1024; // Width
    mailbox[11] = 768; // Height

    // Set X/Y Offset
    mailbox[12] = 0x00048009; // Tag identifier
    mailbox[13] = 8; // Value buffer size
    mailbox[14] = 8; // Request size
    mailbox[15] = 0; // X Offset
    mailbox[16] = 0; // Y Offset

    // Set Color Depth (Bits Per Pixel)
    mailbox[17] = 0x00048005; // Tag identifier
    mailbox[18] = 4; // Value buffer size
    mailbox[19] = 4; // Request size
    mailbox[20] = 32; // 32 bits (ARGB format)

    // Set Pixel Order (RGB or BGR)
    mailbox[21] = 0x00048006; // Tag identifier
    mailbox[22] = 4; // Value buffer size
    mailbox[23] = 4; // Request size
    mailbox[24] = 1; // 1 for RGB format, 0 for BGR format

    // Allocate Framebuffer
    mailbox[25] = 0x00040001; // Tag identifier
    mailbox[26] = 8; // Value buffer size
    mailbox[27] = 8; // Request size
    mailbox[28] = 4096; // Alignment
    mailbox[29] = 0;    // Will be filled with the framebuffer address by the GPU
    mailbox[30] = END_TAG; // End Tag

    if (mailbox_call(8) && mailbox[20] == 32 && mailbox[28] != 0) {
        width = mailbox[5];
        height = mailbox[6];
        pitch = mailbox[5] * 4; // width * bytes_per_pixel (4 for 32 bits)

        // Convert the framebuffer address from GPU address to CPU address
        fb_ptr = (unsigned char *)(unsigned long)(mailbox[28] & 0x3FFFFFFF);
    }
}

void show_splash() {
    int x, y;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            int location = (y * pitch) + (x * 4); 
            fb_ptr[location + 0] = x % 256;         // R
            fb_ptr[location + 1] = y % 256;         // G
            fb_ptr[location + 2] = (x + y) % 256;   // B
            fb_ptr[location + 3] = 255;             // Alpha
        }
    }
}