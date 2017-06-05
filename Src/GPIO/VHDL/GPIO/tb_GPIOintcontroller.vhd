--! @file GPIOintcontroller.vhd
--! @author Salvatore Barone <salvator.barone@gmail.com>
--!			Alfonso Di Martino <alfonsodimartino160989@gmail.com>
--!			Pietro Liguori <pie.liguori@gmail.com>
--! @date 2017-06-05
--! @copyright
--! This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as
--! published by the Free Software Foundation; either version 3 of the License, or any later version.
--! This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
--! of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
--! You should have received a copy of the GNU General Public License along with this program; if not, write to the Free
--! Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

library ieee;
use ieee.std_logic_1164.all;

entity tb_GPIOintcontroller is
end tb_GPIOintcontroller;

architecture behavioral of tb_GPIOintcontroller is

	component GPIOintcontroller is
		generic (	width			: natural := 4);							
		port (		clock			: in	std_logic;							
					GPIO_inout		: in	std_logic_vector(width-1 downto 0);	
					GPIO_inten		: in	std_logic;							
					GPIO_int		: out	std_logic;							
					GPIO_intclr		: in	std_logic);							
	end component;
	
	constant period : time := 10 ns;
	signal clock : std_logic := '0';							
	signal GPIO_inout : std_logic_vector(3 downto 0) := (others => '0');	
	signal GPIO_inten : std_logic := '0';							
	signal GPIO_int : std_logic := '0';							
	signal GPIO_intclr : std_logic := '0';

begin

	uut : GPIOintcontroller 
		generic map (	width			=> 4)
		port map (		clock			=> clock,
						GPIO_inout		=> GPIO_inout,
						GPIO_inten		=> GPIO_inten,
						GPIO_int		=> GPIO_int,
						GPIO_intclr		=> GPIO_intclr);	

	clock_process : process
	begin
		clock <= not clock;
		wait for period / 2;
	end process clock_process;


	stim_process : process
	begin
	
	end process;

end behavioral;