--! @file GPIOsingle.vhd
--! @author Salvatore Barone <salvator.barone@gmail.com>
--! @date 07 04 2017
--!
--! @copyright
--! Copyright 2017 Salvatore Barone <salvator.barone@gmail.com>
--! 
--! This file is part of Zynq7000DriverPack
--! 
--! Zynq7000DriverPack is free software; you can redistribute it and/or modify it under the terms of
--! the GNU General Public License as published by the Free Software Foundation; either version 3 of
--! the License, or any later version.
--! 
--! Zynq7000DriverPack is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
--! without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
--! GNU General Public License for more details.
--! 
--! You should have received a copy of the GNU General Public License along with this program; if not,
--! write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
--! USA.
--!
--! @addtogroup myGPIO
--! @{
--! @addtogroup GPIO-single
--! @{


library ieee;
use ieee.std_logic_1164.all;

--! @brief  Cella base GPIO
entity GPIOsingle is
    Port (	GPIO_enable		: in 	std_logic;	--! 
    --! segnale di abilitazione, permette di pilotare la linea "GPIO_inout".
	--!	Quando GPIO_enable=1, la linea GPIO_inout e quella GPIO_write sono connesse tra loro, consentendo
	--! la scrittura del valore del segnale GPIO_inout
        	GPIO_write 		: in 	std_logic;	--! 
    --! segnale di input, diretto verso l'esterno del device GPIOsingle.
    --! Quando GPIO_enable=1, la linea GPIO_inout e quella GPIO_write sono connesse tra loro, consentendo
	--! la scrittura del valore del pin GPIO_inout.
        	GPIO_inout 		: inout std_logic;	--!
    --! segnale bidirezionale diretto verso l'esterno del device. Può essere usato per leggere/scrivere
    --! segnali digitali da/verso l'esterno del device GPIOsingle
        	GPIO_read 		: out 	std_logic);	--! 
    --! segnale di output, diretto verso l'esterno del device.
    --! Quando GPIO_enable=1, la linea GPIO_inout e quella GPIO_write sono connesse tra loro, consentendo
	--! la scrittura del valore dei pin, per cui questo segnale assume esattamente il valore con cui viene
	--! impostato il segnale GPIO_write
end GPIOsingle;

architecture Structural of GPIOsingle is
begin
	GPIO_read <= GPIO_inout;
	with GPIO_enable select
		GPIO_inout <= 	GPIO_write when '1',
						'Z' when others;
end Structural;

--! @}
--! @}
