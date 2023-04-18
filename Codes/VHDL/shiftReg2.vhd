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
       

        IF r2CLK'event AND r2CLK = '1' THEN
            reg2 <= r2in;
            Cout <= reg2(0);
            reg2(2 DOWNTO 0) <= reg2(3 DOWNTO 1);
            reg2(3) <= carry;
        END IF; 
        
        IF r2CLR = '1' THEN
            reg2 <= "0000";
        END IF;
END PROCESS;
r2OUT <= reg2;
END rtl;