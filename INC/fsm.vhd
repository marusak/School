-- fsm.vhd: Finite State Machine
-- Author(s): Matej Marusak, xmarus06
-- Kod1=1102112033  kod2= 7714784232
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (START,TEST1A,TEST1B,TEST2A,TEST2B,TEST3A,TEST3B,TEST4A,TEST4B,TEST5A, TEST5B,TEST6A,TEST6B,TEST7A,TEST7B,TEST8A,TEST8B,TEST9A,TEST9B,CORRECT,WRONG, PRINT_MESSAGE_WRONG,PRINT_MESSAGE_RIGHT, FINISH);
   --sem daslie TEST1,TEST2...
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= START;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- Kod1=1102112033  kod2= 7714784232
-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when START =>
      next_state <= START;
      if (KEY(1) = '1') then
	      next_state <= TEST1A;
      elsif (KEY(7) = '1') then
	      next_state <= TEST1B;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST1A =>
      next_state <= TEST1A;
      if (KEY(1) = '1') then
	      next_state <= TEST2A;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST2A =>
      next_state <= TEST2A;
      if (KEY(0) = '1') then
	      next_state <= TEST3A;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST3A =>
      next_state <= TEST3A;
      if (KEY(2) = '1') then
	      next_state <= TEST4A;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST4A =>
      next_state <= TEST4A;
      if (KEY(1) = '1') then
	      next_state <= TEST5A;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST5A =>
      next_state <= TEST5A;
      if (KEY(1) = '1') then
	      next_state <= TEST6A;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST6A =>
      next_state <= TEST6A;
      if (KEY(2) = '1') then
	      next_state <= TEST7A;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST7A =>
      next_state <= TEST7A;
      if (KEY(0) = '1') then
	      next_state <= TEST8A;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST8A =>
      next_state <= TEST8A;
      if (KEY(3) = '1') then
	      next_state <= TEST9A;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST9A =>
      next_state <= TEST9A;
      if (KEY(3) = '1') then
	      next_state <= CORRECT;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST1B =>
      next_state <= TEST1B;
      if (KEY(7) = '1') then
	      next_state <= TEST2B;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST2B =>
      next_state <= TEST2B;
      if (KEY(1) = '1') then
	      next_state <= TEST3B;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST3B =>
      next_state <= TEST3B;
      if (KEY(4) = '1') then
	      next_state <= TEST4B;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST4B =>
      next_state <= TEST4B;
      if (KEY(7) = '1') then
	      next_state <= TEST5B;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST5B =>
      next_state <= TEST5B;
      if (KEY(8) = '1') then
	      next_state <= TEST6B;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST6B =>
      next_state <= TEST6B;
      if (KEY(4) = '1') then
	      next_state <= TEST7B;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST7B =>
      next_state <= TEST7B;
      if (KEY(2) = '1') then
	      next_state <= TEST8B;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST8B =>
      next_state <= TEST8B;
      if (KEY(3) = '1') then
	      next_state <= TEST9B;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST9B =>
      next_state <= TEST9B;
      if (KEY(2) = '1') then
	      next_state <= CORRECT;
      elsif (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG; 
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_MESSAGE_WRONG =>
      next_state <= PRINT_MESSAGE_WRONG;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when CORRECT =>
      next_state <= CORRECT;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_RIGHT;
      elsif (KEY(15 downto 0) /= "0000000000000000") then
	      next_state <= WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when WRONG =>
      next_state <= WRONG;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_MESSAGE_RIGHT =>
      next_state <= PRINT_MESSAGE_RIGHT;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= START; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= START;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_MESSAGE_WRONG =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_MESSAGE_RIGHT =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_MX_MEM     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1'; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   end case;
end process output_logic;

end architecture behavioral;

