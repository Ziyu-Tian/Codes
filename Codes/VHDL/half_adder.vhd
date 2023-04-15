library IEEE;
USE IEEE.std_logic_1164.ALL;

entity half_adder is 
    port (
            a: in std_logic;
            b: in std_logic;
            c: out std_logic; -- c for carry
            s: out std_logic    -- s for sum
    );

end half_adder;

-- structural level (logic gate level) of HF

architecture structural of half_adder is 

    -- quote the xor_gate deisgned before
    component xor_gate is 
    port(
            a: in std_logic;
            b: in std_logic;
            c: out std_logic
    );
    end component;

    -- quote the or_gate designed before

    component and_gate is 
    port(
            a: in std_logic;
            b: in std_logic;
            c: out std_logic
    );
    end component;

    begin 
        -- instantiate the and_gate as "A1"
        A1: and_gate
                port map (a=>a, b=>b, c=>c);
        -- instantiate the xor_gate as "X1"
        X1: xor_gate
                port map ( a=>a, b=>b,c=>s);
    end structural;

    
