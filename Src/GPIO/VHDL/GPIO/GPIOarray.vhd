--! @file GPIOarray.vhd
--! @author Salvatore Barone <salvator.barone@gmail.com>
--!			Alfonso Di Martino <alfonsodimartino160989@gmail.com>
--!			Pietro Liguori <pie.liguori@gmail.com>
--! @date 2017-04-07
--! @copyright
--! This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as
--! published by the Free Software Foundation; either version 3 of the License, or any later version.
--! This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
--! of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
--! You should have received a copy of the GNU General Public License along with this program; if not, write to the Free
--! Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

-- Changelog
-- 2017-04-07: Creazione del file e prima implementazione1
-- 2017-06-05: Aggiunta del supporto agli interrupt

library ieee;
use ieee.std_logic_1164.all;

--! @brief array di celle GPIO, pilotabili singolarmente
entity GPIOarray is
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
end GPIOarray;


architecture Structural of GPIOarray is
	component GPIOsingle is
		Port (	GPIO_enable		: in 	std_logic;
	 			GPIO_write 		: in 	std_logic;
	 			GPIO_inout 		: inout std_logic;
	 			GPIO_read 		: out 	std_logic);
	end component;
	
	component GPIOintcontroller is
		generic (	width			: natural := 4);							
		port (		clock			: in	std_logic;							
					GPIO_inout		: in	std_logic_vector(width-1 downto 0);	
					GPIO_inten		: in	std_logic;							
					GPIO_int		: out	std_logic;							
					GPIO_intclr		: in	std_logic);							
	end component;

begin
	gpio_array : for i in GPIO_width-1 downto 0 generate
		single_gpio : GPIOsingle
			Port map(	GPIO_write_en	=> GPIO_write_en(i),
						GPIO_write		=> GPIO_write(i),
						GPIO_inout		=> GPIO_inout(i),
						GPIO_read		=> GPIO_read(i));
	end generate;
	
	int_controller : GPIOintcontroller
		generic map (	width			=> GPIO_width)
		port map (		clock			=> clock,
						GPIO_inout		=> GPIO_inout,
						GPIO_inten		=> GPIO_inten,
						GPIO_int		=> GPIO_int,
						GPIO_intclr		=> GPIO_intclr);
	
 end Structural;
