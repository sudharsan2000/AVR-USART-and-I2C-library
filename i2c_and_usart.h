/*
Author Name : Sudharsan S
Functions: add_time(), BCDtodecimal(),decimaltoBCD(),calculate_time_difference(),i2c_init(),
i2c_read(), i2c_send_address_read(), i2c_send_address_write(),i2c_send_addressordata(),i2c_start(),
i2c_stop(),iszero(),read(),RTC_read(),RTC_write(),usart_init(),usart_print(),write()
Global Variables: current,new_
 */
#define RTC_address_read 0b11010001 // Read and write address of the RTC module to be sent through I2C

#define RTC_address_write 0b11010000

#define sec_address 0x00 // Address to be sent through I2C for retriving time stored in the RTC's memory

#define min_address 0x01

#define hour_address 0x02

#define day_address 0x04

#define month_address 0x05

#define year_address 0x06
// A structure is used for holding complete details of time such as month,year,date,minutes etc. 
typedef struct type_time
{
	uint8_t secs, mins, hours, day, month, year;
};
type_time current,new_;
/*
  Function name: add_time(t1,t2):
  Arguments: 2 variables of type_time 
  Output: Returns addition of the 2 Arguments
  Example call: added_time = add_time(current,timer);
 */
type_time add_time(type_time t1, type_time t2)
{
  type_time added_time;
  added_time.hours = t1.hours + t2.hours;
  added_time.mins = t1.mins + t2.mins;
  added_time.secs = t1.secs + t2.secs;
  if (added_time.secs >= 60)
  {
    ++added_time.mins;
    added_time.secs -= 60;
  }
  if (added_time.mins >= 60)
  {
    ++added_time.hours;
    added_time.mins -= 60;
  }
  if (added_time.hours < 0)
  {
    added_time.hours += 24;
  }
  return added_time;
}
/*
  Function name: calculate_time_difference(t1,t2):
  Arguments: 2 variables of type_time 
  Output: Returns difference between the two times passsed as Arguments
  Example call: time_left = calculate_time_difference(alarm,current);
 */
type_time calculate_time_difference(type_time t1, type_time t2)
{
  type_time time_difference;
  time_difference.hours = t1.hours - t2.hours - 1;
  time_difference.mins = t1.mins + (60 - t2.mins) - 1;
  time_difference.secs = t1.secs + (60 - t2.secs);
  if (time_difference.secs >= 60)
  {
    ++time_difference.mins;
    time_difference.secs -= 60;
  }
  if (time_difference.mins >= 60)
  {
    ++time_difference.hours;
    time_difference.mins -= 60;
  }
  if (time_difference.hours < 0)
  {
    time_difference.hours += 24;
  }
  return time_difference;
}
/*
  Function name: iszero(t)
  Arguments: 1 variable of type_time
  Logic : Tests if the passed argument of type_time contains mins,secs and hours as zero 
  Output: Returns 1(true) or 0(false)
  Example call: if(iszero(time_left))
 */
int iszero(type_time t)
{
  if(t.secs==0 && t.mins==0 && t.hours==0)
  {
    return 1;
  }
  return 0;
}
/*
  Function name: BCDtodecimal(bcd)
  Arguments: 1 variables of type int  
  Logic: Since data in the memory of RTC is stored as BCD(binary-coded decimal) it has to  
  be converted to decimal form for ease of use. So this function arithmetically converts 
  BCD to decimal. This is used when data is read from RTC.
  Output: Returns an integer equivalent in decimal form
  Example call: bcd_equiv = BCDtodecimal(bcd_time)
 */
uint8_t BCDtodecimal(uint8_t bcd)
{
  return (((bcd & 0xF0) >> 4) * 10) + (bcd & 0x0F);
}
/*
  Function name: decimaltoBCD(decimal)
  Arguments: 1 variables of type int  
  Logic: Since data in the memory of RTC is stored as BCD(binary-coded decimal) it has to  
  be converted to decimal form for ease of use. So this function arithmetically converts 
  decimal to BCD. This is used when data is read from RTC.
  Output: Returns an integer equivalent in BCD form
  Example call: decimal_equiv =  decimaltoBCD(decimal_time)
 */
uint8_t decimaltoBCD(uint8_t decimal)
{
  return (((decimal / 10) << 4) | (decimal % 10));
}
/*
  Function name: i2c_init()
  Arguments: Nil
  Output: Nil
  Logic : This function is used to initialize the i2c registers. This function is called 
  only once inside main().
  Example call: i2c_init();
 */
void i2c_init()
{
  TWBR = 8;            // Setting SCL frequency to 100kHz 
  TWCR = (1 << TWEN);  // Enable i2c
  TWSR = 0x00;         // Prescalar set to 1
}
/*
  Function name: i2c_start()
  Arguments: Nil
  Output: Nil
  Logic : This function is used to write data to i2c registers for starting I2C communication. 
  Example call: i2c_start();
 */
void i2c_start()
{
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA); //Start Conditon enable
  while (!(TWCR & (1 << TWINT)))
    ;  // Polling till success of the start condition
}
/*
  Function name: i2c_stop()
  Arguments: Nil
  Output: Nil
  Logic : This function is used to write data to i2c registers for stoping the i2c communication. 
  Example call: i2c_stop();
 */
