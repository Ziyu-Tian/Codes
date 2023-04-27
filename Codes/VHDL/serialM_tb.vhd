LIBRARY IEEE;
USE IEEE.std_logic_1164.ALL;
USE IEEE.numeric_std.ALL;

ENTITY serial_tb IS

END ENTITY;

ARCHITECTURE behaviour OF serial_tb IS

    COMPONENT serial_multi IS
        PORT (
            CLK, START : IN STD_LOGIC;
            R, D : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
            result : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
        );
    END COMPONENT;

    SIGNAL CLK : STD_LOGIC := '0';
    SIGNAL START : STD_LOGIC := '0';
    SIGNAL R : STD_LOGIC_VECTOR(3 DOWNTO 0);
    SIGNAL D : STD_LOGIC_VECTOR(3 DOWNTO 0);
    SIGNAL result : STD_LOGIC_VECTOR(7 DOWNTO 0);

    CONSTANT clk_period : TIME := 100 ns;

BEGIN
    UUT : serial_multi
    PORT MAP
    (
        CLK, START, R, D, result
    );
	
    clk_gen : PROCESS
    BEGIN
        CLK <= '0';
        WAIT FOR clk_period/2;
        CLK <= '1';
        WAIT FOR clk_period/2;
        CLK <= '0';
        WAIT FOR clk_period/2;
        CLK <= '1';
        WAIT FOR clk_period/2;
        CLK <= '0';
        WAIT FOR clk_period/2;
        CLK <= '1';
        WAIT FOR clk_period/2;
        CLK <= '0';
        WAIT FOR clk_period/2;
        CLK <= '1';
        WAIT FOR clk_period/2;
        CLK <= '0';
        WAIT FOR clk_period/2;
        CLK <= '1';
        WAIT FOR clk_period/2;
        CLK <= '0';
        WAIT FOR clk_period/2;
           CLK <= '1';
        WAIT FOR clk_period/2;
        CLK <= '0';
        WAIT FOR clk_period/2;
        CLK <= '1';
        WAIT FOR clk_period/2;
        CLK <= '0';
        WAIT FOR clk_period/2;
        CLK <= '1';
        WAIT FOR clk_period/2;
        CLK <= '0';
        WAIT FOR clk_period/2;
        CLK <= '1';
        WAIT FOR clk_period/2;
        CLK <= '0';
        WAIT FOR clk_period/2;
        CLK <= '1';
        WAIT FOR clk_period/2;
        CLK <= '0';
        WAIT FOR clk_period/2;
         CLK <= '1';
        WAIT FOR clk_period/2;
        CLK <= '0';
        WAIT FOR clk_period/2;
        CLK <= '1';
        WAIT FOR clk_period/2;
        CLK <= '0';
        WAIT FOR clk_period/2;
        CLK <= '1';
        WAIT FOR clk_period/2;
        CLK <= '0';
        WAIT FOR clk_period/2;
        CLK <= '1';
        WAIT FOR clk_period/2;
        CLK <= '0';
        WAIT FOR clk_period/2;
        CLK <= '1';
        WAIT FOR clk_period/2;
        CLK <= '0';
        WAIT FOR clk_period/2;
        wait;
    END PROCESS; -- clk_gen
    
    start_gen : PROCESS
    BEGIN
        START <= '1';
        WAIT FOR 100 ns;
        START <= '0';
        wait for 400 ns;
        START <= '1';
        WAIT FOR 100 ns;
        START <= '0';
        wait for 400 ns;
        START <= '1';
        WAIT FOR 100 ns;
        START <= '0';
        wait for 400 ns;
        WAIT;
    END PROCESS; -- START_gen
    
    input : PROCESS
    BEGIN

        R <= "1111";
        D <= "1111";
        WAIT FOR 500 ns;
        R <= "1001";
        D <= "1101";
        WAIT FOR 500 ns;
        R <= "1100";
        D <= "1110";
        WAIT FOR 500 ns;
        WAIT;
    END PROCESS; -- input



END behaviour; -- behaviour