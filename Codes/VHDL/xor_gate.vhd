library IEEE;
USE IEEE.std_logic_1164.ALL;

-- entity of xor_gate 

entity xor_gate is 

port (
        a: in std_logic;
        b: in std_logic;
        c: out std_logic
    );

end xor_gate;

-- architecture in RTL of xor_gate 

architecture rtl of xor_gate is 

begin 

c <= a xor b;

end rtl;


