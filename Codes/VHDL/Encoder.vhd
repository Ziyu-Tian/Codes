Library IEEE;
use IEEE.std_logic_1164.all;

entity pri_enc is 
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
end pri_enc;

architecture rtl of pri_enc is 
	signal enc_in: std_logic_vector(0 to 5);
    signal enc_out: std_logic_vector(0 to 2);


begin 
	enc_in <= a & b & c & d & e & f;
    
    with enc_in select
    	enc_out <= "000" when "000000",
        "001" when "000001",
        "010" when "00001-",
        "011" when "0001--",
        "100" when "001---",
        "101" when "01----",
        "110" when "1-----",
        "---" when others;
        
      f0 <= enc_out(0);
      f1 <= enc_out(1);
      f2 <= enc_out(2);
end rtl;