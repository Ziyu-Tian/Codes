library IEEE;
USE IEEE.std_logic_1164.ALL;

-- entity of or_gate

entity or_gate is 

port (
        a: in std_logic;
        b: in std_logic;
        c: out std_logic
    );

end or_gate;

-- archiecture in RTL of or_gate 

architecture rtl of or_gate is 

begin 

c <= a or b;

end rtl;