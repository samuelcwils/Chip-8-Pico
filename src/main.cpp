
// ****************************************************************************
//
//                                 Main code
//
// ****************************************************************************

#include "include.h"
#include "chip8.h"
#include "breakoutRom.c"

extern const unsigned long breakout_length;
extern const unsigned char breakout[];
#define PIXEL_SCALE 3
#define SCALE_X 3
#define SCALE_Y 5
#define START_X 23
#define START_Y 10

int main()
{

	stdio_usb_init();

	char wait;
	while(wait < 2)
	{
		std::cout << "INITIALIZED\n";
		sleep_ms(1000);
		wait++;
	}
	
	// initialize videomode
	ALIGNED u8 frameBuffer[256*192];
	Video(DEV_VGA, RES_ZX, FORM_MONO, frameBuffer);
	DrawClear(&Canvas);

	//create chip8
	chip8* chip = new chip8;

	//initialize inputs
	gpio_init(16);
	gpio_init(17);
	gpio_set_dir(16, false);
	gpio_set_dir(17, false);

	//loadRom
	chip->loadROM(breakout, breakout_length);

	//delay
	sleep_ms(3000);
	
	while(true)
	{
		if(gpio_get(16))

		{

			chip->keypad[4] = true;	
			printf("16 is pressed!\n");

		} else {
			
			chip->keypad[4] = false;
			

		}
		
		if(gpio_get(17))
		{

			chip->keypad[6] = true;	
			printf("17 is pressed!\n");

		} else {
			
			chip->keypad[6] = false;

		}

		chip->cycle();

		if(chip->delay != 0)
		{
			chip->delay--;

		}
			
		if(chip->sound != 0)
		{
			chip->sound--;

		}

		sleep_us(2000);

		if(chip->drawFlag)
		{
			DrawFrame(&Canvas, START_X-1, START_Y-1, 64*SCALE_X+2, 32*SCALE_Y+2, COL_WHITE);
			printf("\nin sprite loop \n\n");

			int pixel = 0;
			
			int draw_y = START_Y;

			for(int y = 0; y < 32; y++)
			{
				int draw_x = START_X;
				
				for(int x = 0; x < 64; x++)
				{

					if(chip->framebuffer[x+(y*64)])
					{
						pixel = COL_WHITE;
					} else {
						pixel = COL_BLACK;
					}

					//printf("pixel drawn at x: %d y:%d color: %d\n", x, y, pixel);
					DrawRect(&Canvas, draw_x, draw_y, SCALE_X, SCALE_Y, pixel);
					
					draw_x += SCALE_X;

				}
		
				draw_y += SCALE_Y;

			}


		}

	}

}
