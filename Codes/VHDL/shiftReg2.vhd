LIBRARY IEEE;
USE IEEE.std_logic_1164.ALL;
USE IEEE.numeric_std.ALL;
USE IEEE.std_logic_arith.ALL;

ENTITY reg_2 IS

    PORT (
        r2CLK, r2CLR, carry : IN STD_LOGIC;
        r2in : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
        Cout : OUT STD_LOGIC;
        r2OUT : OUT STD_LOGIC_VECTOR(3 DOWNTO 0)
    );

END reg_2;

ARCHITECTURE rtl OF reg_2 IS

    SIGNAL reg2 : STD_LOGIC_VECTOR(3 DOWNTO 0);

BEGIN
    reg : PROCESS (r2CLK, r2CLR)
    BEGIN

        IF (r2CLR = '1') THEN
            reg2 <= (OTHERS => '0');
        ELSIF (r2CLK'event AND r2CLK = '1') THEN
            reg2(3)<= carry;
	    reg2(2)<= r2in(3);
	    reg2(1)<= r2in(2);
	    reg2(0)<= r2in(1);
        END IF;
    END PROCESS;
    r2OUT <= reg2;
    Cout <= r2in(0);
END rtl;