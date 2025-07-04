// ADS1256 cpp file
// Modified by Michael Ferrell to add ESP32-S3 support

#include "ADS1256.h"

#define convertSigned24BitToLong(value) ((value) & (1l << 23) ? (value) - 0x1000000 : value)

//Constructor
ADS1256::ADS1256(SPIClass *spi, const byte DRDY_pin, const byte CS_pin, float VREF, const byte RESET_pin, const byte SYNC_pin)
{
    _spi = spi;        // Store SPI instance
    _DRDY_pin = DRDY_pin;
    pinMode(_DRDY_pin, INPUT);

    _CS_pin = CS_pin;
    pinMode(_CS_pin, OUTPUT);

    if (RESET_pin != -1) {
        _RESET_pin = RESET_pin;
        pinMode(_RESET_pin, OUTPUT);
    } else {
        _RESET_pin = 0;
    }

    if (SYNC_pin != -1) {
        _SYNC_pin = SYNC_pin;
        pinMode(_SYNC_pin, OUTPUT);
    } else {
        _SYNC_pin = 0;
    }

    _VREF = VREF;
}
	
//Initialization
void ADS1256::InitializeADC()
{
    digitalWrite(_CS_pin, LOW);

    // Only reset if the pin is defined
    if (_RESET_pin) {
        digitalWrite(_RESET_pin, LOW);
        delay(200);
        digitalWrite(_RESET_pin, HIGH);
        delay(1000);
    }

    // Only sync if the pin is defined
    if (_SYNC_pin) {
        digitalWrite(_SYNC_pin, HIGH);
    }

    // Set default register values
    _STATUS = 0b00110110;
    writeRegister(STATUS_REG, _STATUS);
    delay(200);

    _MUX = 0b00000001;
    writeRegister(MUX_REG, _MUX);
    delay(200);

    _ADCON = 0b00000000;
    writeRegister(ADCON_REG, _ADCON);
    delay(200);

    _DRATE = 0b10000010; // Default to 100 SPS
    writeRegister(DRATE_REG, _DRATE);
    delay(200);

    sendDirectCommand(SELFCAL); // Self-calibration
    delay(200);

    _isAcquisitionRunning = false;
}

void ADS1256::waitForDRDY()
{		
	while (digitalRead(_DRDY_pin) == HIGH) {}	
}

void ADS1256::stopConversion() //Sending SDATAC to stop the continuous conversion
{	
	waitForDRDY(); //SDATAC should be called after DRDY goes LOW (p35. Figure 33)
	_spi->transfer(0b00001111); //Send SDATAC to the ADC	
	digitalWrite(_CS_pin, HIGH); //We finished the command sequence, so we switch it back to HIGH
	_spi->endTransaction();
	
	_isAcquisitionRunning = false; //Reset to false, so the MCU will be able to start a new conversion
}

void ADS1256::setDRATE(uint8_t drate) //Setting DRATE (sampling frequency)
{		
	writeRegister(DRATE_REG, drate);
	_DRATE = drate;
	delay(200);
}

void ADS1256::setMUX(uint8_t mux) //Setting MUX (input channel)
{	
	writeRegister(MUX_REG, mux);
	_MUX = mux;
	delay(200);
}

void ADS1256::setPGA(uint8_t pga) //Setting PGA (input voltage range)
{	
	_PGA = pga;
	_ADCON = readRegister(ADCON_REG); //Read the most recent value of the register
	
	_ADCON = (_ADCON & 0b11111000) | (_PGA & 0b00000111); // Clearing and then setting bits 2-0 based on pga
	
	writeRegister(ADCON_REG, _ADCON);	
	delay(200);
}

uint8_t ADS1256::getPGA() //Reading PGA from the ADCON register
{
	uint8_t pgaValue = 0;
	
	pgaValue = readRegister(ADCON_REG) & 0b00000111; 
	//Reading the ADCON_REG and keeping the first three bits.
	
	return(pgaValue);	
}

