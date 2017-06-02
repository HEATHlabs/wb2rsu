-- DATASHEET: https://drive.google.com/open?id=0BxW0H68Pf2sKWXYxVkhYYUw2dVE
--timer mode reset is needed before timer starts.
-- clock : input
-- reset : input
-- data_in[]: input params [5], input address_base [22]
-- params : input [3]
-- read_params : input
-- write_params : input
-- reconfig : input
-- reset_timer : input 
-- read_source : input [2]
-- data_out[]: ouput [29]
-- busy : output

--
-- 


-- For Cyclone III devices, mapping to each parameter type and corresponding parameter bit width is defined as follows:
-- *000 - Master State Machine Current State Mode (Read Only) - width of 2. Values are defined as follows:
--    00 Factory mode
--    01 Application mode
--    11 Application mode with Master State Machine
-- User Watchdog Timer Enabled
-- 001 - Force early CONF_DONE (Cd_early) check - width of 1.
-- *010 - Watchdog Timeout Value - width of 29 when reading and width of 12 when writing. Note that the 12 bits for writing are the upper 12 bits of the 29-bit Watchdog Timeout Value.
-- *011 - Watchdog Enable - width of 1
-- *100 - Boot Address- width of 24 when reading and width of
-- 22 when writing. Note that the boot address should be written to the upper 22 bits of the 24-bit Boot Address.
-- *101 - (illegal value)
-- *110 - Force the internal oscillator as startup state machine clock (Osc_int) option bit - width of 1
-- *111 - Reconfiguration trigger conditions (Read Only) -
-- width of 5. Trigger bits are defined as follows:
-- Bit 4: nconfig_source: external configuration reset (nCONFIG) assertion
-- Bit 3: crcerror_source: CRC error during applicationconfiguration
-- Bit 2: nstatus_source: nSTATUS asserted by an external device as the result of an error
-- Bit 1: wdtimer_source: User Watchdog Timer timeout
-- Bit 0: runconfig_source: configuration reset triggered from logic array
-- All parameters can be written in Factory configuration mode only
-- 
--Usage.
-- write params at address 0x0 then write Data or read Data
-- Write Data at address 0x1 (wd timer value, boot address, we enable ,etc
-- Write anything to ox2 to start RECONFIG
-- Write anything to 0x3 to reset WD timer
-- Read from params at address 0x0
--WBADR[5:2]	param 	

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity wb_rsu is
generic (
    dat_sz  : natural := 32
);
port (
    clk_i  : in  std_logic;
    rst_i  : in  std_logic;
    --
    -- Whishbone Interface
    --
    dat_i  : in  std_logic_vector((dat_sz - 1) downto 0);
    dat_o  : out std_logic_vector((dat_sz - 1) downto 0);
    adr_i  : in  std_logic_vector(3 downto 0);
    cyc_i  : in  std_logic;
    sel_i  : in  std_logic_vector(3 downto 0);
    we_i   : in  std_logic;
    ack_o  : out std_logic;
    err_o  : out std_logic;
    rty_o  : out std_logic;
    halt_o: out std_logic;
    stb_i  : in  std_logic;
    --
    -- RSU Interface/ALTREMOTE_UPDATE ip design
    --
    rsu_clock  : out std_logic;
    rsu_reset  : out std_logic;
	rsu_datain : out std_logic_vector (21 downto 0);
	rsu_dataout: in std_logic_vector(28 downto 0);
	rsu_params : out std_logic_vector(2 downto 0);
	rsu_readparams: out std_logic;
	rsu_writeparams: out std_logic;
	rsu_reconfig: out std_logic;
	rsu_resettimer: out std_logic;
	rsu_busy : in std_logic;
	rsu_readsource: out std_logic_vector(1 downto 0)
	
	
	
);
end wb_rsu;

architecture Behavioral of wb_rsu is 
    -- Internal Signals
    signal reg_params		: std_logic_vector(2 downto 0);
    signal reg_readparams	: std_logic;
	signal reg_writeparams	: std_logic;
	signal reg_data_in		: std_logic_vector(21 downto 0);
	
	TYPE State_type IS (START, READSOURCE, READPARAMS, WRITEPARAMS, BUSY, BUSY2, WAIT1, WAIT2);  -- Define the states
	SIGNAL State : State_Type;    -- Create a signal that uses 
	-- the different states

	BEGIN
	-- always
	rsu_datain <= dat_i(21 downto 0);
	--rsu_readsource <= "00"; -- always reading current state.
	rsu_clock <= clk_i;
	rsu_reset <= rst_i;
	err_o <= '0';
 	rty_o <= '0';
	
	
PROCESS (clk_i, rst_i) BEGIN
    If (rst_i = '1') THEN            -- Upon reset, set the state to A
		rsu_readsource <= "11";
		State <= START;
 
    ELSIF rising_edge(clk_i) THEN    -- if there is a rising edge of the
			 -- clock, then do the stuff below
 

		rsu_params <= reg_params;
		rsu_readparams <= reg_readparams;
		rsu_writeparams <= reg_writeparams;
		rsu_reconfig <= '0';
		reg_writeparams <= '0';
		reg_readparams <= '0';
		rsu_resettimer <= '0';
		ack_o <= '0';
		halt_o <= '0';
		-- The CASE statement checks the value of the State variable,
	-- and based on the value and any other control signals, changes
	-- to a new state.		
		CASE State IS
 
		-- If the current state is A and P is set to 1, then the
		-- next state is B
		WHEN START => 
				if (stb_i = '1') then
--					ack_o <= '1';
					ack_o <= '0';
					halt_o <= '0';
					rsu_reconfig <= '0';
					reg_writeparams <= '0';
					reg_readparams <= '0';
					rsu_resettimer <= '0';					
		--------------------------------------------------------------------					
		--------------------------------------------------------------------					
					--WRITE
		--------------------------------------------------------------------					
		--------------------------------------------------------------------					
					if (we_i = '1') then  --write:
						case adr_i is 
		--------------------------------------------------------------------					
						when "0000" =>-- Master State machine (RO)
							-- not WRITE to this address/do nothing
							ack_o <= '0';
							halt_o <= '0';
							State <= START;

		--------------------------------------------------------------------					
						when "0001" =>-- Force early CONF_DONE check (RW)
							reg_params   <= adr_i(2 downto 0);
							reg_data_in     <= "000000000000000000000" & dat_i(0);	
							reg_writeparams <= '1';
							reg_readparams <= '0';
							rsu_reconfig <= '0';
							rsu_resettimer <= '0';
							State <= WRITEPARAMS;
	
		--------------------------------------------------------------------					
						when "0010" =>-- Watchdog Timer Value (RW) [Read: 29bits, Write: 12bits]
							reg_params   <= adr_i(2 downto 0);
							reg_data_in     <= "0000000000" & dat_i(11 downto 0);	
							reg_writeparams <= '1';
							reg_readparams <= '0';
							rsu_reconfig <= '0';
							rsu_resettimer <= '0';
							State <= WRITEPARAMS;
		--------------------------------------------------------------------					
						when "0011" =>-- Watchdog Enable (RW)
							reg_params   <= adr_i(2 downto 0);
							reg_data_in     <= "000000000000000000000" & dat_i(0);	
							reg_writeparams <= '1';
							reg_readparams <= '0';
							rsu_reconfig <= '0';
							rsu_resettimer <= '0';
							State <= WRITEPARAMS;
		--------------------------------------------------------------------					
						when "0100" =>-- Boot Address (RW) [Read: 24bits, Write: 22bits]
						   reg_params   <= adr_i(2 downto 0);
							reg_data_in     <= dat_i(21 downto 0);	
							reg_writeparams <= '1';
							reg_readparams <= '0';
							rsu_reconfig <= '0';
							rsu_resettimer <= '0';
							State <= WRITEPARAMS;
		--------------------------------------------------------------------					
						when "0101" =>-- NA
							--reg_params   <= adr_i(5 downto 2);
							reg_writeparams <= '1';
							reg_readparams <= '0';
							rsu_reconfig <= '0';
							rsu_resettimer <= '0';
							State <= WRITEPARAMS;
		--------------------------------------------------------------------					
						when "0110" =>-- Force internal OSC for state machine
							reg_params   <= adr_i(2 downto 0);
							reg_data_in     <= "000000000000000000000" & dat_i(0);	
							reg_writeparams <= '1';
							reg_readparams <= '0';
							rsu_reconfig <= '0';
							rsu_resettimer <= '0';
							State <= WRITEPARAMS;
		--------------------------------------------------------------------					
						when "0111" =>-- Reconfigure trigger condition (RO) 5 bits
							--reg_params   <= adr_i(5 downto 2);
							reg_writeparams <= '1';
							reg_readparams <= '0';
							rsu_reconfig <= '0';
							rsu_resettimer <= '0';
							State <= WRITEPARAMS;
		 --------------------------------------------------------------------					
					   when "1000" =>-- read_source
							rsu_readsource <=  dat_i(1 downto 0);	
							reg_writeparams <= '0';
							reg_readparams <= '0';
							rsu_reconfig <= '0';
							rsu_resettimer <= '0';
							ack_o <= '1';
							
							State <= READSOURCE;
			--------------------------------------------------------------------					
					   when "1001" =>-- reconfig
							rsu_reconfig <= '1';
							reg_writeparams <= '0';
							reg_readparams <= '0';
							rsu_resettimer <= '0';
							ack_o <= '0';
							halt_o <= '0';
							State <= START;
		 --------------------------------------------------------------------					
						when "1010" =>-- reset WD timer
							rsu_reconfig <= '0';
							reg_writeparams <= '0';
							reg_readparams <= '0';
							rsu_resettimer <= '1';
							ack_o <= '0';
							halt_o <= '0';
							State <= START;
		 --------------------------------------------------------------------					
					   when others =>
							--dat_o   <= (others => '-');
						end case;
					else --Read 
						case adr_i is 
		--------------------------------------------------------------------					
						when "0000" =>-- Master State machine (RO)
							reg_params   <= adr_i(2 downto 0);
							reg_writeparams <= '0';
							reg_readparams <= '1';
							rsu_reconfig <= '0';
							rsu_resettimer <= '0';
							State <= READPARAMS;
		--------------------------------------------------------------------					
						when "0001" =>-- Force early CONF_DONE check (RW)
							reg_params   <= adr_i(2 downto 0);
							reg_writeparams <= '0';
							reg_readparams <= '1';
							rsu_reconfig <= '0';
							rsu_resettimer <= '0';
							State <= READPARAMS;
		--------------------------------------------------------------------					
						when "0010" =>-- Watchdog Timer Value (RW) [Read: 29bits, Write: 12bits]
							reg_params   <= adr_i(2 downto 0);
							reg_writeparams <= '0';
							reg_readparams <= '1';
							rsu_reconfig <= '0';
							rsu_resettimer <= '0';
							State <= READPARAMS;
		--------------------------------------------------------------------					
						when "0011" =>-- Watchdog Enable (RW)
							reg_params   <= adr_i(2 downto 0);
							reg_writeparams <= '0';
							reg_readparams <= '1';
							rsu_reconfig <= '0';
							rsu_resettimer <= '0';
							State <= READPARAMS;
		--------------------------------------------------------------------					
						when "0100" =>-- Boot Address (RW) [Read: 24bits, Write: 22bits]
							reg_params   <= adr_i(2 downto 0);
							reg_writeparams <= '0';
							reg_readparams <= '1';
							rsu_reconfig <= '0';
							rsu_resettimer <= '0';
							State <= READPARAMS;
		--------------------------------------------------------------------					
						when "0101" =>-- NA
							--reg_params   <= adr_i(5 downto 2);
							reg_writeparams <= '0';
							reg_readparams <= '1';
							rsu_reconfig <= '0';
							rsu_resettimer <= '0';
							State <= START;
		--------------------------------------------------------------------					
						when "0110" =>-- Force internal OSC for state machine
							reg_params   <= adr_i(2 downto 0);
							reg_writeparams <= '0';
							reg_readparams <= '1';
							rsu_reconfig <= '0';
							rsu_resettimer <= '0';
							State <= READPARAMS;
		--------------------------------------------------------------------					
						when "0111" =>-- Reconfigure trigger condition (RO) 5 bits
							reg_params   <= adr_i(2 downto 0);
							reg_writeparams <= '0';
							reg_readparams <= '1';
							rsu_reconfig <= '0';
							rsu_resettimer <= '0';
							State <= READPARAMS;
		 --------------------------------------------------------------------					
--					   when "1000" =>-- read_source
--							dat_o <= "000000000000000000000000000000" & rsu_readsource;
--							--read_source <=  dat_i(1 downto 0);	
--							reg_writeparams <= '0';
--							reg_readparams <= '0';
--							rsu_reconfig <= '0';
--							rsu_resettimer <= '0';
--							State <= START;
						when others =>
							dat_o   <= (others => '-');
							ack_o <= '0';
							halt_o <= '0';
							State <= START;
						end case;
					end if; -- we_i
				else -- stb=0
					ack_o <= '0';
					halt_o <= '0';
					State <= START;
				end if; -- stb		-
		WHEN READSOURCE => 
			reg_writeparams <= '0';
			reg_readparams <= '0';
			rsu_reconfig <= '0';
			rsu_resettimer <= '0';
			halt_o <= '0';
			ack_o <= '1';
			State <= WAIT2; 
		WHEN WRITEPARAMS => 
			reg_writeparams <= '0';
			reg_readparams <= '0';
			rsu_reconfig <= '0';
			rsu_resettimer <= '0';
			halt_o <= '1';
			ack_o <= '0';

			State <= BUSY; 
		WHEN READPARAMS => 
			reg_writeparams <= '0';
			reg_readparams <= '0';
			rsu_reconfig <= '0';
			rsu_resettimer <= '0';
			halt_o <= '1';
			ack_o <= '0';
			State <= BUSY; 
		WHEN BUSY => -- busy reading or writing.
			halt_o <= '1';
			ack_o <= '0';
			
			State <= BUSY2;
		WHEN BUSY2 => -- busy reading or writing.
			ack_o <= '0';
			IF rsu_busy='1' THEN 
				State <= BUSY2; 
				halt_o <= '1';
				ack_o <= '0';
			ELSE 
				halt_o <= '0';	
				ack_o <= '1';
				State <= WAIT1;
				case reg_params is 
--------------------------------------------------------------------					
				when "000" =>-- Master State machine (RO)
					dat_o   <= "000000000000000000000000000000" & rsu_dataout(1 downto 0);
--------------------------------------------------------------------					
				when "001" =>-- Force early CONF_DONE check (RW)
					dat_o   <= "0000000000000000000000000000000"& rsu_dataout(0);

--------------------------------------------------------------------					
				when "010" =>-- Watchdog Timer Value (RW) [Read: 29bits, Write: 12bits]
					dat_o   <= "000"&rsu_dataout(28 downto 0);
--------------------------------------------------------------------					
				when "011" =>-- Watchdog Enable (RW)
					dat_o   <= "0000000000000000000000000000000"&rsu_dataout(0);
--------------------------------------------------------------------					
				when "100" =>-- Boot Address (RW) [Read: 24bits, Write: 22bits]
					dat_o   <= "00000000"&rsu_dataout(23 downto 0);
--------------------------------------------------------------------					
				when "101" =>-- NA
					dat_o   <= (others => '-');
--------------------------------------------------------------------					
				when "110" =>-- Force internal OSC for state machine
					dat_o   <= "0000000000000000000000000000000"&rsu_dataout(0);
--------------------------------------------------------------------					
				when "111" =>-- Reconfigure trigger condition (RO) 5 bits
					dat_o   <= "000000000000000000000000000"&rsu_dataout(4 downto 0);
 --------------------------------------------------------------------					
				when others =>
					dat_o   <= (others => '-');
				end case;
			END IF;
				--State <= START; 
		WHEN WAIT1 =>
			halt_o <= '0';	
			ack_o <= '0';
			State <= WAIT2;
		WHEN WAIT2 =>
			halt_o <= '0';	
			ack_o <= '0';
			State <= START;
		WHEN others =>
			State <= START;
	END CASE; 
    END IF; 
  END PROCESS;
end Behavioral; --WAIT1
