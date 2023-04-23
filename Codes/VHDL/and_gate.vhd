library IEEE;
USE IEEE.std_logic_1164.ALL;

-- entity of and_gate

entity and_gate is 

port (
        a: in std_logic;
        b: in std_logic;
        c: out std_logic
    );

end and_gate;

-- archiecture in RTL of and_gate 

architecture rtl of and_gate is 

begin 

c <= a and b;

end rtl;