void ADS1256::setCLKOUT(uint8_t clkout) //Setting CLKOUT 
{
	_ADCON = readRegister(ADCON_REG); //Read the most recent value of the register
	
	//Values: 0, 1, 2, 3
	
	if(clkout == 0)
	{
		//00
		bitWrite(_ADCON, 6, 0);
		bitWrite(_ADCON, 5, 0);		
	}
	else if(clkout == 1)
	{
		//01 (default)	
		bitWrite(_ADCON, 6, 0);
		bitWrite(_ADCON, 5, 1);			
	}
	else if(clkout == 2)
	{
		//10
		bitWrite(_ADCON, 6, 1);
		bitWrite(_ADCON, 5, 0);		
	}
	else if(clkout == 3)
	{
		//11	
		bitWrite(_ADCON, 6, 1);
		bitWrite(_ADCON, 5, 1);				
	}
	else{}
	
	writeRegister(ADCON_REG, _ADCON);
	delay(100);
}

void ADS1256::setSDCS(uint8_t sdcs) //Setting SDCS
{
	_ADCON = readRegister(ADCON_REG); //Read the most recent value of the register
	
	//Values: 0, 1, 2, 3
	
	if(sdcs == 0)
	{
		//00 (default)
		bitWrite(_ADCON, 4, 0);
		bitWrite(_ADCON, 3, 0);		
	}
	else if(sdcs == 1)
	{
		//01	
		bitWrite(_ADCON, 4, 0);
		bitWrite(_ADCON, 3, 1);		
	}
	else if(sdcs == 2)
	{
		//10
		bitWrite(_ADCON, 4, 1);
		bitWrite(_ADCON, 3, 0);		
	}
	else if(sdcs == 3)
	{
		//11
		bitWrite(_ADCON, 4, 1);
		bitWrite(_ADCON, 3, 1);				
	}
	else{}
	
	writeRegister(ADCON_REG, _ADCON);
	delay(100);
}

void ADS1256::setByteOrder(uint8_t byteOrder) //Setting byte order (MSB/LSB)
{
	_STATUS = readRegister(STATUS_REG); //Read the most recent value of the register
	
	if(byteOrder == 0)
	{
		//Byte order is MSB (default)
		bitWrite(_STATUS, 3, 0);
		//Set value of _STATUS at the third bit to 0
	}
	else if(byteOrder == 1)
	{
		//Byte order is LSB
		bitWrite(_STATUS, 3, 1);
		//Set value of _STATUS at the third bit to 1
	}
	else{}
	
	writeRegister(STATUS_REG, _STATUS);
	delay(100);
}

void ADS1256::getByteOrder() //Getting byte order (MSB/LSB)
{	
	uint8_t statusValue = readRegister(STATUS_REG);	//Read the whole STATUS register	
	
	if(bitRead(statusValue, 3) == 0) //Read bit 1 and print the corresponding message
	{
		//Byte order is MSB (default)
		Serial.println("Byte order is MSB (default)");
	}
	else
	{
		//Byte order is LSB
		Serial.println("Byte order is LSB");
	}
}

void ADS1256::setAutoCal(uint8_t acal) //Setting ACAL (Automatic SYSCAL)
{	
	_STATUS = readRegister(STATUS_REG); //Read the most recent value of the register
	
	if(acal == 0)
	{
		//Auto-calibration is disabled (default)
		bitWrite(_STATUS, 2, 0);
		//_STATUS |= B00000000;
	}
	else if(acal == 1)
	{
		//Auto-calibration is enabled
		bitWrite(_STATUS, 2, 1);
		//_STATUS |= B00000100;
	}
	else{}
	
	writeRegister(STATUS_REG, _STATUS);
	delay(100);
}

void ADS1256::getAutoCal() //Getting ACAL (Automatic SYSCAL)
{	
	uint8_t statusValue = readRegister(STATUS_REG);	//Read the whole STATUS register	
	
	if(bitRead(statusValue, 2) == 0) //Read bit 1 and print the corresponding message
	{
		//Auto-calibration is disabled (default)
		Serial.println("ACAL is disabled!");
	}
	else
	{
		//Auto-calibration is enabled
		Serial.println("ACAL is enabled!");
	}
}

