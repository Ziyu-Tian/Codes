LIBRARY IEEE;
USE IEEE.std_logic_1164.ALL;

ENTITY tb_coder2 IS
END ENTITY;

ARCHITECTURE rtl OF tb_coder2 IS

    COMPONENT sec_enc IS
        PORT (
            f0,f1,f2 : IN STD_LOGIC;
            a : OUT STD_LOGIC;
            b : OUT STD_LOGIC;
            c : OUT STD_LOGIC;
            d : OUT STD_LOGIC;
            e : OUT STD_LOGIC;
            f : OUT STD_LOGIC;
            g : OUT STD_LOGIC;
            h : OUT STD_LOGIC
        );
    END COMPONENT;
    SIGNAL a, b, c, d, e, f, g, h, f0, f1, f2 : STD_LOGIC := '0';

BEGIN

    UUT : sec_enc PORT MAP(
        a, b, c, d, e, f, f0, f1, f2
    );

    stimulus : PROCESS
    BEGIN
        f0 <= '0';
        f1 <= '0';
        f2 <= '0';
        WAIT FOR 1 ns;
        f0 <= '0';
        f1 <= '0';
        f2 <= '1';
        WAIT FOR 1 ns;
        f0 <= '0';
        f1 <= '1';
        f2 <= '0';
        WAIT FOR 1 ns;
        f0 <= '0';
        f1 <= '1';
        f2 <= '1';
        WAIT FOR 1 ns;
        f0 <= '1';
        f1 <= '1';
        f2 <= '1';
        WAIT;
    END PROCESS;
END ARCHITECTURE;