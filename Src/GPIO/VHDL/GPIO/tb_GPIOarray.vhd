--! @file GPIOarray.vhd
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

entity tb_GPIOarray is
end tb_GPIOarray;

architecture behavioral of tb_GPIOarray is

	component GPIOarray is
	Generic (	GPIO_width 		: 		natural := 4);								--! parallelismo dell'array, di default pari a 4 celle.
    Port 	(	clock			: in	std_logic;									--! segnale di clock
    			GPIO_enable		: in 	std_logic_vector (GPIO_width-1 downto 0);	--! segnale di abilitazione, permette di pilotare la linea 
    																				--! "GPIO_inout".
																					--!	Quando GPIO_enable=1, la linea GPIO_inout e quella GPIO_write sono connesse tra loro.
           		GPIO_write 		: in 	std_logic_vector (GPIO_width-1 downto 0);	--! segnale di input, diretto verso l'esterno del device.
           		GPIO_inout	 	: inout std_logic_vector (GPIO_width-1 downto 0);	--! segnale bidirezionale diretto verso l'esterno del device.
           		GPIO_read 		: out 	std_logic_vector (GPIO_width-1 downto 0);	--! segnale di output, diretto verso l'interno del device.
           		-- interrupt
		    	GPIO_inten		: in	std_logic;									--! segnale di abilitazione dell'interrupt sul cambio di stato sul pin GPIO_inout
		    	GPIO_int		: out	std_logic;									--! segnale di interrupt a livelli, se GPIO_inten='1' diventa alto quando GPIO_inout cambia stato
		    	GPIO_intclr		: in	std_logic);									--! segnale di clear per GPIO_int, riporta a '0' il valore di GPIO_int
	end component;
							
	constant period : time := 10 ns;
	signal clock : std_logic := '0';
	signal GPIO_enable: std_logic_vector(3 downto 0) := (others => '0');
	signal GPIO_write: std_logic_vector(3 downto 0) := (others => '0');
	signal GPIO_read: std_logic_vector(3 downto 0) := (others => '0');					
	signal GPIO_inout : std_logic_vector(3 downto 0) := (others => '0');	
	signal GPIO_inten : std_logic := '0';							
	signal GPIO_int : std_logic := '0';							
	signal GPIO_intclr : std_logic := '0';

begin

	uut : GPIOarray 
		generic map (	GPIO_width			=> 4)
		port map (		clock			=> clock,
						GPIO_enable		=> GPIO_enable,
						GPIO_write		=> GPIO_write,
						GPIO_read		=> GPIO_read,
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
		wait for 100 ns;
		GPIO_enable <= x"C";
		GPIO_inten <= '1';
		wait for 10 ns;
		--GPIO_write <= x"4";
		wait for 50 ns;
		GPIO_intclr <= '1', '0' after 50 ns;
		wait for 100 ns;
		GPIO_inout <= x"1";
		wait for 50 ns;
		GPIO_intclr <= '1', '0' after 50 ns;
		wait;
	end process;

end behavioral;
