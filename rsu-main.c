//https://www.altera.com/ja_JP/pdfs/literature/ug/ug_altremote.pdf


#include "../lib/storm_core.h"
#include "../lib/storm_soc_basic.h"
#include "../lib/uart.h"

#define INTOSCVALUE 1
#define WDPATTERN 0xFFFFFFFF

//BTADDRESS is the REAL PAGE address you want to load. (shifting is handled by the software)
#define BTADDRESS 0x00080000
#define RSUBASE 0xFFFF0500
#define RSU_WATCHDOG_TIMEOUT_VALUE_WR_MASK	( 0xFFF )
#define RSU_BOOT_ADDRESS_WR_MASK	( 0x003FFFFF )
#define RSU_BOOT_ADDRESS_SHIFT 2

//static void printf_int(char *printstring, unsigned long int num2print);
//static void printf_char(char *printstring, unsigned long int num2print);
static void rsu_factory_trigger_reconfig(unsigned long int rsu_base, unsigned long int boot_address_value, unsigned long int watch_dog_value);
static void print_help (void);
// +------------------------------+
// |    Simple Program Demo       |
// +------------------------------+
/*   void printf_int(char *printstring, unsigned long int num2print) {
	char tmpstr[10];
	
	uart0_printf(printstring);
	long_to_hex_string(num2print, tmpstr, 8);
	uart0_printf(tmpstr);
	uart0_printf("\n\r");

}
void printf_char(char *printstring, unsigned long int num2print) {
	char tmpstr[10];
	
	uart0_printf(printstring);
	long_to_hex_string(num2print, tmpstr, 2);
	uart0_printf(tmpstr);
	uart0_printf("\n\r");

} */
/*
#define RSU_BASE       (*(REG32 (0xFFFF0500)))
#define RSU_SIZE       10*4
#define RSU_MSM       (*(REG32 (0xFFFF0500)))//Master State Machine[R only]
#define RSU_ECD       (*(REG32 (0xFFFF0504)))//Force early CONF_DONE check (RW) 1 bit
#define RSU_WDV       (*(REG32 (0xFFFF0508)))//WD Timer Value (R: 29b, W:12b)
#define RSU_WDE       (*(REG32 (0xFFFF050C)))//WD enable (RW: 1 bit)
#define RSU_BAD       (*(REG32 (0xFFFF0510)))//Boot Addr (R:24b, W:22b)
//#define RSU_TX1       (*(REG32 (0xFFFF0514)))//NA
#define RSU_IOS       (*(REG32 (0xFFFF0518)))//Force internal OSC for state machine (RW: 1 bit)
#define RSU_RTC       (*(REG32 (0xFFFF051C)))//Reconfigure trigger condision (RO:5bits)
#define RSU_RCF       (*(REG32 (0xFFFF0520))) //Reconfig (WO: 1 bit)
#define RSU_RWD       (*(REG32 (0xFFFF0524))) //Reset WD (WO: 1 bit)
*/

/* ---- Main function ---- */
void print_help (void) {
	uart0_printf("0: Bootloader                           1: Read RSU Boot Addr\r\n");
	uart0_printf("2: Write RSU Boot Addr                  3: Read master State Machine\r\n");
	uart0_printf("4: Read force Early CONF_DONE check     5: Write force Early CONF_DONE check\r\n");
	uart0_printf("6: Read Watchdog Timer                  7: Write Watchdog Timer\r\n");
	uart0_printf("8: Read Watchdog Enable                 9: Write Watchdog Enable\r\n");
	uart0_printf("a: Read internal OSC for state machine  b: Write internal OSC for state machine\r\n");
	uart0_printf("c: Read Reconfigure trigger condition   d: Trigger Reconfigure\r\n");
	uart0_printf("e: Trigger reset Watchdog timer         f: Write 0x0 to read_source\r\n");
	uart0_printf("g: Write 0x1 to read_source             i: Write 0x2 to read_source\r\n");
	uart0_printf("j: Write 0x3 to read_source             k: Reconfigure PAGE1\r\n");
	uart0_printf("m: clear WD Enable                      n: Clear WD Timer\r\n");
	uart0_printf("h: Print this Menu\r\n");
}

