--! @file gpio.vhd
--! @author Salvatore Barone <salvator.barone@gmail.com>
--!			Alfonso Di Martino <alfonsodimartino160989@gmail.com>
--!			Pietro Liguori <pie.liguori@gmail.com>
--! @date 2017-04-08 00:07:19
--! @copyright
--! This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as
--! published by the Free Software Foundation; either version 3 of the License, or any later version.
--! This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
--! of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
--! You should have received a copy of the GNU General Public License along with this program; if not, write to the Free
--! Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

-- Changelog
-- 2017-04-07:  Creazione del file e prima implementazione1

--! @addtogroup myGPIO
--! @{


library ieee;
use ieee.std_logic_1164.all;

--! @brief  cella base GPIO
entity GPIOsingle is
    Port (	GPIO_enable		: in 	std_logic;	--! segnale di abilitazione, permette di pilotare la linea
												--! "GPIO_inout". Quando GPIO_enable=1, la linea GPIO_inout e quella GPIO_write sono
												--! connesse tra loro.
        	GPIO_write 		: in 	std_logic;	--! segnale di input, diretto verso l'esterno del device.
        	GPIO_inout 		: inout std_logic;	--! segnale bidirezionale diretto verso l'esterno del device.
        	GPIO_read 		: out 	std_logic);	--! segnale di output, diretto verso l'interno del device.
end GPIOsingle;

architecture Structural of GPIOsingle is
begin
	GPIO_read <= GPIO_inout;
	with GPIO_enable select
		GPIO_inout <= 	GPIO_write when '1',
						'Z' when others;
end Structural;

--! @}
