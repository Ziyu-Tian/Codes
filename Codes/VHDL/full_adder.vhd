library IEEE;
USE IEEE.std_logic_1164.ALL;

entity full_adder is 
    port (
            a: in std_logic;
            b: in std_logic;
            cin: in std_logic;  -- cin for input carry
            cout: out std_logic; -- cout for output carry
            s: out std_logic    -- s for sum
    );
end full_adder;

-- structural level of FF

architecture structural of full_adder is 

 -- quote the xor_gate deisgned before
    component xor_gate is 
    port(
            a: in std_logic;
            b: in std_logic;
            c: out std_logic
    );
    end component;

    -- quote the and_gate designed before

    component and_gate is 
    port(
            a: in std_logic;
            b: in std_logic;
            c: out std_logic
    );
    end component;

    -- quote the or_gate 

    component or_gate is 
    port(
            a: in std_logic;
            b: in std_logic;
            c: out std_logic
    );
    end component;

    -- define the internal signals

    signal a1 : std_logic;
    signal ax: std_logic;
    signal bx: std_logic;
    signal x1: std_logic;
    signal a2: std_logic;

    begin 
        -- instantiate the gates 
        AND1: and_gate
                port map( a=>a, b=>b, c=>a1);
        AND2: and_gate
                port map( a=>x1, b=>cin, c=>a2);
        XOR1: xor_gate
                port map( a=>a, b=>b, c=> x1);
        XOR2: xor_gate
                port map( a=>x1, b=>cin, c=>s);
        OR1: or_gate
                port map( a=>a1, b=>a2, c=>cout);
    end structural;