void rsu_factory_trigger_reconfig(
			unsigned long int rsu_base,
			unsigned long int boot_address_value,		// MSB 22-bits of 24-bit address
			unsigned long int watch_dog_value			// MSB 12-bits of 29-bit timeout count
		) {

	uart0_printf("Writing the force Early CONF_DONE check\r\n");
	RSU_RSC = 0x00;
	RSU_ECD = 0x01;	

	uart0_printf("Writing the Watchdog Timer \r\n");
	RSU_RSC = 0x00;
	RSU_WDV = (watch_dog_value & RSU_WATCHDOG_TIMEOUT_VALUE_WR_MASK);
	
	if( watch_dog_value > 0 ) {
		uart0_printf("Enabling the Watchdog Timer \r\n");
		RSU_RSC = 0x00;
		RSU_WDE = 0x1;
	} else {
		uart0_printf("Disabling the Watchdog Timer \r\n");
		RSU_RSC = 0x00;
		RSU_WDE = 0x0;
	}

//	WR_RSU_BOOT_ADDRESS_REG( rsu_base, ( boot_address_value << RSU_BOOT_ADDRESS_WR_OFST ) & RSU_BOOT_ADDRESS_WR_MASK );
	uart0_printf("Writing the Watchdog Timer \r\n");
	RSU_RSC = 0x0;
	RSU_BAD = ((boot_address_value & RSU_BOOT_ADDRESS_WR_MASK)>>RSU_BOOT_ADDRESS_SHIFT);
	
//	WR_RSU_FORCE_INTERNAL_OSC_REG( rsu_base, ( 0x01 << RSU_FORCE_INTERNAL_OSC_WR_OFST ) & RSU_FORCE_INTERNAL_OSC_WR_MASK );
	uart0_printf("Enable the Interal Oscillator \r\n");
	RSU_RSC = 0x0;
	RSU_IOS = 0x1;
//	WR_RSU_CONTROL_STATUS_REG( rsu_base, ( RSU_CONTROL_STATUS_RECONFIG << RSU_CONTROL_STATUS_OFST ) & RSU_CONTROL_STATUS_MASK );
	uart0_printf("Triggering Reconfig\r\n");
	RSU_RCF = 0x1;
	while(1) uart0_printf("awaiting reconfigure");	// at this point we should undergo hardware reconfiguration

	return;
}

