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

--! @brief
entity GPIOintcontroller is
	generic (	width			: natural := 4);							--! parallelismo dell'array, di default pari a 4 celle.
	port (		clock			: in	std_logic;							--! segnale di clock
				GPIO_inout		: in	std_logic_vector(width-1 downto 0);	--! segnale monitorato, al cambio di stato, se GPIO_inten e' '1', viene generato interrupt
				GPIO_inten		: in	std_logic;							--! segnale di abilitazione dell'interrupt sul cambio di stato sul pin GPIO_inout
				GPIO_int		: out	std_logic;							--! segnale di interrupt a livelli, se GPIO_inten='1' diventa alto quando GPIO_inout cambia stato
				GPIO_intclr		: in	std_logic);							--! segnale di clear per GPIO_int, riporta a '0' il valore di GPIO_int
end GPIOintcontroller;

architecture behavioral of GPIOintcontroller is
	signal old_GPIO_inout :	std_logic_vector(width-1 downto 0) := (others => '0');
	signal GPIO_int_tmp : std_logic := '0';
begin

	GPIO_int <= GPIO_int_tmp;
	
	process(clock, GPIO_inout, GPIO_inten, GPIO_intclr)
	begin
		if (rising_edge(clock)) then
			if (GPIO_inten = '1' and old_GPIO_inout /= GPIO_inout) then
				GPIO_int_tmp <= '1';
				old_GPIO_inout <= GPIO_inout;
			elsif (GPIO_intclr = '1') then
				GPIO_int_tmp <= '0';
			end if;
		end if;	
	end process;

end behavioral;