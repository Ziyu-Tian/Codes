library IEEE;
USE IEEE.std_logic_1164.ALL;


ENTITY xor_gate IS
    PORT (
        A, B : IN STD_LOGIC;
        C : OUT STD_LOGIC
    );
END ENTITY;

ARCHITECTURE gate_level OF xor_gate IS

    SIGNAL An, Bn, t1, t2 : STD_LOGIC := 0;
BEGIN
    An <= NOT A;
    Bn <= NOT B;
    t1 <= An AND B;
    t2 <= Bn AND A;
    C <= t1 OR t2;
END ARCHITECTURE;