void ADS1256::setBuffer(uint8_t bufen) //Setting input buffer (Input impedance)
{	
	_STATUS = readRegister(STATUS_REG); //Read the most recent value of the register
	
	if(bufen == 0)
	{
		//Analog input buffer is disabled (default)
		//_STATUS |= B00000000;
		bitWrite(_STATUS, 1, 0);
	}
	else if(bufen == 1)
	{
		//Analog input buffer is enabled (recommended)
		//_STATUS |= B00000010;
		bitWrite(_STATUS, 1, 1);
	}
	else{}
	
	writeRegister(STATUS_REG, _STATUS);
	delay(100);
}

void ADS1256::getBuffer() //Getting input buffer (Input impedance)
{	
	uint8_t statusValue = readRegister(STATUS_REG);	//Read the whole STATUS register	
	
	if(bitRead(statusValue, 1) == 0) //Read bit 1 and print the corresponding message
	{
		//Analog input buffer is disabled (default)
		Serial.println("Buffer is disabled!");
	}
	else
	{
		//Analog input buffer is enabled (recommended)
		Serial.println("Buffer is enabled!");
	}
}

void ADS1256::setGPIO(uint8_t dir0, uint8_t dir1, uint8_t dir2, uint8_t dir3) //Setting GPIO
{	
	_GPIO = readRegister(IO_REG); //Read the most recent value of the register
	
	//Default: 11100000 - DEC: 224 - Ref: p32 I/O section
	//Sets D3-D0 as input or output
	uint8_t GPIO_bit7, GPIO_bit6, GPIO_bit5, GPIO_bit4;
	
	//Bit7: DIR3
	if(dir3 == 1)
	{
		GPIO_bit7 = 1; //D3 is input (default)
	}
	else
	{
		GPIO_bit7 = 0; //D3 is output
	}	
	bitWrite(_GPIO, 7, GPIO_bit7);
	//-----------------------------------------------------
	//Bit6: DIR2
	if(dir2 == 1)
	{
		GPIO_bit6 = 1; //D2 is input (default)
	}
	else
	{
		GPIO_bit6 = 0; //D2 is output
	}
	bitWrite(_GPIO, 6, GPIO_bit6);
	//-----------------------------------------------------
	//Bit5: DIR1 
	if(dir1 == 1)
	{
		GPIO_bit5 = 1; //D1 is input (default) 
	}
	else
	{
		GPIO_bit5 = 0; //D1 is output
	}
	bitWrite(_GPIO, 5, GPIO_bit5);
	//-----------------------------------------------------
	//Bit4: DIR0
	if(dir0 == 1)
	{
		GPIO_bit4 = 1; //D0 is input
	}
	else
	{
		GPIO_bit4 = 0; //D0 is output (default)
	}
	bitWrite(_GPIO, 4, GPIO_bit4);
	//-----------------------------------------------------
	
	writeRegister(IO_REG, _GPIO);
	delay(100);
}

void ADS1256::writeGPIO(uint8_t dir0value, uint8_t dir1value, uint8_t dir2value, uint8_t dir3value) //Writing GPIO
{
	_GPIO = readRegister(IO_REG);
	
	//Sets D3-D0 output values
	//It is important that first one must use setGPIO, then writeGPIO
	
	uint8_t GPIO_bit3, GPIO_bit2, GPIO_bit1, GPIO_bit0;
	
	//Bit3: DIR3 
	if(dir3value == 1)
	{
		GPIO_bit3 = 1;
	}
	else
	{
		GPIO_bit3 = 0;
	}	
	bitWrite(_GPIO, 3, GPIO_bit3);
	//-----------------------------------------------------
	//Bit2: DIR2 
	if(dir2value == 1)
	{
		GPIO_bit2 = 1;
	}
	else
	{
		GPIO_bit2 = 0;
	}	
	bitWrite(_GPIO, 2, GPIO_bit2);
	//-----------------------------------------------------
	//Bit1: DIR1 
	if(dir1value == 1)
	{
		GPIO_bit1 = 1;
	}
	else
	{
		GPIO_bit1 = 0;
	}
	bitWrite(_GPIO, 1, GPIO_bit1);	
	//-----------------------------------------------------
	//Bit0: DIR0 
	if(dir0value == 1)
	{
		GPIO_bit0 = 1;
	}
	else
	{
		GPIO_bit0 = 0;
	}	
	bitWrite(_GPIO, 0, GPIO_bit0);
	//-----------------------------------------------------	
		
	writeRegister(IO_REG, _GPIO);
	delay(100);
}

