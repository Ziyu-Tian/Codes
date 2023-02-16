library IEEE;
use IEEE.std_logic_1164.all;

entity tb_coder is
end entity tb_coder;

architecture rtl of tb_coder is 

signal a,b,c,d,e,f,f0,f1,f2,f3:std_logic:='0';

	component pri_enc is 
		port(a: in std_logic;
         b: in std_logic;
         c: in std_logic;
         d: in std_logic;
         e: in std_logic;
         f: in std_logic;
         f0: out std_logic;
         f1: out std_logic;
         f2: out std_logic
        );
	end component;


begin

CODER : pri_enc port map(
	a =>a,
    b=>b,
    c=>c,
    d=>d,
    e=>e,
    f=>f,
    f0 =>f0,
    f1=>f1,
    f2=>f2
    );
    
stimulus: process
begin
    a <= '0'; 
    b <= '0';
    c <= '0'; 
    d <= '0'; 
    e <= '0'; 
    f <= '0'; wait for 100 ns;
    a <= '0'; 
    b <= '0';
    c <= '0'; 
    d <= '0'; 
    e <= '0'; 
    f <= '1'; wait for 100 ns;
    a <= '0'; 
    b <= '0';
    c <= '0'; 
    d <= '0'; 
    e <= '1'; 
    f <= '-'; wait for 100 ns;
    wait;
end process;
	

end rtl;