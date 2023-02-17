LIBRARY IEEE;
USE IEEE.std_logic_1164.ALL;

ENTITY tb_coder IS
END ENTITY;

ARCHITECTURE rtl OF tb_coder IS

    COMPONENT pri_enc IS
        PORT (
            a, b, c, d, e, f : IN STD_LOGIC;
            f0 : OUT STD_LOGIC;
            f1 : OUT STD_LOGIC;
            f2 : OUT STD_LOGIC
        );
    END COMPONENT;
    SIGNAL a, b, c, d, e, f, f0, f1, f2 : STD_LOGIC := '0';

BEGIN

    UUT : pri_enc PORT MAP(
        a, b, c, d, e, f, f0, f1, f2;
    );

    stimulus : PROCESS
    BEGIN
        a <= '0';
        b <= '0';
        c <= '0';
        d <= '0';
        e <= '0';
        f <= '0';
        WAIT FOR 100 ns;
        a <= '0';
        b <= '0';
        c <= '0';
        d <= '0';
        e <= '0';
        f <= '1';
        WAIT FOR 100 ns;
        a <= '0';
        b <= '0';
        c <= '0';
        d <= '0';
        e <= '1';
        f <= '-';
        WAIT FOR 100 ns;
        a <= '0';
        b <= '0';
        c <= '0';
        d <= '1';
        e <= '-';
        f <= '-';
        WAIT;
    END PROCESS;
END ARCHITECTURE;