uint8_t ADS1256::readGPIO(uint8_t gpioPin) //Reading GPIO
{	
	uint8_t GPIO_bit3, GPIO_bit2, GPIO_bit1, GPIO_bit0, GPIO_return;
	
	_GPIO = readRegister(IO_REG); //Read the GPIO register
	
	//Save each bit values in a variable
	GPIO_bit3 = bitRead(_GPIO, 3);
	GPIO_bit2 = bitRead(_GPIO, 2);
	GPIO_bit1 = bitRead(_GPIO, 1);
	GPIO_bit0 = bitRead(_GPIO, 0);	
	
	delay(100);	
	
	switch(gpioPin) //Selecting which value should be returned
	{
		case 0:
		GPIO_return = GPIO_bit0;
		break;
		
		case 1:
		GPIO_return = GPIO_bit1;
		break;
		
		case 2:
		GPIO_return = GPIO_bit2;
		break;
		
		case 3:
		GPIO_return = GPIO_bit3;
		break;
	}

	return GPIO_return;
	
}

void ADS1256::sendDirectCommand(uint8_t directCommand)
{
  //Direct commands can be found in the datasheet Page 34, Table 24.
  _spi->beginTransaction(SPISettings(1920000, MSBFIRST, SPI_MODE1));

  digitalWrite(_CS_pin, LOW); //REF: P34: "CS must stay low during the entire command sequence"
  delayMicroseconds(5);
  _spi->transfer(directCommand); //Send Command
  delayMicroseconds(5);
  digitalWrite(_CS_pin, HIGH); //REF: P34: "CS must stay low during the entire command sequence"

  _spi->endTransaction();
}


float ADS1256::convertToVoltage(int32_t rawData) //Converting the 24-bit data into a voltage value
{
  float voltage = ((2 * _VREF) / 8388608) * rawData / (pow(2, _PGA)); //8388608 = 2^{23} - 1
  //REF: p23, Table 16.
	
  return(voltage);
}

void ADS1256::writeRegister(uint8_t registerAddress, uint8_t registerValueToWrite)
{	
  waitForDRDY();

  _spi->beginTransaction(SPISettings(1920000, MSBFIRST, SPI_MODE1));
  //SPI_MODE1 = output edge: rising, data capture: falling; clock polarity: 0, clock phase: 1.

  digitalWrite(_CS_pin, LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]

  delayMicroseconds(5); //see t6 in the datasheet

  _spi->transfer(0x50 | registerAddress); // 0x50 = 01010000 = WREG

  _spi->transfer(0x00); //2nd (empty) command byte

  _spi->transfer(registerValueToWrite); //pass the value to the register
  
  digitalWrite(_CS_pin, HIGH);
  _spi->endTransaction();
  delay(100);
  
}

long ADS1256::readRegister(uint8_t registerAddress) //Reading a register
{
   waitForDRDY();
	
  _spi->beginTransaction(SPISettings(1920000, MSBFIRST, SPI_MODE1));
  //SPI_MODE1 = output edge: rising, data capture: falling; clock polarity: 0, clock phase: 1.

  digitalWrite(_CS_pin, LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]

  _spi->transfer(0x10 | registerAddress); //0x10 = 0001000 = RREG - OR together the two numbers (command + address)

  _spi->transfer(0x00); //2nd (empty) command byte

  delayMicroseconds(5); //see t6 in the datasheet

  _registerValuetoRead = _spi->transfer(0xFF); //read out the register value

  digitalWrite(_CS_pin, HIGH);
  _spi->endTransaction();
  delay(100);
  return _registerValuetoRead;
}


