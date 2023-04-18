library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity single_Multiplier is 

    port(
        m1X: in std_logic;
        m1Y: in std_logic_vector(3 downto 0);
        m1OUT: out std_logic_vector(3 downto 0)
    );

end single_Multiplier;

architecture rtl of single_Multiplier is    

begin 
    M1: process(m1X,m1Y)
    begin
        for i in 0 to 3 loop
            m1OUT(i) <= m1Y(i) and m1X;
            end loop;
    end process M1;

end rtl ; -- rtl