void i2c_stop()
{
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); //  Transmit STOP condition
}
/*
  Function name: i2c_send_addressordata(data)
  Arguments: One integer variable
  Output: Nil
  Logic : This function is used to write data to i2c registers for sending data or 
  address through i2c. 
  Example call: i2c_send_addressordata(day_address);
 */
void i2c_send_addressordata(uint8_t data) 
{
  TWDR = data;
  TWCR = (1 << TWINT) | (1 << TWEN); //Load DATA into TWDR Register. Clear TWINT bit in TWCR to start transmission of data
  while (!(TWCR & (1 << TWINT)))
    ; // Wait for TWINT Flag set which indicates that the data has been transmitted, and ACK has been received
}
/*
  Function name: i2c_send_address_read()
  Arguments: Nil
  Output: Nil
  Logic : This function is used to write data to i2c registers for sending address for reading from RTC
  Example call: i2c_send_address_read();
 */
void i2c_send_address_read()
{
  TWDR = RTC_address_read;           // Move value to I2C reg
  TWCR = (1 << TWINT) | (1 << TWEN); //Enable I2C and Clear Interrupt
  while (!(TWCR & (1 << TWINT)))
    ; 
}
/*
  Function name: i2c_send_address_write()
  Arguments: Nil
  Output: Nil
  Logic : This function is used to write data to i2c registers for sending address for writing 
  to memory of RTC.
  Example call: i2c_send_address_write();
 */
void i2c_send_address_write()
{
  TWDR = RTC_address_write;          //Move value to I2C reg
  TWCR = (1 << TWINT) | (1 << TWEN); //Enable I2C and Clear Interrupt
  while (!(TWCR & (1 << TWINT)))
    ; //Write Successful with TWDR Empty
}
/*
  Function name: i2c_read()
  Arguments: Nil
  Output: Integer (as BCD) read from I2C line.
  Logic : This function is used to write data to i2c registers for receiving 
  data from i2c line.
  Example call: data_rx = i2c_read();
 */
uint8_t i2c_read()
{
  TWCR = (1 << TWEN) | (1 << TWINT); //--- Enable I2C and Clear Interrupt
  while (!(TWCR & (1 << TWINT)))
    ; //--- Read successful with all data received in TWDR
  return TWDR;
}
/*
  Function name: write(address_of_data, data)
  Arguments: address_of_data and data
  Output: Nil
  Logic : This function is used to complete the entire I2C communication flow for
  writing data to the RTC.
  Example call: write(sec_address,0);
 */
void write(uint8_t address_of_data, uint8_t data)
{
  i2c_start();
  i2c_send_address_write();

  i2c_send_addressordata(address_of_data);
  i2c_send_addressordata(data);

  i2c_stop();
}
/*
  Function name: read(address_of_data)
  Arguments: address_of_data 
  Output: The data read from i2c line
  Logic : This function is used to complete the entire I2C communication flow for
  reading data from the RTC.
  Example call: data_rx = read(sec_address);
 */
uint8_t read(uint8_t address_of_data)
{
  i2c_start();
  i2c_send_address_write();

  i2c_send_addressordata(address_of_data);

  i2c_start();

  i2c_send_address_read();
  uint8_t data_rx = i2c_read();

  i2c_stop();

  return data_rx;
}
/*
  Function name: RTC_write()
  Arguments: Nil
  Output: Nil
  Logic : This function is used to write all the components of an object of type_time.
  Example call: RTC_write();
 */
void RTC_write()
{
  write(sec_address, decimaltoBCD(0));
  write(min_address, decimaltoBCD(new_.mins));
  write(hour_address, decimaltoBCD(new_.hours));
  write(day_address, decimaltoBCD(new_.day));
  write(month_address, decimaltoBCD(new_.month));
  write(year_address, decimaltoBCD(new_.year));
}
/*
  Function name: RTC_read()
  Arguments: Nil
  Output: Nil
  Logic : This function is used to read all the components of an object of type_time from RTC.
  Example call: RTC_read();
 */
void RTC_read()
{
  current.secs = BCDtodecimal(read(sec_address));
  current.mins = BCDtodecimal(read(min_address));
  current.hours = BCDtodecimal(read(hour_address));
  current.day = BCDtodecimal(read(day_address));
  current.month = BCDtodecimal(read(month_address));
  current.year = BCDtodecimal(read(year_address));
}
/*
  Function name: usart_init()
  Arguments: Nil
  Output: Nil
  Logic : This function is used to initialize the USART registers. This function is called 
  only once inside main().
  Example call: usart_init();
 */
void usart_init()
{
  UCSR0A = 0X00;
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
  UBRR0 = 103;                              //Set baud rate to 9600 
}
/*
  Function name: usart_print()
  Arguments: 1 string to be printed on the serial monitor
  Output: Nil
  Logic : This function is used to send data using USART.
  Example call: usart_print("Hello");
 */
void usart_print(String data)
{
  for (int i = 0; data[i] != '\0'; i++)
  {
    while (!(UCSR0A & (1 << UDRE0)))
      ;
    UDR0 = data[i];
  }
}
