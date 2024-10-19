#include <KellyEInk_42_SSD1683_BW_2BIT.h>

KellyEInk_42_SSD1683_BW_2BIT::KellyEInk_42_SSD1683_BW_2BIT(int16_t busy, int16_t rst, int16_t dc, int16_t cs, int16_t clk, int16_t din): KellyEInk(busy, rst, dc, cs, clk, din, 400, 300) {
	
}

const unsigned char KellyEInk_42_SSD1683_BW_2BIT::LUT_TABLE_LUT_4gray_all[233] PROGMEM={							
    0x01,	0x0A,	0x1B,	0x0F,	0x03,	0x01,	0x01,	
    0x05,	0x0A,	0x01,	0x0A,	0x01,	0x01,	0x01,	
    0x05,	0x08,	0x03,	0x02,	0x04,	0x01,	0x01,	
    0x01,	0x04,	0x04,	0x02,	0x00,	0x01,	0x01,	
    0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
    0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
    0x01,	0x0A,	0x1B,	0x0F,	0x03,	0x01,	0x01,	
    0x05,	0x4A,	0x01,	0x8A,	0x01,	0x01,	0x01,	
    0x05,	0x48,	0x03,	0x82,	0x84,	0x01,	0x01,	
    0x01,	0x84,	0x84,	0x82,	0x00,	0x01,	0x01,	
    0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
    0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
    0x01,	0x0A,	0x1B,	0x8F,	0x03,	0x01,	0x01,	
    0x05,	0x4A,	0x01,	0x8A,	0x01,	0x01,	0x01,	
    0x05,	0x48,	0x83,	0x82,	0x04,	0x01,	0x01,	
    0x01,	0x04,	0x04,	0x02,	0x00,	0x01,	0x01,	
    0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
    0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
    0x01,	0x8A,	0x1B,	0x8F,	0x03,	0x01,	0x01,	
    0x05,	0x4A,	0x01,	0x8A,	0x01,	0x01,	0x01,	
    0x05,	0x48,	0x83,	0x02,	0x04,	0x01,	0x01,	
    0x01,	0x04,	0x04,	0x02,	0x00,	0x01,	0x01,	
    0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
    0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
    0x01,	0x8A,	0x9B,	0x8F,	0x03,	0x01,	0x01,	
    0x05,	0x4A,	0x01,	0x8A,	0x01,	0x01,	0x01,	
    0x05,	0x48,	0x03,	0x42,	0x04,	0x01,	0x01,	
    0x01,	0x04,	0x04,	0x42,	0x00,	0x01,	0x01,	
    0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
    0x01,	0x00,	0x00,	0x00,	0x00,	0x01,	0x01,	
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
    0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	
    0x02,	0x00,	0x00,	0x07,	0x17,	0x41,	0xA8,	
    0x32,	0x30,						
};	


/*	
	Set partial data for display RAM

	if Image is NULL - clears area by background color (invers = true - turn every pixel ON)
*/
void KellyEInk_42_SSD1683_BW_2BIT::displayPartial(const unsigned char *Image, unsigned int  Xstart, unsigned int  Ystart, unsigned int  Xend, unsigned int  Yend, bool update, bool inverse) {
	
	partialProcess = true;
 	unsigned int i, j;
    unsigned int BytesPerLine = displayWidth / 8;

    // Переводим координаты в байты
    unsigned int ByteXstart = Xstart / 8;
    unsigned int ByteXend = (Xend + 7) / 8; // округляем вверх, чтобы захватить все пиксели

	displayReset();

	sendCommand(0x3C); // BorderWaveform
	sendData(0x80);

	sendCommand(0x21);
	sendData(0x00);
	sendData(0x00);

	sendCommand(0x3C);
	sendData(0x80);

	// Set memory area

	sendCommand(0x44); // set RAM x address start/end
	sendData(ByteXstart & 0xff); // RAM x address start
	sendData((ByteXend - 1) & 0xff); // RAM x address end

	sendCommand(0x45); // set RAM y address start/end
	sendData(Ystart & 0xff); // RAM y address start
	sendData((Ystart >> 8) & 0x01); // RAM y address start high byte
	sendData((Yend - 1) & 0xff); // RAM y address end
	sendData(((Yend - 1) >> 8) & 0x01); // RAM y address end high byte

	// Set memory pointer

	sendCommand(0x4E); // set RAM x address count to 0
	sendData(ByteXstart & 0xff);
	sendCommand(0x4F); // set RAM y address count to 0x127
	sendData(Ystart & 0xff);
	sendData((Ystart >> 8) & 0x01);

	sendCommand(0x24); // Write Black and White image to RAM

	for (j = Ystart; j < Yend; j++) {
		for (i = ByteXstart; i < ByteXend; i++) {
			unsigned int index = j * BytesPerLine + i;
			if (!Image) sendData(inverse ? 0x00 : 0xFF);
			else sendData(inverse ? ~Image[index] : Image[index]);
		}
	}

	if (update) {
		displayUpdate();
	}

	partialProcess = false;
}