long ADS1256::readSingle() //Reading a single value ONCE using the RDATA command 
{
	_spi->beginTransaction(SPISettings(1920000, MSBFIRST, SPI_MODE1));
	digitalWrite(_CS_pin, LOW); //REF: P34: "CS must stay low during the entire command sequence"  
	waitForDRDY();
	_spi->transfer(0b00000001); //Issue RDATA (0000 0001) command
	delayMicroseconds(7); //Wait t6 time (~6.51 us) REF: P34, FIG:30.

	_outputBuffer[0] = _spi->transfer(0); // MSB
	_outputBuffer[1] = _spi->transfer(0); // Mid-byte
	_outputBuffer[2] = _spi->transfer(0); // LSB		

	//Shifting and combining the above three items into a single, 24-bit number
	_outputValue = ((long)_outputBuffer[0]<<16) | ((long)_outputBuffer[1]<<8) | (_outputBuffer[2]);
	_outputValue = convertSigned24BitToLong(_outputValue);
	
	digitalWrite(_CS_pin, HIGH); //We finished the command sequence, so we set CS to HIGH
	_spi->endTransaction();
  
	return(_outputValue);
}

long ADS1256::readSingleContinuous() //Reads the recently selected input channel using RDATAC
{		
	if(_isAcquisitionRunning == false)
	{
	  _isAcquisitionRunning = true;
	  _spi->beginTransaction(SPISettings(1920000, MSBFIRST, SPI_MODE1));
	  digitalWrite(_CS_pin, LOW); //REF: P34: "CS must stay low during the entire command sequence"	  
	  waitForDRDY();
	  _spi->transfer(0b00000011);  //Issue RDATAC (0000 0011) 
	  delayMicroseconds(7); //Wait t6 time (~6.51 us) REF: P34, FIG:30.	  
	}
	else
	{
		waitForDRDY();
	}	
	
	_outputBuffer[0] = _spi->transfer(0); // MSB 
	_outputBuffer[1] = _spi->transfer(0); // Mid-byte
	_outputBuffer[2] = _spi->transfer(0); // LSB	 
	
	_outputValue = ((long)_outputBuffer[0]<<16) | ((long)_outputBuffer[1]<<8) | (_outputBuffer[2]);
	_outputValue = convertSigned24BitToLong(_outputValue);	
	
	return _outputValue;
}

long ADS1256::cycleSingle()
{
	if(_isAcquisitionRunning == false)
	{
	  _isAcquisitionRunning = true;
	  _cycle = 0;
	  _spi->beginTransaction(SPISettings(1920000, MSBFIRST, SPI_MODE1));
	  digitalWrite(_CS_pin, LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]
	  _spi->transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
      _spi->transfer(0x00);
      _spi->transfer(SING_0); //AIN0+AINCOM
	  digitalWrite(_CS_pin, HIGH);			
	  delay(50);
	  digitalWrite(_CS_pin, LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]
	}
	else
	{}
  
	if(_cycle < 8)
	{      
	  _outputValue = 0;
	  waitForDRDY();
      //Step 1. - Updating MUX
      switch (_cycle)
      {
        //Channels are written manually
        case 0: //Channel 2
          _spi->transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
          _spi->transfer(0x00);
          _spi->transfer(SING_1);  //AIN1+AINCOM
          break;

        case 1: //Channel 3
          _spi->transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
          _spi->transfer(0x00);
          _spi->transfer(SING_2);  //AIN2+AINCOM
          break;

        case 2: //Channel 4
          _spi->transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
          _spi->transfer(0x00);
          _spi->transfer(SING_3);  //AIN3+AINCOM
          break;

        case 3: //Channel 5
          _spi->transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
          _spi->transfer(0x00);
          _spi->transfer(SING_4);  //AIN4+AINCOM
          break;

        case 4: //Channel 6
          _spi->transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
          _spi->transfer(0x00);
          _spi->transfer(SING_5);  //AIN5+AINCOM
          break;

        case 5: //Channel 7
          _spi->transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
          _spi->transfer(0x00);
          _spi->transfer(SING_6);  //AIN6+AINCOM
          break;

        case 6: //Channel 8
          _spi->transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
          _spi->transfer(0x00);
          _spi->transfer(SING_7);  //AIN7+AINCOM
          break;

        case 7: //Channel 1
          _spi->transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
          _spi->transfer(0x00);
          _spi->transfer(SING_0); //AIN0+AINCOM
          break;
      }
      //Step 2.
      _spi->transfer(0b11111100); //SYNC
      delayMicroseconds(4); //t11 delay 24*tau = 3.125 us //delay should be larger, so we delay by 4 us
      _spi->transfer(0b11111111); //WAKEUP

      //Step 3.
      //Issue RDATA (0000 0001) command
      _spi->transfer(0b00000001);
      delayMicroseconds(7); //Wait t6 time (~6.51 us) REF: P34, FIG:30.

	  _outputBuffer[0] = _spi->transfer(0x0F); // MSB 
	  _outputBuffer[1] = _spi->transfer(0x0F); // Mid-byte
	  _outputBuffer[2] = _spi->transfer(0x0F); // LSB
		
	  _outputValue = ((long)_outputBuffer[0]<<16) | ((long)_outputBuffer[1]<<8) | (_outputBuffer[2]);
	  _outputValue = convertSigned24BitToLong(_outputValue);
		
	  _cycle++; //Increase cycle - This will move to the next MUX input channel
	  if(_cycle == 8)
	  {
		  _cycle = 0; //Reset to 0 - Restart conversion from the 1st input channel
	  }
    }	
  
	return _outputValue;
}

