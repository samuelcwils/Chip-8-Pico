#include "chip8.h"

void chip8::initialize()
{
	opcode = 0; //sets to zero
	I = 0; // sets index register to zero
	pc = 512; //starts program counter at 0x200
	sp = 0;
	drawFlag = 0;

	printf("inital opcode is: %u\n", opcode);
	printf("inital I is: %u\n", I);	
	printf("inital pc is: %u\n", pc);
	printf("inital sp is: %u\n", sp);
	printf("inital drawFlag is: %i\n", drawFlag);

	memory.fill(0); //sets memory
	V.fill(0); // sets registers
	keypad.fill(0);
	framebuffer.fill(0); //sets framebuffer
	
	delay = 0; //starts delay timer at 60
	sound = 0; //starts sound timer at 60

	for(int i = 0; i < 80; i++) //installs the fontset
	{
	
		memory[i] = fontset[i];
	
	}
}


bool chip8::loadROM(std::string filePath)
{
	initialize();
	
	//load file
	std::ifstream rom;
	rom.open(filePath, std::ios::binary);

	if (!rom) {
		return false;
	}

	//find length of file
	rom.seekg(0, rom.end);
	int bufferLength = rom.tellg();
	rom.seekg(0, rom.beg);


	//allocate buffer
	char *fileBuffer = ((char*)(calloc(bufferLength, sizeof(char))));

	rom.read(fileBuffer, bufferLength);

	//push each byte from rom into the memory
	for (int i = 0; i <= (bufferLength); i++)
	{
		memory[i+512] = (uint8_t)fileBuffer[i];

	}

	return true;
	rom.close();

}

bool chip8::loadROM(const unsigned char rom[], unsigned long romSize)
{
	initialize();

	for (int i = 0; i < romSize; i++)
	{
		memory[i + 512] = rom[i];
		printf("%0x = %0x\n", memory[i+512], rom[i]);

	}

	return true;
}