void KellyEInk_42_SSD1683_BW_2BIT::initDisplay4Gray() {
	
    displayReset();
    readBusy();

    sendCommand(0x12);
    readBusy();

    sendCommand(0x21);
    sendData(0x00);
    sendData(0x00);

    sendCommand(0x3C);
    sendData(0x03);

    sendCommand(0x0C);
    sendData(0x8B);
    sendData(0x9C);
    sendData(0xA4);
    sendData(0x0F);

    sendCommand(0x32);
    for(int i=0; i<227; i++) {
        sendData(pgm_read_byte(&LUT_TABLE_LUT_4gray_all[i]));
	}

    sendCommand(0x3F);
    sendData(pgm_read_byte(&LUT_TABLE_LUT_4gray_all[227]));

    sendCommand(0x03);
    sendData(pgm_read_byte(&LUT_TABLE_LUT_4gray_all[228]));

    sendCommand(0x04);
    sendData(pgm_read_byte(&LUT_TABLE_LUT_4gray_all[229]));
    sendData(pgm_read_byte(&LUT_TABLE_LUT_4gray_all[230]));
    sendData(pgm_read_byte(&LUT_TABLE_LUT_4gray_all[231]));

    sendCommand(0x2c);
    sendData(pgm_read_byte(&LUT_TABLE_LUT_4gray_all[232]));

    sendCommand(0x11);
    sendData(0x03);

    sendCommand(0x44); 
    sendData(0x00);
    sendData(0x31); 

    sendCommand(0x45); 
    sendData(0x00);
    sendData(0x00); 
    sendData(0x2B);
    sendData(0x01);

    sendCommand(0x4E);
    sendData(0x00);

    sendCommand(0x4F);
    sendData(0x00);
    sendData(0x00);
    readBusy();
}			

void KellyEInk_42_SSD1683_BW_2BIT::display4Gray(const unsigned char *Image, int x, int y, int w, int l) {

    int i,j,k,m;
	int z=0;
    unsigned char temp1,temp2,temp3;

	/****Color display description****
		 white  gray1  gray2  black
	0x10|  01     01     00     00
	0x13|  01     00     01     00
	*********************************/
	
	sendCommand(0x24);
	z=0;
	x= x/8*8;
	for(m = 0; m < (int) displayHeight;m++)
		for(i=0; i < (int) displayWidth/8; i++)
		{
			if(i >= x/8 && i <(x+w)/8 && m >= y && m < y+l){
				
				temp3=0;
				for(j=0;j<2;j++)	
				{
					temp1 = pgm_read_byte(&Image[z*2+j]);
					for(k=0;k<2;k++)	
					{
						temp2 = temp1&0xC0 ;
						if(temp2 == 0xC0)
							temp3 |= 0x01;//white
						else if(temp2 == 0x00)
							temp3 |= 0x00;  //black
						else if(temp2 == 0x80) 
							temp3 |= 0x01;  //gray1
						else //0x40
							temp3 |= 0x00; //gray2
						temp3 <<= 1;	
						
						temp1 <<= 2;
						temp2 = temp1&0xC0 ;
						if(temp2 == 0xC0)  //white
							temp3 |= 0x01;
						else if(temp2 == 0x00) //black
							temp3 |= 0x00;
						else if(temp2 == 0x80)
							temp3 |= 0x01; //gray1
						else    //0x40
								temp3 |= 0x00;	//gray2	
						if(j!=1 || k!=1)				
							temp3 <<= 1;
						
						temp1 <<= 2;
					}
				}
				z++;
				sendData(temp3);
				
			}else{
				sendData(0xff);
			}				
		}
    // new  data
    sendCommand(0x26);
	z=0;
	for(m = 0; m < (int) displayHeight; m++)
		for(i=0; i < (int)  displayWidth/8; i++)
		{
			if(i >= x/8 && i <(x+w)/8 && m >= y && m < y+l){
				
				temp3=0;
				for(j=0;j<2;j++)	
				{
					temp1 = pgm_read_byte(&Image[z*2+j]);
					for(k=0;k<2;k++)	
					{
						temp2 = temp1&0xC0 ;
						if(temp2 == 0xC0)
							temp3 |= 0x01;//white
						else if(temp2 == 0x00)
							temp3 |= 0x00;  //black
						else if(temp2 == 0x80) 
							temp3 |= 0x00;  //gray1
						else //0x40
							temp3 |= 0x01; //gray2
						temp3 <<= 1;	
						
						temp1 <<= 2;
						temp2 = temp1&0xC0 ;
						if(temp2 == 0xC0)  //white
							temp3 |= 0x01;
						else if(temp2 == 0x00) //black
							temp3 |= 0x00;
						else if(temp2 == 0x80)
							temp3 |= 0x00; //gray1
						else    //0x40
								temp3 |= 0x01;	//gray2
						if(j!=1 || k!=1)					
							temp3 <<= 1;
						
						temp1 <<= 2;
					}
				}
				z++;
				sendData(temp3);	
			}else {
				sendData(0xff);	
			}
		}

	// Turn on Display
    sendCommand(0x22);
	sendData(0xCF);
    sendCommand(0x20);
    readBusy();
}


