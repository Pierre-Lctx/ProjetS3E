FILENAME = main
FILENAME2 = functions
FILENAME3 = debug
FILENAME4 = ChainableLED
PORT = /dev/cu.usbserial-00000000
DEVICE = atmega328p
PROGRAMMER = arduino
BAUD = 115200
COMPILE = avr-gcc -Wall -Os -DF_CPU=16000000UL -mmcu=$(DEVICE)

default : compile upload clean

compile:  
		$(COMPILE) -c $(FILENAME).c -o $(FILENAME).o
		$(COMPILE) -c $(FILENAME2).c -o $(FILENAME2).o
		$(COMPILE) -c $(FILENAME3).c -o $(FILENAME3).o
		$(COMPILE) -o $(FILENAME).elf $(FILENAME).o $(FILENAME2).o $(FILENAME3).o 
		avr-objcopy -j .text -j .data -O ihex $(FILENAME).elf $(FILENAME).hex
		avr-size --format=avr --mcu=$(DEVICE) $(FILENAME).elf

upload:
		avrdude -v -p $(DEVICE) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -U flash:w:$(FILENAME).hex:i 

clean:
		rm $(FILENAME).o
		rm $(FILENAME2).o 
		rm $(FILENAME3).o 
		rm $(FILENAME).elf
		rm $(FILENAME).hex