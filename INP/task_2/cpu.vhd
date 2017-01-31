-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2015 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): DOPLNIT
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (1) / zapis (0)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
	signal PC_int : std_logic;
	signal PC_dec : std_logic;
	signal PC_out : std_logic_vector(12 downto 0);--kolko??
	
	signal PTR_int : std_logic;
	signal PTR_dec : std_logic;
	signal PTR_out : std_logic_vector(12 downto 0);--kolko??

	signal CNT_int : std_logic;
	signal CNT_dec : std_logic;
	signal CNT_out : std_logic_vector(7 downto 0);--kolko??

	type input_char is (fetch,decode, ptrInc, ptrSub,valIncR, valSubR,valInc,valSub,whileB,whileBR,
                        whileE,whileER, crlikaj_dopredu, crlikaj_dopreduR, crlikaj_spatR,
                        crlikaj_spat, printR, print,scan, scanR, toTmp,toTmpR, fromTmp, Eof,comment);
    signal next_state    : input_char;
    signal present_state : input_char;

    signal tmp     : std_logic_vector(7 downto 0);

begin

	
	PC: process(CLK,PC_int,PC_dec,RESET)--programovy citac
        --posuvam sa nim po insturkciach od 0000-0FFF
        --kontrola ze ci neprebehlo za F000?
	begin
		if (RESET = '1') then
			PC_out <= (others => '0');
		elsif (CLK'event) and (CLK = '1') then
			if(PC_int = '0' and PC_dec = '1') then
				PC_out <= PC_out - 1;
			elsif( PC_int = '1' and PC_dec = '0') then
				PC_out <= PC_out + 1;
			end if;
		end if;
	end process;
	
	
	PTR: process(CLK,PTR_int, PTR_dec, RESET)--ukazatel do pameti dat 
	begin
		if (RESET = '1') then
			PTR_out <= "1000000000000";
		elsif (CLK'event) and (CLK = '1') then
			if(PTR_int = '0' and PTR_dec = '1') then
					if (PTR_out = "1000000000000")then
						PTR_out <= "1111111111111";
					else 
						PTR_out <= PTR_out - 1;
					end if;
			elsif( PTR_int = '1' and PTR_dec = '0') then
				if (PTR_out = "1111111111111") then
					PTR_out <= "1000000000000";
				else
					PTR_out <= PTR_out + 1;
				end if;
            end if;
		end if;
	end process;
	
    CNT: process(CLK,CNT_int, CNT_dec, RESET)--obmedzene na 8bitov
    begin
        if (RESET = '1') then
            CNT_out <= "00000000";
		elsif (CLK'event) and (CLK = '1') then
			if(CNT_int = '0' and CNT_dec = '1') then
				CNT_out <= CNT_out - 1;
			elsif( CNT_int = '1' and CNT_dec = '0') then
				CNT_out <= CNT_out + 1;
			end if;
		end if;
	end process;
        

    stuff :process(CLK, RESET, EN, next_state)
    begin
        if (RESET = '1') then
            present_state <= fetch;
        elsif (EN = '1' and CLK'event and CLK = '1') then
            present_state <= next_state;
        end if;
    end process;


   next_state_logic :process(CLK,present_state,PC_OUT,DATA_RDATA,PTR_OUT,CNT_OUT,OUT_BUSY,IN_VLD,IN_DATA,EN,tmp)
	VARIABLE instruction : std_logic_vector(7 downto 0);
	begin
        --next_state <= fetch;
        PTR_int <= '0';
        PTR_dec <= '0';
        PC_int  <= '0';
        PC_dec  <= '0';
        CNT_int <= '0';
        CNT_dec <= '0';
        OUT_WE  <= '0';
        DATA_EN <= '0';
		case (present_state) is
            when fetch =>--precitame co je na mem[PC]
                DATA_ADDR  <= PC_out;
                DATA_EN    <= '1';
                DATA_RDWR  <= '1';--citanie
                next_state <= decode;
            when decode =>--zistim co to je za instrukciu
                instruction := DATA_RDATA;
                case (instruction) is
			        when X"3E"  =>  next_state <= ptrInc;  -- > DONE
			        when X"3C"  =>  next_state <= ptrSub;  -- < DONE
			        when X"2B"  =>  next_state <= valIncR; -- + DONE
			        when X"2D"  =>  next_state <= valSubR; -- - DONE
			        when X"5B"  =>  next_state <= whileBR;  -- [ 
			        when X"5D"  =>  next_state <= whileER;  -- ]
			        when X"2E"  =>  next_state <= printR;  -- . DONE
			        when X"2C"  =>  next_state <= scanR;   -- , DONE
			        when X"24"  =>  next_state <= toTmpR;   -- $ DONE
			        when X"21"  =>  next_state <= fromTmp; -- !  DONE
			        when X"00"  =>  next_state <= Eof;     -- null DONE
                    when others =>  next_state <= comment; --komentar DONE
                end case;
            when ptrInc =>
                PTR_int    <= '1';
                PC_int     <= '1';
                next_state <= fetch;
            when ptrSub =>
                PTR_dec    <= '1';
                PC_int     <= '1';
                next_state <= fetch;
            when valIncR =>--precitame bunku
                DATA_ADDR <= PTR_out;
                DATA_EN   <= '1';
                DATA_RDWR <= '1';
                next_state <= valInc;
            when valInc =>--zapiseme bunku
                DATA_ADDR  <= PTR_out;
                DATA_EN    <= '1';
                DATA_RDWR  <= '0';
                DATA_WDATA <= DATA_RDATA + 1;
                PC_int     <= '1';
                next_state <= fetch;
            when valSubR =>--precitame bunku
                DATA_ADDR <= PTR_out;
                DATA_EN   <= '1';
                DATA_RDWR <= '1';
                next_state <= valSub;
            when valSub =>--zapiseme bunku
                DATA_ADDR  <= PTR_out;
                DATA_EN    <= '1';
                DATA_RDWR  <= '0';
                DATA_WDATA <= DATA_RDATA - 1;
                PC_int     <= '1';
                next_state <= fetch;
            when printR =>
                DATA_ADDR <= PTR_out;
                DATA_EN   <= '1';
                DATA_RDWR <= '1';
                next_state <= print;
            when print =>
                if (OUT_BUSY = '1') then
                    next_state <= print;
                else
                    OUT_DATA    <= DATA_RDATA;
                    OUT_WE      <= '1';
                    PC_int      <= '1';
                    next_state <= fetch;
                end if;
            when scanR =>
                IN_REQ     <= '1';
                next_state <= scan;
            when scan =>
                if (IN_VLD = '1') then
                    IN_REQ <= '0';
                    DATA_ADDR  <= PTR_out;
                    DATA_EN    <= '1';
                    DATA_RDWR  <= '0';
                    DATA_WDATA <= IN_DATA;
                    PC_int     <= '1';
                    next_state <= fetch;
                else
                    next_state <= scan;
                end if;
            when toTmpR =>
                DATA_ADDR <= PTR_out;
                DATA_EN   <= '1';
                DATA_RDWR <= '1';
                next_state <= toTmp;
            when toTmp =>
                tmp <= DATA_RDATA;
                PC_int     <= '1';
                next_state <= fetch;
            when fromTmp =>
                DATA_ADDR <= PTR_out;
                DATA_EN   <= '1';
                DATA_RDWR <= '0';
                DATA_WDATA <= tmp;
                PC_int     <= '1';
                next_state <= fetch;

            when whileBR =>--len citanie
                DATA_ADDR <= PTR_out;
                DATA_EN   <= '1';
                DATA_RDWR <= '1';
                next_state <= whileB;
            when whileB =>
                if (DATA_RDATA = "00000000") then--nepravda precrlikam dalej
                    CNT_int    <= '1';
                    PC_int     <= '1';
                    next_state <= crlikaj_dopreduR;
                else--pravda, vykonam dalej instrukcie
                    next_state <= fetch;
                    PC_int     <= '1';
                end if;
            when crlikaj_dopreduR =>
                DATA_ADDR  <= PC_out;
                DATA_EN    <= '1';
                DATA_RDWR  <= '1';--citanie
                next_state <= crlikaj_dopredu;
            when crlikaj_dopredu =>
                if (DATA_RDATA = X"5B")then --[
                    CNT_int    <= '1';
                    PC_int     <= '1';
                    next_state <= crlikaj_dopreduR;
                elsif (DATA_RDATA = X"5D") then --]
                    if (CNT_out = "00000001") then
                        CNT_dec    <= '1';
                        PC_int     <= '1';
                        next_state <= fetch;
                    else
                        CNT_dec    <= '1';
                        PC_int     <= '1';
                        next_state <= crlikaj_dopreduR;
                    end if;
                else
                    PC_int     <= '1';
                    next_state <= crlikaj_dopreduR;
                end if;

            when whileER =>
                DATA_ADDR <= PTR_out;
                DATA_EN   <= '1';
                DATA_RDWR <= '1';
                next_state <= whileE;
            when whileE =>
                if (DATA_RDATA = "00000000") then--nepravda idem dalej 
                    next_state <= fetch;
                    PC_int     <= '1';
                else--pravda, precrlikam na zaciatok
                    CNT_int <= '1';
                    PC_dec     <= '1';
                    next_state <= crlikaj_spatR; 
                end if;
            when crlikaj_spatR =>
                DATA_ADDR  <= PC_out;
                DATA_EN    <= '1';
                DATA_RDWR  <= '1';--citanie
                next_state <= crlikaj_spat;
            when crlikaj_spat =>
                if (DATA_RDATA = X"5D") then--]
                    CNT_int    <= '1';
                    PC_dec     <= '1';
                    next_state <= crlikaj_spatR;
                elsif (DATA_RDATA = X"5B") then --[
                    if (CNT_out = "00000001") then
                        CNT_dec    <= '1';
                        PC_int     <= '1';
                        next_state <= fetch;
                    else
                        CNT_dec    <= '1';
                        PC_dec     <= '1';
                        next_state <= crlikaj_spatR;
                    end if;
                else
                    PC_dec     <= '1';
                    next_state <= crlikaj_spatR;
                end if;


            when comment =>
                PC_int     <= '1';
                next_state <= fetch;
            when Eof =>
                next_state <= Eof;
            when others=>--ako sa robi NOP?
                next_state <= fetch;
                PC_dec <='0';
        end case;
    end process;

	
	

 -- zde dopiste vlastni VHDL kod

end behavioral;
 
