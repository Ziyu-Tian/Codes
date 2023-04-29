LIBRARY IEEE;
USE IEEE.std_logic_1164.ALL;
USE IEEE.numeric_std.ALL;

ENTITY parallel_tb IS

END ENTITY;

ARCHITECTURE behaviour OF parallel_tb IS

    COMPONENT parallelMultiplier IS
        PORT (
            A : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
            -- A, B is the first number and the second number
            B : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
            S : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
            -- S is the sum 
        );
    END COMPONENT;
    SIGNAL A : STD_LOGIC_VECTOR(3 DOWNTO 0) := (OTHERS => '0');
    SIGNAL B : STD_LOGIC_VECTOR(3 DOWNTO 0) := (OTHERS => '0');

    SIGNAL S : STD_LOGIC_VECTOR (7 DOWNTO 0) := (OTHERS => '0');

BEGIN
    UUT : parallelMultiplier
    PORT MAP(A, B, S);

    stimulus : PROCESS

    BEGIN
        A <= "0110";
        B <= "1100";

        WAIT FOR 100 ns;

        A <= "0010";
        B <= "0001";

        WAIT FOR 100 ns;
        A <= "0111";
        B <= "0101";

        WAIT FOR 100 ns;
        A <= "0100";
        B <= "1101";

        WAIT FOR 100 ns;
        A <= "1010";
        B <= "1110";

        WAIT FOR 100 ns;
        A <= "1011";
        B <= "1011";

        WAIT FOR 100 ns;
        WAIT;
    END PROCESS;
END ARCHITECTURE;