void KellyEInk_42_SSD1683_BW_2BIT::displayInit(unsigned int newBitMode, bool partialMode) {	
	
	bitMode = newBitMode;
	
	if (bitMode == 2) {
		initDisplay4Gray();
		return;
	}

	Serial.println(F("e-Paper : [displayInit]"));
	displayReset();
	readBusy();

	sendCommand(0x12); // SW RESET 
	readBusy();

	sendCommand(0x21); // Display Update Contro
	sendData(0x40);
	sendData(0x00);

	sendCommand(0x3C); // Border Waveform Contro
	sendData(0x05);

	sendCommand(0x11); // Data Entry mode setting
	sendData(0x03);

	if (!partialMode) {

		sendCommand(0x44);  // Set RAM X - address 
		sendData(0x00);
		sendData(((displayWidth / 8) - 1) & 0xff); // 0x31
		
		sendCommand(0x45); // Set RAM Y - address 
		sendData(0x00);
		sendData(0x00);  
		sendData((displayHeight - 1) & 0xff); // 0x2B
		sendData(((displayHeight - 1) >> 8) & 0x01); // 0x01

		sendCommand(0x4E);  // Make initial settings for the RAM X address in the address counter
		sendData(0x00);

		sendCommand(0x4F);  // Make initial settings for the RAM Y address in the address counter
		sendData(0x00);
		sendData(0x00);
	}

	readBusy();
		
	delay(dispInitDelay);
}

void KellyEInk_42_SSD1683_BW_2BIT::displayUpdate() {

	//  Display Update settings
	sendCommand(0x22); 

	if (partialProcess) sendData(0xFF);
	else sendData(0xF7);

	// Begin Display Update Sequence
	sendCommand(0x20);

	delay(100);
	readBusy();
}

void KellyEInk_42_SSD1683_BW_2BIT::displaySleep() {

	if (ENK_PIN_RST < 0) {
		Serial.println(F("[Sleep] fail - RST pin not available")); // we wont be able to return back form sleep without reset
		return;
	}

	sendCommand(0x10);  
	sendData(0x01);        
	delay(100);
}

/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays, for black & white displays with single buffer
parameter:
******************************************************************************/
void KellyEInk_42_SSD1683_BW_2BIT::display(const unsigned char *blackimage, const unsigned char *ryimage, bool update)
{	
	if (bitMode == 2) {
		display4Gray(blackimage, 0, 0, displayWidth, displayHeight);
		return;		
	}
	
	uint16_t Width, Height;
	Width = (displayWidth % 8 == 0)? (displayWidth / 8 ): (displayWidth / 8 + 1);
	Height = displayHeight;

	sendCommand(0x24);
	for (uint16_t j = 0; j < Height; j++) {
		for (uint16_t i = 0; i < Width; i++) {
			if (blackimage) sendData(pgm_read_byte(&blackimage[i + j * Width]));
			else sendData(FILL_BACKGROUND);
		}
	}

	sendCommand(0x26);
	
	for (uint16_t j = 0; j < Height; j++) {
		for (uint16_t i = 0; i < Width; i++) {
			if (blackimage) sendData(pgm_read_byte(&blackimage[i + j * Width])); // важно дублировать сюда для корректного partial update'a
			else sendData(FILL_BACKGROUND);
		}
	}
	
	if (update) {	
		delay(10);	
		displayUpdate();
	}
}