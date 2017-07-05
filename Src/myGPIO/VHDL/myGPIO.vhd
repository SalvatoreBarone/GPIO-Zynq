--! @file muGPIO_AXI.vhd
--! @author Salvatore Barone <salvator.barone@gmail.com>
--! @date 22 06 2017
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
--! @addtogroup AXI-device
--! @{


library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

--! @mainpage
--! @brief Periferica AXI4 Lite che implementa una GPIO pilotabile da processing-system.
--!
--! @details
--!
--! <h4>Registri interni del device</h4>
--! Il device possiede i registri indicati di seguito. Per oognuno di essi viene indicata la modalità di
--! accesso (R sola lettura, W sola scrittura, R/W lettura scrittura), e l'offset, rispetto all'indirizzo
--! base del device, col quale è possibile indirizzarli.
--!
--!  - MODE (R/W, offset +0x0): consente di impostare i singoli pin del device come ingressi o uscite; solo i
--!    GPIO_width bit meno significativi del registro hanno significato, agire sui restanti bit non produce
--!    nessun effetto; Il valore che i singoli pin possono
--!    assumere è:
--!			- '1': il pin viene configurato come pin di uscita;
--!			- 'ò: il pin viene configurato come pin di ingresso;
--!			.
--!  - WRITE (R/W, offset +0x4): consente di imporre un valore ai pin del device, qualora essi siano configurati
--!    come uscite; solo i GPIO_width bit meno significativi del hanno significato, agire sui restanti bit non produce
--!    nessun effetto;
--!  - READ (R, offset +0x8): consente di leggere il valore dei pin del device, sia quelli configurati come
--!    ingressi che quelli configurati come uscite (il cui valore coincide con quello settato nel registro
--!    WRITE); solo i GPIO_width bit meno significativi del registro hanno significato, gli altri vengono letti
--!    zero;
--!  - GIES (Global Interrupt Enable/Status, R/W, offset 0xC): Consente di abilitare/disabilitare gli interrupt
--!    globali della periferica; solo due dei bit sono significativi:
--!	   		- IE (bit 0): interrupt enable, abilita gli interrupt, può essere scritto e letto; se posto ad '1'
--!			  la periferica potrà generare interrupt quando uno dei pin impostati come ingresso assume
--!			  valore '1' (ed il corrispondente bit in PIE è impostato ad '1'); se posto a '0' il device
--!			  non genererà mai interruzioni;
--!			- IS (bit 1): interrupt status, settato internamente ad '1' nel caso in cui la periferica abbia
--!			  generato interrupt; replica del segnale "interrupt" diretto verso il processing-system.
--!  - PIE (Pin Interrupt Enable, R/W, offset 0x10): consente di abilitare/disabilitare gli interrupt per i
--!	   singoli pin. Con GIES(0)='1' e MODE(n)='0' (cioè se gli interrupt globali sono abilitati e il pin
--!	   n-esimo è configurato come input), se PIE(n)='1' allora il device genererà un interrupt verso il
--!	   processing-system quando il pin n-esimo assumerà valore '1', mentre, se PIE(n)='0' non verrà
--!	   generata una interruzione;
--!  - IRQ (Interrupt Request, R, offset 0x14): IRQ(n)='1' indica che la sorgente di interruzione è il bit
--!    n-esimo; la or-reduce di tale registro costituisce il segnale "interrupt" diretto verso il processing
--!    system;
--!  - IACK (Interrupt Ack, W, offset 0x18): imponento IACK(n)='1' è possibile segnalare al device che
--!    l'interruzione generata dal in n-esimo è stata servita; il bit IRQ(n) verrà resettato automaticamente. 
--!
--!
--! <h4>Process di scrittura dei registri della periferica</h4>
--! Il process che implementa la logica di scrittura dei registri è stato modificato in modo da ottenere
--! il seguente indirizzamento:
--! <table>
--! <tr><th>Indirizzo</th><th>Offset</th><th>Registro</th></tr>
--! <tr><td>b"00000"</td><td>0x00</td><td>MODE</td></tr>
--! <tr><td>b"00100"</td><td>0x04</td><td>WRITE</td></tr>
--! <tr><td>b"01000"</td><td>0x08</td><td>READ(*)</td></tr>
--! <tr><td>b"01100"</td><td>0x0C</td><td>GIES(**)</td></tr>
--! <tr><td>b"10000"</td><td>0x10</td><td>PIE</td></tr>
--! <tr><td>b"10100"</td><td>0x14</td><td>IRQ(***)</td></tr>
--! <tr><td>b"11000"</td><td>0x18</td><td>IACK(****)</td></tr>
--! </table>
--! (*) Il registro READ è a sola lettura: le scritture su questo registro non producono effetti;
--!  la scrittura, infatti, avviene su slv_reg2, che è inutilizzato;<br>
--! (**) La scrittura ha effetto solo sul bit zero del registro;<br>
--! (***) Il registro IRQ è a sola lettura: le scritture su questo registro non producono effetti;
--!  la scrittura, infatti, avviene su slv_reg5, che è inutilizzato;<br>
--! (****) La scrittura su IACK è fittizzia, nel senso che appena si smette di indirizzare il registro,
--! esso assume valore zero;<br>
--!
--!
--! <h4>Process di lettura dei registri della periferica</h4>
--! Il process che implementa la logica di lettura dei registri è stato modificato in modo da ottenere
--! il seguente indirizzamento:
--! <table>
--! <tr><th>Indirizzo</th><th>Offset</th><th>Registro</th></tr>
--! <tr><td>b"00000"</td><td>0x00</td><td>MODE</td></tr>
--! <tr><td>b"00100"</td><td>0x04</td><td>WRITE</td></tr>
--! <tr><td>b"01000"</td><td>0x08</td><td>READ(*)</td></tr>
--! <tr><td>b"01100"</td><td>0x0C</td><td>GIES(**)</td></tr>
--! <tr><td>b"10000"</td><td>0x10</td><td>PIE</td></tr>
--! <tr><td>b"10100"</td><td>0x14</td><td>IRQ</td></tr>
--! <tr><td>b"11000"</td><td>0x18</td><td>IACK(***)</td></tr>
--! </table>
--! (*) Il registro READ è direttamente connesso alla porta GPIO_inout<br>
--! (**) Il bit 2 di GIES è il flag "interrupt", che vale '1' nel caso in cui la periferica abbia generato
--! interrupt ancora non gestiti.<br>
--! (***) Viene letto sempre zero, dal momento che la scrittura su tale registro è fittizzia.
--!
--!
--! <h4>Process di scrittura su IRQ</h4>	
--! La logica di scrittura su IRQ è semplice (non viene scritto come un normale registro, ma pilotato
--! internamente dalla periferica):
--! se uno dei bit di GPIO_inout_masked è '1', (la or-reduce è 1) allora il valore del segnale GPIO_inout_masked
--! viene posto in bitwise-or con il valore attuale del registro IRQ, in modo da non resettare i bit di quest'
--! ultimo che siano stati settati a seguito di una interruzione non ancora servita
--! se uno dei bit di IACK è '1' (la or-reduce è '1'), allora il nuovo valore del registro IRQ viene ottenuto
--!   - mascherando IACK con l'attuale valore di IRQ, in modo da non effettuare il set di bit resettati
--!   - ponendo in XOR la maschera precedente con il valore attuale del registro	

entity myGPIO is
	generic (
		-- Users to add parameters here
		GPIO_width : natural := 4;	--! numero di GPIO offerti dalla periferica, di default pari a 4 celle.
		-- User parameters ends
		-- Do not modify the parameters beyond this line

		--! @cond
		-- Parameters of Axi Slave Bus Interface S00_AXI
		C_S00_AXI_DATA_WIDTH	: integer	:= 32;
		C_S00_AXI_ADDR_WIDTH	: integer	:= 5
		--! @endcond
	);
	port (
		-- Users to add ports here
		GPIO_inout : inout std_logic_vector (GPIO_width-1 downto 0); --! 
		--! segnale bidirezionale diretto verso l'esterno del device.
		
		interrupt : out std_logic; --!							
		--! segnale di interrupt a livelli diretto verso il processing - system. Se le interruzioni sono
		--! abilitate ed uno dei pin del device è settato come input ed è abilitato a generare interruzioni,
		--! diventa '1' appena tale pin assume valore '1', e mantiene tale valore fino a quando tutte le
		--! interruzioni non siano state servite.
		
		--! @cond
		-- User ports ends
		-- Do not modify the ports beyond this line


		-- Ports of Axi Slave Bus Interface S00_AXI
		s00_axi_aclk	: in std_logic;
		s00_axi_aresetn	: in std_logic;
		s00_axi_awaddr	: in std_logic_vector(C_S00_AXI_ADDR_WIDTH-1 downto 0);
		s00_axi_awprot	: in std_logic_vector(2 downto 0);
		s00_axi_awvalid	: in std_logic;
		s00_axi_awready	: out std_logic;
		s00_axi_wdata	: in std_logic_vector(C_S00_AXI_DATA_WIDTH-1 downto 0);
		s00_axi_wstrb	: in std_logic_vector((C_S00_AXI_DATA_WIDTH/8)-1 downto 0);
		s00_axi_wvalid	: in std_logic;
		s00_axi_wready	: out std_logic;
		s00_axi_bresp	: out std_logic_vector(1 downto 0);
		s00_axi_bvalid	: out std_logic;
		s00_axi_bready	: in std_logic;
		s00_axi_araddr	: in std_logic_vector(C_S00_AXI_ADDR_WIDTH-1 downto 0);
		s00_axi_arprot	: in std_logic_vector(2 downto 0);
		s00_axi_arvalid	: in std_logic;
		s00_axi_arready	: out std_logic;
		s00_axi_rdata	: out std_logic_vector(C_S00_AXI_DATA_WIDTH-1 downto 0);
		s00_axi_rresp	: out std_logic_vector(1 downto 0);
		s00_axi_rvalid	: out std_logic;
		s00_axi_rready	: in std_logic
		--! @endcond
	);
end myGPIO;

--! @cond
architecture arch_imp of myGPIO is

	-- component declaration
	component myGPIO_AXI is
		generic (
		GPIO_width 			: natural := 4;
		C_S_AXI_DATA_WIDTH	: integer	:= 32;
		C_S_AXI_ADDR_WIDTH	: integer	:= 5
		);
		port (
		GPIO_inout 		: inout std_logic_vector (GPIO_width-1 downto 0);
		interrupt 		: out std_logic;
		S_AXI_ACLK		: in std_logic;
		S_AXI_ARESETN	: in std_logic;
		S_AXI_AWADDR	: in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
		S_AXI_AWPROT	: in std_logic_vector(2 downto 0);
		S_AXI_AWVALID	: in std_logic;
		S_AXI_AWREADY	: out std_logic;
		S_AXI_WDATA		: in std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
		S_AXI_WSTRB		: in std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0);
		S_AXI_WVALID	: in std_logic;
		S_AXI_WREADY	: out std_logic;
		S_AXI_BRESP		: out std_logic_vector(1 downto 0);
		S_AXI_BVALID	: out std_logic;
		S_AXI_BREADY	: in std_logic;
		S_AXI_ARADDR	: in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
		S_AXI_ARPROT	: in std_logic_vector(2 downto 0);
		S_AXI_ARVALID	: in std_logic;
		S_AXI_ARREADY	: out std_logic;
		S_AXI_RDATA		: out std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
		S_AXI_RRESP		: out std_logic_vector(1 downto 0);
		S_AXI_RVALID	: out std_logic;
		S_AXI_RREADY	: in std_logic
		);
	end component myGPIO_AXI;

