#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <qrencode.h>
#include "qrencode.h"

// build: gcc text2qr.c -o text_to_qr.exe -I..\libqrencode -L..\libqrencode\builds -lqrencode

// Cmake build cmd: cmake ".." -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_RC_COMPILER=windres -DWITH_TOOLS=NO -DWITHOUT_PNG=YES



/*
extern QRcode *QRcode_encodeString(
    const char *string, 
    int version, 
    QRencodeMode hint, 
    QRecLevel level, 
    int casesensitive
);
Parameter Breakdown
** const char *string
- The NUL-terminated text data you want to bake into the QR Code (e.g., your command-line string).

**int version
- The QR Code size version (ranges from 1 to 40).
Tip: Set this to 0 to let the library automatically detect and choose the smallest version size that fits your text.

** QRencodeMode hint
- Tells the library how to parse your string. For generic text and URLs, use QR_MODE_8 (8-bit data mode).
(Note: The other common hint option is QR_MODE_KANJI for Japanese characters).

** QRecLevel level
- The Reed-Solomon Error Correction Level. Higher levels let the code remain readable even if it gets scratched or dirty:
    QR_ECLEVEL_L : Lowest (~7% correction)

    QR_ECLEVEL_M : Medium (~15% correction)

    QR_ECLEVEL_Q : Quality (~25% correction)

    QR_ECLEVEL_H : Highest (~30% correction)

** int casesensitive
- Set to 1 (True) for case-sensitive encoding, or 0 (False) for case-insensitive encoding. For generic alphanumeric URLs, 1 is usually preferred.

**Return Value
- Returns a pointer to a QRcode structure holding the grid matrix dimensions (width) and pixel mapping (data).
  If encoding fails (e.g., the text is too long for the specified version), it returns NULL.
*/

// Define the size of each QR "module" (pixel block) in the output image
#define PIXEL_SIZE 8 
#define MARGIN 4

void write_ppm(const char *filename, QRcode *qr) {
    // Calculate final image dimensions including the margin
    int qr_width = qr->width;
    int img_width = (qr_width + (MARGIN * 2)) * PIXEL_SIZE;
    int img_height = img_width;

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("Error opening output file");
        return;
    }

    // Write PPM P6 Header
    // P6 means binary RGB, followed by width, height, and max color value (255)
    fprintf(fp, "P6\n%d %d\n255\n", img_width, img_height);

    // Allocate a single row of RGB pixels for writing
    unsigned char *row_buffer = (unsigned char *)malloc(img_width * 3);
    if (!row_buffer) {
        perror("Memory allocation failed");
        fclose(fp);
        return;
    }

    // Iterate through each row of the final image
    for (int y = 0; y < img_height; y++) {
        // Map current image 'y' to the corresponding QR matrix row
        int qr_y = (y / PIXEL_SIZE) - MARGIN;

        for (int x = 0; x < img_width; x++) {
            // Map current image 'x' to the corresponding QR matrix column
            int qr_x = (x / PIXEL_SIZE) - MARGIN;

            int is_black = 0;

            // Check if coordinates fall within the actual QR code boundaries
            if (qr_y >= 0 && qr_y < qr_width && qr_x >= 0 && qr_x < qr_width) {
                // libqrencode sets the lowest bit (1) if the module is black
                if (qr->data[qr_y * qr_width + qr_x] & 1) {
                    is_black = 1;
                }
            }

            // Fill RGB channels (0,0,0 for black; 255,255,255 for white)
            int buf_idx = x * 3;
            if (is_black) {
                row_buffer[buf_idx]     = 0;   // R
                row_buffer[buf_idx + 1] = 0;   // G
                row_buffer[buf_idx + 2] = 0;   // B
            } else {
                row_buffer[buf_idx]     = 255; // R
                row_buffer[buf_idx + 1] = 255; // G
                row_buffer[buf_idx + 2] = 255; // B
            }
        }
        // Write the constructed row to the file
        fwrite(row_buffer, 1, img_width * 3, fp);
    }

    free(row_buffer);
    fclose(fp);
    printf("Successfully generated QR code: %s\n", filename);
}

int main(int argc, char *argv[]) {
    // Check if the user actually passed a text argument
    if (argc < 2) {
        printf("Usage: %s <text_to_encode>\n", argv[0]);
        return 1;
    }

    // argv[1] contains the first argument after the program name
    const char *text = argv[1];
    const char *output_filename = "qrcode.ppm";

    // Print the string properly using %s and a pointer
    printf("Input text: %s\n", text);

    // Generate QR Code
    QRcode *qr = QRcode_encodeString(text, 0, QR_ECLEVEL_H, QR_MODE_8, 1);

    if (qr == NULL) {
        perror("Failed to generate QR code");
        return 1;
    }

    // Convert QR structure to PPM file
    write_ppm(output_filename, qr);

    // Free resources
    QRcode_free(qr);

    return 0;
}