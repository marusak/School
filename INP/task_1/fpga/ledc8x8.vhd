library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
    port(
        ROW : out std_logic_vector (7 downto 0);
        LED : out std_logic_vector (7 downto 0);
        RESET, SMCLK : in std_logic);
    end ledc8x8;

architecture ledc8x8 of ledc8x8 is
    signal ce         : std_logic := '0';
	signal helpRow    : std_logic_vector(7 downto 0)  := "10000000"; 
	signal helpLed    : std_logic_vector(7 downto 0)  := "10000000"; 
    signal count      : std_logic_vector(7 downto 0)  := "00000000";
	signal first_half : std_logic_vector(22 downto 0) := "00000000000000000000000";
begin
    
--FINAL COUTNDOWN
    line_decoder : process (ce,RESET,HelpLed,helpRow)
    begin
    
       if (rising_edge(ce)) then
			--ROW <= helpRow;
			if first_half(22) = '1' then
				case helpRow is
					when "00000001" => helpLed <= "01110111";
					when "00000000" => helpLed <= "01110111";
					when "01000000" => helpLed <= "01010111";
					when "00100000" => helpLed <= "00100110";
					when "00010000" => helpLed<= "01100110";
					when "00001000" => helpLed <= "11101010";
					when "00000100" => helpLed <= "11100100";
					when "00000010" => HelpLed <= "11101110";
					when others => null;
				end case;
			else
				case helpRow is
					when "00000001" => helpLed <= "11111111";
					when "00000000" => helpLed <= "11111111";
					when "01000000" => helpLed <= "11111111";
					when "00100000" => helpLed <= "11101110";
					when "00010000" => helpLed <= "11101110";
					when "00001000" => helpLed <= "11101010";
					when "00000100" => helpLed <= "11100100";
					when "00000010" => helpLed <= "11101110";
					when others => null;
				end case;
			end if;
        end if;
    end process;


    rot_reg : process (RESET, ce, SMCLK)
    begin
        if RESET = '1' then
			helpRow <= "10000000";
        elsif rising_edge(ce) then
                case helpRow is
                    when "10000000" => helpRow <= "01000000";
                    when "01000000" => helpRow <= "00100000";
                    when "00100000" => helpRow <= "00010000";
                    when "00010000" => helpRow <= "00001000";
                    when "00001000" => helpRow <= "00000100";
                    when "00000100" => helpRow <= "00000010";
                    when "00000010" => helpRow <= "00000001";
                    when "00000001" => helpRow <= "10000000";
                    when others => null;
                end case;
        end if;
    end process;
                

    ena : process (RESET, SMCLK,helpLed,helpRow)
    begin
        if (RESET = '1') then
            ROW <= "10000000";
            count <= "00000000";
			LED <= "11111111";
			first_half <= "00000000000000000000000";
        elsif rising_edge(SMCLK) then
            if (count = "11111111") then
                ce <= '1';
	--			LED <= "11111111";
				ROW <=helpRow;
				LED <= helpLed;
            else
                ce <= '0';
            end if;
            if first_half(22) = '0' then
				first_half <= first_half + 1;
			end if;
            count <= count + 1;
        end if;
    end process;



end architecture;



            
