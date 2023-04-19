LIBRARY IEEE;
USE IEEE.std_logic_1164.ALL;
USE IEEE.numeric_std.ALL;

ENTITY serial_multi IS

    PORT (
        CLK, START : IN STD_LOGIC;
        R, D : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
        result : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
    );

END serial_multi;

ARCHITECTURE structural OF serial_multi IS

    COMPONENT single_Multiplier
        PORT (
            m1X : IN STD_LOGIC;
            m1Y : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
            m1OUT : OUT STD_LOGIC_VECTOR(3 DOWNTO 0)
        );
    END COMPONENT;

    COMPONENT adder4
        PORT (
            aIN1 : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
            aIN2 : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
            aOUT : OUT STD_LOGIC_VECTOR(3 DOWNTO 0);
            cout : OUT STD_LOGIC
        );
    END COMPONENT;

    COMPONENT reg_1
        PORT (
            r1CLK, r1LOAD : IN STD_LOGIC;
            carry : IN STD_LOGIC;
            r1in : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
            r1out : OUT STD_LOGIC;
            out4 : OUT STD_LOGIC_VECTOR(3 DOWNTO 0)
        );
    END COMPONENT;

    COMPONENT reg_2
        PORT (
            r2CLK, r2CLR, carry : IN STD_LOGIC;
            r2in : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
            Cout : OUT STD_LOGIC;
            r2OUT : OUT STD_LOGIC_VECTOR(3 DOWNTO 0)
        );
    END COMPONENT;

    SIGNAL newstart, RS, RR, AR2 : STD_LOGIC;
    SIGNAL AR1, SA, RA : STD_LOGIC_VECTOR(3 DOWNTO 0);
    SIGNAL final : STD_LOGIC_VECTOR(7 DOWNTO 0);
    SIGNAL temp : STD_LOGIC_VECTOR(3 DOWNTO 0);

BEGIN
    multi : PROCESS (CLK, START)
    BEGIN
        IF START = '1' THEN
            newstart <= '1';

        ELSIF CLK = '0' THEN
            newstart <= '0';

        ELSIF (CLK'event AND CLK = '1') THEN
            final(7 DOWNTO 4) <= RA;
            final(3 DOWNTO 0) <= temp;
        END IF;
    END PROCESS;

    M1 : single_Multiplier
    PORT MAP(
        RS, D, SA
    );
    A1 : adder4
    PORT MAP(
        SA, RA, AR1, AR2
    );
    R1 : reg_1
    PORT MAP(
        CLK, newstart, RR, R, RS, temp
    );
    R2 : reg_2
    PORT MAP(
        CLK, newstart, AR2, AR1, RR, RA
    );

    -- output connection

    result <= final;

END structural; -- structural