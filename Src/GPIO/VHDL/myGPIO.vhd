--! @file myGPIO.vhd
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

--! @addtogroup myGPIO
--! @{

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


--! @brief Periferica AXI4 Lite che implementa una GPIO pilotabile da processing-system.
--!
--! Registri della periferica
--! - MODE : consente di impostare i singoli GPIO come ingressi o uscite; solo i GPIO_width bit meno significativi del registro sono
--!   significativi; l'offset, rispetto all'indirizzo base della periferica e' 0;
--! - WRITE : consente di imporre un valore qualora i GPIO siano configurati come uscite; solo i GPIO_width bit meno significativi del
--!   registro sono significativi;  l'offset, rispetto all'indirizzo base della periferica e' 4;
--! - READ : consente di leggere il valore dei GPIO, sia quelli configurati come ingressi che quelli configurati come uscite; solo i
--!   GPIO_width bit meno significativi del registro sono significativi; l'offset, rispetto all'indirizzo base della periferica e' 8;
--! - S/C : registro di stato controllo; solo i tre bit meno significativi del registro sono significativi;
--!    - IntEn (bit 0): interrupt-enable, '1' abilita le interruzioni, '0' disabilita le interruzioni
--!    - Irq (bit 1): interrupt-request (sola lettura), '1' indica che la periferica ha generato una interruzione
--!    - IntAck (bit 2): interrupt-ack (clear, sola scrittura), consente di resettare il segnale interrupt-request, via software, dopo 
--!		 aver servito l'interruzione.
--!
--! @warning il segnale GPIO_inout viene mascherato in modo che solo i pin settati come input possano generare interruzione 
--!
entity myGPIO is
	generic (
		-- Users to add parameters here
		GPIO_width : natural := 4;	--! numero di GPIO offerti dalla periferica, di default pari a 4 celle.
		-- User parameters ends
		-- Do not modify the parameters beyond this line
		-- Parameters of Axi Slave Bus Interface S00_AXI
		C_S00_AXI_DATA_WIDTH : integer	:= 32;
		C_S00_AXI_ADDR_WIDTH : integer	:= 4
	);
	port (
		-- Users to add ports here
		GPIO_inout : inout std_logic_vector (GPIO_width-1 downto 0);	--! segnale bidirezionale diretto verso l'esterno del device.
		GPIO_int : out std_logic;										--! segnale di interrupt a livelli, se gli interrupt sono abilitati
																		--! diventa alto quando GPIO_inout cambia stato
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
	);
end myGPIO;

architecture arch_imp of myGPIO is

	-- component declaration
	component myGPIO_AXI is
		generic (
		GPIO_width 			: natural := 4;
		C_S_AXI_DATA_WIDTH	: integer	:= 32;
		C_S_AXI_ADDR_WIDTH	: integer	:= 4
		);
		port (
		GPIO_inout	 	: inout std_logic_vector (GPIO_width-1 downto 0);
		GPIO_int		: out std_logic;	
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

-- Instantiation of Axi Bus Interface S00_AXI
myGPIO_AXI_inst : myGPIO_AXI
	generic map (
		GPIO_width	=> GPIO_width,
		C_S_AXI_DATA_WIDTH	=> C_S00_AXI_DATA_WIDTH,
		C_S_AXI_ADDR_WIDTH	=> C_S00_AXI_ADDR_WIDTH
	)
	port map (
		GPIO_inout => GPIO_inout,
		GPIO_int => GPIO_int,
		S_AXI_ACLK	=> s00_axi_aclk,
		S_AXI_ARESETN	=> s00_axi_aresetn,
		S_AXI_AWADDR	=> s00_axi_awaddr,
		S_AXI_AWPROT	=> s00_axi_awprot,
		S_AXI_AWVALID	=> s00_axi_awvalid,
		S_AXI_AWREADY	=> s00_axi_awready,
		S_AXI_WDATA	=> s00_axi_wdata,
		S_AXI_WSTRB	=> s00_axi_wstrb,
		S_AXI_WVALID	=> s00_axi_wvalid,
		S_AXI_WREADY	=> s00_axi_wready,
		S_AXI_BRESP	=> s00_axi_bresp,
		S_AXI_BVALID	=> s00_axi_bvalid,
		S_AXI_BREADY	=> s00_axi_bready,
		S_AXI_ARADDR	=> s00_axi_araddr,
		S_AXI_ARPROT	=> s00_axi_arprot,
		S_AXI_ARVALID	=> s00_axi_arvalid,
		S_AXI_ARREADY	=> s00_axi_arready,
		S_AXI_RDATA	=> s00_axi_rdata,
		S_AXI_RRESP	=> s00_axi_rresp,
		S_AXI_RVALID	=> s00_axi_rvalid,
		S_AXI_RREADY	=> s00_axi_rready
	);

	-- Add user logic here

	-- User logic ends

end arch_imp;

--! @}
