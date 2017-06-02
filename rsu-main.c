/* this is where my Addresses are defined. Yours will be different */
#include "../lib/storm_core.h"
#include "../lib/storm_soc_basic.h"

#define WDPATTERN 0x10
#define BTPATTERN 0x10
#define BTADDRESS 0x00080000

static void printf_int(char *printstring, unsigned long int num2print);
static void printf_char(char *printstring, unsigned long int num2print);
// +------------------------------+
// |    Simple Program Demo       |
// +------------------------------+
  void printf_int(char *printstring, unsigned long int num2print) {
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

}
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
	uart0_printf("j: Write 0x3 to read_source             h: Print this Menu\r\n");
}

int main( void ){  
    /*define a packet,of structure ARP,and lets call it arpPacket*/
	unsigned long regtemp1;
	char  str_temp[10]="test";
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
					printf_int("Read boot addr: ", regtemp1);
					break;
				case '2'  :
					uart0_printf("writing BTADDRESS to Boot Address\r\n");
					RSU_RSC = 0x0;
//					RSU_BAD = BTADDRESS>>2;	
					RSU_BAD = BTADDRESS;	
					break;
				case '3'  :
					uart0_printf("Reading Master State Machine\r\n");
					RSU_RSC = 0x3;
					regtemp1 = RSU_MSM;	
					printf_int("Read Early CONF_DONE check: ", regtemp1);
					break;
				case '4'  :
					uart0_printf("Reading the force Early CONF_DONE check\r\n");
					regtemp1 = RSU_ECD;
					printf_int("Read Early CONF_DONE check: ", regtemp1);
					break;
				case '5'  :
					uart0_printf("Writing the force Early CONF_DONE check\r\n");
					RSU_ECD = 0x01;	
					break;
				case '6'  :
					uart0_printf("Reading Watchdog Timer\r\n");
					RSU_RSC = 0x3;
					regtemp1 = RSU_WDV;
					printf_int("Read WD value: ", regtemp1);
					break;
				case '7'  :
					uart0_printf("writing WDPATTERn to Watchdog Timer\r\n");
					RSU_RSC = 0x0;
					RSU_WDV = WDPATTERN;	
					break;
				case '8'  :
					uart0_printf("Reading Watchdog Enable\r\n");
					regtemp1 = RSU_WDE;
					printf_int("Read WD Enable value: ", regtemp1);
					break;
				case '9'  :
					uart0_printf("writing Watchdog Enable\r\n");
					RSU_WDE = 0x1;	
					break;
				case 'a'  :
					uart0_printf("Reading Int OSC state\r\n");
					regtemp1 = RSU_IOS;
					printf_int("Read Int OSC state: ", regtemp1);
					break;
				case 'b'  :
					uart0_printf("writing Int OSC state\r\n");
					RSU_IOS = 0x1;	
					break;
				case 'c'  :
					uart0_printf("Reading Reconfigure trigger condition\r\n");
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

			}

	} //while()
	//uart0_printf("Should not be here -- While loop complete.");
return 1;
} //main()

