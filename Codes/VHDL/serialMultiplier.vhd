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
        	r1in : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
        	r1out : OUT STD_LOGIC
        );
    END COMPONENT;

    COMPONENT reg_2
        PORT (
            	r2CLK, r2CLR: IN STD_LOGIC;
        	r2in : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
		carry : IN STD_LOGIC;
        	r2OUT : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
        );
    END COMPONENT;

    SIGNAL newstart, RS, AR2 : STD_LOGIC;
    SIGNAL AR1, SA : STD_LOGIC_VECTOR(3 DOWNTO 0);
    SIGNAL RA : STD_LOGIC_VECTOR(7 DOWNTO 0);

BEGIN
    result <= RA;
    multi : PROCESS (CLK, START)
    BEGIN
        IF START = '1' THEN
            newstart <= '1';

        ELSIF CLK = '0' THEN
            newstart <= '0';
        END IF;
    END PROCESS;

    M1 : single_Multiplier
    PORT MAP(
        RS, D, SA
    );
    A1 : adder4
    PORT MAP(
        SA, RA(7 downto 4), AR1, AR2
    );
    R1 : reg_1
    PORT MAP(
        CLK, newstart, R, RS
    );
    R2 : reg_2
    PORT MAP(
        CLK, newstart, AR1, AR2, RA
    );


END structural; -- structural