typedef uint8_t Pixels[10][3];

#define PIX_GRN 0
#define PIX_RED 1
#define PIX_BLU 2

void init_neopixel_blit();
void neopixel_blit(Pixels pixels);
