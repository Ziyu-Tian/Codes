library IEEE;
USE IEEE.std_logic_1164.ALL;
USE IEEE.numeric_std.ALL;

ENTITY xor_tb IS

END ENTITY;

ARCHITECTURE behaviour OF xor_tb IS

    COMPONENT xor_gate IS
        PORT (
            A, B : IN STD_LOGIC;
            C : OUT STD_LOGIC
        );
    END COMPONENT;

    SIGNAL A, B, C : STD_LOGIC := '0';
BEGIN

    UUT : xor_gate PORT MAP(A, B, C);

    stimulus : PROCESS
    BEGIN
        A <= '0';
        B <= '0';
        WAIT FOR 100 ns;
        A <= '0';
        B <= '1';
        WAIT FOR 100 ns;
        A <= '1';
        B <= '1';
        WAIT;
    END PROCESS;
END ARCHITECTURE; -- behaviour