/*

MSM Mode
PARAM	Read	Param	Desc									bits	Run
R   W   source															mode
1 	0 	[11] 	[001] 	Read the early confdone check bits 		1 		Factory
1 	0 	[11] 	[010] 	Read watchdog time-out value 			12 		Factory
1 	0 	[11] 	[011] 	Read watchdog enable bit 				1 		Factory
1 	0 	[11] 	[100] 	Read boot address 						22 		Factory
1	0	[11] 	[110] 	Read to check whether the internal oscillator
						is set as startup state machine clock	1 		Factory
1 	0 	[10] 	[100] 	Read current application mode boot
						address 								24 		Application
1 	0 	[00] 	[100] 	Read factory boot address 				24 		Factory
1 	0 	[00] 	[000] 	Master State Machine Current State Mode
						(Read Only)
						• 00—Factory mode
						• 01—Application mode
						• 11—Application mode with master state
							machine user watchdog timer enabled	2 		Factory or
																		Application
0 	1 	[00] 	[100] 	Write application boot address.
						All parameters can be written in factory
						mode only.								22 		Factory
0 	1 	[00] 	[011] 	Write the watchdog enable bit.
						All parameters can be written in factory
						mode only.								1 		Factory
0 	1 	[00] 	[110] 	Write to force the internal oscillator as
						startup state machine clock. All parameters
						can be written in factory mode only.	1 		Factory
0 	1 	[00] 	[001] 	Write the early confdone check bit.
						All parameters can be written in factory
						mode only.								1 		Factory
0 	1 	[00]	 [010] 	Write the watchdog time-out value.
						All parameters can be written in factory
						mode only								12 		Factory
*/
int main( void ){  
    /*define a packet,of structure ARP,and lets call it arpPacket*/
	unsigned long regtemp1;
//	char  str_temp[10]="test";
	char temp;

	
	uart0_printf("\r\n\r\nSTORM SoC Basic Configuration\r\n");
	uart0_printf("Demo RSU program\r\n\r\n");

	print_help();
	/**********************************************/
	while(1){
		temp = io_uart0_read_byte();

			switch (temp) {
				case '0'  :
					uart0_printf("returning to bootloader...\n\r");
					asm volatile ("mov pc,  #0x00010000"); // jump to bootloader
					break;
				case '1'  :
					uart0_printf("Reading Current Boot Adr reg\n\r");
					RSU_RSC = 0x3;
					regtemp1 = RSU_BAD;
					printf_int("Read boot addr: ", ( regtemp1 & 0x3FFFFF) << 2 );
					break;
				case '2'  :
					uart0_printf("writing BTADDRESS to Boot Address\r\n");
					RSU_RSC = 0x0;
					RSU_BAD = BTADDRESS>>2;	
					break;
				case '3'  :
					uart0_printf("Reading Master State Machine\r\n");
					RSU_RSC = 0x0;
					regtemp1 = RSU_MSM & 0x3;	
					printf_int("Read Early Maser Sate machine: ", (regtemp1) );
					break;
				case '4'  :
					uart0_printf("Reading the force Early CONF_DONE check\r\n");
					RSU_RSC = 0x3;
					regtemp1 = RSU_ECD;
					printf_int("Read Early CONF_DONE check: ", regtemp1);
					break;
				case '5'  :
					uart0_printf("Writing the force Early CONF_DONE check\r\n");
					RSU_RSC = 0x00;
					RSU_ECD = 0x01;	
					break;
				case '6'  :
					uart0_printf("Reading Watchdog Timer\r\n");
					RSU_RSC = 0x3;
					regtemp1 = RSU_WDV;
					printf_int("Read WD value: ", regtemp1);
					break;
				case '7'  :
					uart0_printf("writing WDPATTERN to Watchdog Timer\r\n");
					RSU_RSC = 0x0;
					RSU_WDV = WDPATTERN;	
					break;
				case '8'  :
					uart0_printf("Reading Watchdog Enable\r\n");
					RSU_RSC = 0x3;
					regtemp1 = RSU_WDE;
					printf_int("Read WD Enable value: ", ( regtemp1 & 0x1) );
					break;
				case '9'  :
					uart0_printf("writing Watchdog Enable\r\n");
					RSU_RSC = 0x0;
					RSU_WDE = 0x1;	
					break;
				case 'a'  :
					uart0_printf("Reading Int OSC state\r\n");
					RSU_RSC = 0x3;
					regtemp1 = RSU_IOS;
					printf_int("Read Int OSC state: ", regtemp1);
					break;
				case 'b'  :
					uart0_printf("writing Int OSC state\r\n");
					RSU_RSC = 0x0;
					RSU_IOS = INTOSCVALUE;	
					break;
				case 'c'  :
					uart0_printf("Reading Reconfigure trigger condition\r\n");
					RSU_RSC = 0x3;
					regtemp1 = RSU_RTC;
					printf_int("Read reconfig cond: ", regtemp1);
					break;
				case 'd'  :
					uart0_printf("Triggering Reconfig\r\n");
					RSU_RCF = 0x1;	
					break;
				case 'e'  :
					uart0_printf("Reseting  Watchdog timer \r\n");
					RSU_RWD = 0x1;	
					break;
				case 'f'  :
					uart0_printf("Writing 0x0 to read_source \r\n");
					RSU_RSC = 0x0;	
					break;
					
				case 'g'  :
					uart0_printf("Writing 0x1 to read_source \r\n");
					RSU_RSC = 0x3;	
					break;
				case 'i'  :
					uart0_printf("Writing 0x2 to read_source \r\n");
					RSU_RSC = 0x3;	
					break;
				case 'j'  :
					uart0_printf("Writing 0x3 to read_source \r\n");
					RSU_RSC = 0x3;	
					break;
				case 'h'  :
					uart0_printf("\r\n");
					 print_help ();
				break; 
				case 'k'  :
					uart0_printf("reconfigure\r\n");
					 rsu_factory_trigger_reconfig(RSUBASE,BTADDRESS,WDPATTERN);
				break;
 				case 'm'  :
					uart0_printf("clear WD enable\r\n");
					RSU_RSC = 0x0;
					RSU_WDE = 0x0;	
				break; 
				case 'n'  :
					uart0_printf("clear WD Timer\r\n");
					RSU_RSC = 0x0;
					RSU_WDV = 0x0;	
				break; 
			}

	} //while()
	//uart0_printf("Should not be here -- While loop complete.");
return 1;
} //main()