void chip8::cycle()
{
	opcode = memory[pc] << 8 | memory[pc + 1];
	srand(time(NULL));
	uint8_t x = (opcode & 0x0f00) >> 8; //extracts lower 4 bits of second byte;
	uint8_t y = (opcode & 0x00f0) >> 4; //extracts upper 4 bits of first byte
	uint8_t n = opcode & 0x000f; //extracts last 4 bits
	uint8_t nn = opcode & 0x00ff; //extracts last 8 bits
	uint16_t nnn = opcode & 0x0fff; //extracts last 12 bits

	drawFlag = 0;

	printf("pc is: %i\n", pc);
	printf("	%0x\n", opcode);

	switch (opcode & 0xf000) //finds the instruction group "c" from first byte
	{
		case 0x0000:
			switch (nn)
			{
				case 0xE0:
					framebuffer.fill(0);
					drawFlag = 1;
					pc += 2;
					break;

				case 0xEE:
					sp--;
					pc = stack[sp];
					pc += 2;
					break;

				default:
					std::cout << "invalid opcode\n";
					pc += 2;
					break;
			}

			break;

		case 0x1000:
			pc = nnn;
			break;

		case 0x2000:
			stack[sp] = pc;
			pc = nnn;
			sp++;
			break;

		case 0x3000:
			if (V[x] == nn) {
				pc += 4;
			}
			else {
				pc += 2;
			}
			break;

		case 0x4000:
			if (V[x] != nn) {
				pc += 4;
			}
			else {
				pc += 2;
			}
			break;

		case 0x5000:
			if (V[x] == V[y]) {
				pc += 4;
			}
			else {
				pc += 2;
			}
			break;

		case 0x6000:
			V[x] = nn;
			pc += 2;
			break;

		case 0x7000:
			V[x] += nn;
			pc += 2;
			break;

		case 0x8000:
			switch (n)
			{
				case 0x0:
					V[x] = V[y];
					pc += 2;
					break;

				case 0x1:
					V[x] = V[x] | V[y];
					pc += 2;
					break;

				case 0x2:
					V[x] = V[x] & V[y];
					pc += 2;
					break;

				case 0x3:
					V[x] = V[x] ^ V[y];
					pc += 2;
					break;

				case 0x4:
					if ((V[x] + V[y]) > 255) {
						V[0xF] = 1;
					}
					else {
						V[0xF] = 0;
					}

					V[x] += V[y];
					pc += 2;
					break;

				case 0x5:
					if ((V[x] - V[y]) < 0) {
						V[0xF] = 0;
					}
					else {
						V[0xF] = 1;
					}

					V[x] -= V[y];
					pc += 2;
					break;

				case 0x6:
					V[0xF] = V[y] & 1;
					V[x] = V[y] >> 1;
					pc += 2;
					break;

				case 0x7:
					if ((V[x] - V[y]) < 0) {
						V[0xF] = 0;
					}
					else {
						V[0xF] = 1;
					}
					V[x] = V[y] - V[x];
					pc += 2;
					break;


				case 0xE:
					V[0xF] = V[y] & 0x01;
					V[x] = V[y] << 1;
					pc += 2;
					break;

				default:
					std::cout << "invalid opcode\n";
					pc += 2;
					break;


			}
			break;

		case 0x9000:
			if (V[x] != V[y]) {
				pc += 4;
			}
			else {
				pc += 2;
			}
			break;

		case 0xA000:
			I = nnn;
			pc += 2;
			break;

		case 0xB000:
			pc = V[0] + nnn;
			break;

		case 0xC000:
			V[x] = (rand() % 255) & nn;
			pc += 2;
			break;

		case 0xD000:
			{	

				printf("	I = %0x y = %d x = %d n = %d \n", I - 512, V[y], V[x], n);
				//The index register is the current line being drawn of the sprite

				uint8_t x_pos = V[x];
				uint8_t y_pos = V[y];

				V[0xF] = 0;

				for (int line = 0; line < n; line++) //goes through each line of the sprite
				{
					uint8_t spriteLine = (((memory[I + line])));// current line of the sprite being drawn

					for (int column = 0; column < 8; column++) //goes through each column of the sprite
					{
						bool currentPixel = (spriteLine & (0x80 >> column)); //finds value of current pixel
						
						if(currentPixel) //only draws if value is positive
						{
							int pixel_x = x_pos + column;
							int pixel_y = y_pos + line;

							if (pixel_x > 0x3f)
							{
								pixel_x = pixel_x % 64;
							}


							if (pixel_y > 0x1f)
							{
								pixel_y = pixel_y % 32;
							}

							int drawingPos = ((pixel_y) * 64) + pixel_x;

							if (framebuffer[drawingPos] & !V[0xF]) //checks if a pixel was unset if the unsetFlag is off	
							{

								V[0xF] = 1;

							}

							framebuffer[drawingPos] ^= 1; //draws the pixel to the framebuffer 

						
						}					

					}

					//goes to the next line of the sprite in memory
					
					

				}

				pc += 2;
				std::cout << "	sprite drawn\n";
				drawFlag = 1; 
			}
		
			break;

		case 0xE000:
			switch(nn)
			{
				case 0x9E:
		
					if (keypad[V[x]] != 0) {
						pc += 4;
					}
					else {
						pc += 2;
					}

					break;

				case 0xA1:
					if (keypad[V[x]] == 0) {
						pc += 4;
					}
					else {
						pc += 2;
					}

					break;

				default:
					std::cout << "invalid opcode\n";
					break;

			}

			break;

		case 0xF000:
			switch(nn)
			{
		
				case 0x07:
					V[x] = delay;
					pc += 2;
					break;

				case 0x0A:
				{
					bool keyPressed = 0;

					while (!keyPressed)
					{

						for (int i = 0; i < 15; i++)
						{
							if (keypad[i]) {
								keyPressed = 1;
								V[x] = keyPressed;
							}

						}

						std::cout << "keypressed\n";

					}

					pc += 2;
					break;
				}

				case 0x15:
					delay = V[x];
					pc += 2;
					break;

				case 0x18:
					sound = V[x];
					pc += 2;
					break;

				case 0x1E:
					if(I+V[x] > 255){
						V[0xF] = 1;
					}

					I += V[x];
					pc += 2;
					break;

				case 0x29:
					I = V[x] * 5;
					pc += 2;
					break;

				case 0x33:
					memory[I] = V[x] / 100;
					memory[I+1] = (V[x] / 10) % 10;
					memory[I+2] = V[x] % 10;
					pc += 2;
					break;

				case 0x55:
					for(int i = 0; i <= x; i++)
					{
						memory[I+i] = V[i];
					}

					I += x + 1;
					pc += 2;
					break;

				case 0x65:
					for (int i = 0; i <= x; i++)
					{
						V[i] = memory[I + i];
					}

					I += x + 1;
					pc += 2;
					break;

				default:
					std::cout << "invalid opcode\n";
					break;


			}

			break;
		



		default:
			std::cout << "invalid opcode\n";
			break;
		

	} 

}


