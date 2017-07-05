--! @file GPIOarray.vhd
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
--! @addtogroup GPIO-array
--! @{



library ieee;
use ieee.std_logic_1164.all;

--! @brief Array di celle GPIO, pilotabili singolarmente
entity GPIOarray is
	Generic (	GPIO_width 		: 		natural := 4); --! 
	--!	numero di istanze GPIO create, di default pari a 4 celle.
    Port 	(	GPIO_enable		: in 	std_logic_vector (GPIO_width-1 downto 0);	--! 
    --! segnale di abilitazione, permette di pilotare la linea "GPIO_inout".
	--!	Quando GPIO_enable(i)=1, la linea GPIO_inout(i) e quella GPIO_write(i) sono connesse tra loro, consentendo
	--! la scrittura del valore del pin.
           		GPIO_write 		: in 	std_logic_vector (GPIO_width-1 downto 0);	--! 
    --! segnale di input, diretto verso l'esterno del device.
    --! Quando GPIO_enable(i)=1, la linea GPIO_inout(i) e quella GPIO_write(i) sono connesse tra loro, consentendo
	--! la scrittura del valore del pin.
           		GPIO_inout	 	: inout std_logic_vector (GPIO_width-1 downto 0);	--!
    --! segnale bidirezionale diretto verso l'esterno del device. Può essere usato per leggere/scrivere
    --! segnali digitali da/verso l'esterno del device.
           		GPIO_read 		: out 	std_logic_vector (GPIO_width-1 downto 0));	--! 
    --! segnale di output, diretto verso l'esterno del device.
    --! Quando GPIO_enable(i)=1, la linea GPIO_inout(i) e quella GPIO_write(i) sono connesse tra loro, consentendo
	--! la scrittura del valore del pin, per cui questo segnale assume esattamente il valore con cui viene
	--! impostato il segnale GPIO_write(i). Se GPIO_enable(i)=0, il valore del segnale può essere forzato dall'
	--! esterno del device.
end GPIOarray;


architecture Structural of GPIOarray is
	component GPIOsingle is
		Port (	GPIO_enable		: in 	std_logic;
	 			GPIO_write 		: in 	std_logic;
	 			GPIO_inout 		: inout std_logic;
	 			GPIO_read 		: out 	std_logic);
	end component;
	
begin

	gpio_array : for i in GPIO_width-1 downto 0 generate
		single_gpio : GPIOsingle
			Port map(	GPIO_enable		=> GPIO_enable(i),
						GPIO_write		=> GPIO_write(i),
						GPIO_inout		=> GPIO_inout(i),
						GPIO_read		=> GPIO_read(i));
	end generate;
		
 end Structural;
 
 --! @}
 --! @}