begin


myGPIO_AXI_inst : myGPIO_AXI
	generic map (
		GPIO_width			=> GPIO_width,
		C_S_AXI_DATA_WIDTH	=> C_S00_AXI_DATA_WIDTH,
		C_S_AXI_ADDR_WIDTH	=> C_S00_AXI_ADDR_WIDTH
	)
	port map (
		GPIO_inout 		=> GPIO_inout,
		interrupt 		=> interrupt,
		S_AXI_ACLK		=> s00_axi_aclk,
		S_AXI_ARESETN	=> s00_axi_aresetn,
		S_AXI_AWADDR	=> s00_axi_awaddr,
		S_AXI_AWPROT	=> s00_axi_awprot,
		S_AXI_AWVALID	=> s00_axi_awvalid,
		S_AXI_AWREADY	=> s00_axi_awready,
		S_AXI_WDATA		=> s00_axi_wdata,
		S_AXI_WSTRB		=> s00_axi_wstrb,
		S_AXI_WVALID	=> s00_axi_wvalid,
		S_AXI_WREADY	=> s00_axi_wready,
		S_AXI_BRESP		=> s00_axi_bresp,
		S_AXI_BVALID	=> s00_axi_bvalid,
		S_AXI_BREADY	=> s00_axi_bready,
		S_AXI_ARADDR	=> s00_axi_araddr,
		S_AXI_ARPROT	=> s00_axi_arprot,
		S_AXI_ARVALID	=> s00_axi_arvalid,
		S_AXI_ARREADY	=> s00_axi_arready,
		S_AXI_RDATA		=> s00_axi_rdata,
		S_AXI_RRESP		=> s00_axi_rresp,
		S_AXI_RVALID	=> s00_axi_rvalid,
		S_AXI_RREADY	=> s00_axi_rready
	);

	-- Add user logic here

	-- User logic ends

end arch_imp;

--! @endcond
--! @}
--! @}
