LIBRARY IEEE;
USE IEEE.std_logic_1164.ALL;
USE IEEE.numeric_std.ALL;
USE IEEE.std_logic_arith.ALL;

ENTITY reg_2 IS

    PORT (
        r2CLK, r2CLR: IN STD_LOGIC;
        r2in : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
	carry : IN STD_LOGIC;
        r2OUT : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
    );

END reg_2;

ARCHITECTURE rtl OF reg_2 IS

    SIGNAL reg2 : STD_LOGIC_VECTOR(7 DOWNTO 0);

BEGIN
    reg : PROCESS (r2CLK, r2CLR)
    BEGIN

        IF (r2CLR = '1') THEN
            reg2 <= (OTHERS => '0');
        ELSIF (r2CLK'event AND r2CLK = '1') THEN
	    reg2(2 downto 0)<= reg2(3 downto 1);
	    reg2(6 downto 3)<=r2in;
	    reg2(7) <= carry;
        END IF;
    END PROCESS;
    r2OUT <= reg2;
END rtl;