LIBRARY IEEE;
USE IEEE.std_logic_1164.ALL;
USE IEEE.numeric_std.ALL;
USE IEEE.std_logic_arith.ALL;

ENTITY reg_1 IS

    PORT (
        r1CLK, r1LOAD : IN STD_LOGIC;
	    carry : IN STD_LOGIC;
        r1in : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
        r1out : OUT STD_LOGIC;
        --
        out4: OUT std_logic_vector(3 downto 0)
    );

END reg_1;

ARCHITECTURE rtl OF reg_1 IS

    SIGNAL reg1 : STD_LOGIC_VECTOR(3 DOWNTO 0); 
    SIGNAL temp : std_logic;

BEGIN
    reg : PROCESS (r1CLK, r1LOAD)
    BEGIN
        IF r1CLK'event AND r1CLK = '1' THEN
            IF r1LOAD = '1' THEN
                reg1 <= r1in;
		
            ELSE
		temp <= reg1(0);
                reg1(2 DOWNTO 0) <= reg1(3 DOWNTO 1);
                reg1(3) <= carry;
            END IF;
        END IF;
    END PROCESS;
	r1out <= temp;
    out4 <= reg1;
END rtl;