long ADS1256::cycleDifferential() 
{
	if(_isAcquisitionRunning == false)
	{
	  _cycle = 0;
	  _isAcquisitionRunning = true;
	  _spi->beginTransaction(SPISettings(1920000, MSBFIRST, SPI_MODE1));
	  
	  //Set the AIN0+AIN1 as inputs manually
	  digitalWrite(_CS_pin, LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]
	  _spi->transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
	  _spi->transfer(0x00);
	  _spi->transfer(DIFF_0_1);  //AIN0+AIN1
	  digitalWrite(_CS_pin, HIGH);	  
	  delay(50);
	  digitalWrite(_CS_pin, LOW); //CS must stay LOW during the entire sequence [Ref: P34, T24]
	}
	else
	{}
	
	if(_cycle < 4)
    {
	  _outputValue = 0;
      //DRDY has to go low
	  waitForDRDY();

      //Step 1. - Updating MUX
      switch (_cycle)
      {
        case 0: //Channel 2
          _spi->transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
          _spi->transfer(0x00);
          _spi->transfer(DIFF_2_3);  //AIN2+AIN3
          break;

        case 1: //Channel 3
          _spi->transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
          _spi->transfer(0x00);
          _spi->transfer(DIFF_4_5); //AIN4+AIN5
          break;

        case 2: //Channel 4
          _spi->transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
          _spi->transfer(0x00);
          _spi->transfer(DIFF_6_7); //AIN6+AIN7
          break;

        case 3: //Channel 1
          _spi->transfer(0x50 | 1); // 0x50 = WREG //1 = MUX
          _spi->transfer(0x00);
          _spi->transfer(DIFF_0_1); //AIN0+AIN1
          break;
      }

      _spi->transfer(0b11111100); //SYNC
      delayMicroseconds(4); //t11 delay 24*tau = 3.125 us //delay should be larger, so we delay by 4 us
      _spi->transfer(0b11111111); //WAKEUP

      //Step 3.
      _spi->transfer(0b00000001); //Issue RDATA (0000 0001) command
      delayMicroseconds(7); //Wait t6 time (~6.51 us) REF: P34, FIG:30.

	
	  _outputBuffer[0] = _spi->transfer(0); // MSB 
	  _outputBuffer[1] = _spi->transfer(0); // Mid-byte
	  _outputBuffer[2] = _spi->transfer(0); // LSB
		
	  _outputValue = ((long)_outputBuffer[0]<<16) | ((long)_outputBuffer[1]<<8) | (_outputBuffer[2]);
	  _outputValue = convertSigned24BitToLong(_outputValue);
		
	  _cycle++;
	  if(_cycle == 4)
	  {
		  _cycle = 0;
		  //After the 4th cycle, we reset to zero so the next iteration reads the 1st MUX again
	  }
    }	
  
	return _outputValue;
}



