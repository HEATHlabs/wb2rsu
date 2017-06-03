# wb2rsu
Wishbone bus (simple) to Altera Remote System Update IP Core (MegaWizard cores)

Tested with Cyclone III and will probably work with Cyclone IV (untested).  

Additional notes:

From the user guide

read_source follow: 


■ 00 - Current State Content in Status Register

■ 01 - Previous State Register 1 Content in Status Register 

■ 10 - Previous State Register 2 Content in Status Register 

■ 11 - Value in Input Register  ← **use this one**

 
2) Read out and check the registers of altremote_update before and after you try to restart in application image. Note that its confusing which value to use for read_source for Cyclone III/IV. I have come to that read_source shall be 0 for parameter 1 (Current image), 2 for parameter 4 (Boot address) and 1 for parameter 7 (Config source)

 
I intend to boot with 2nd application image (offset 0x080000). I write param 4 as follows:

read_source = 0x00

param = 4

data_in = 0x20000

To check if writing was successful, I read back param 4:

read_source = 0x11

param = 4

data_out = 0x20000


