library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity parity_encoder is
    port(
        a , b, c: in std_logic;
        x: out std_logic
    );
end entity;

architecture rtl of parity_encoder is
  
    component NOT_GATE is
    PORT(a: in std_logic;
        x: out std_logic
    );
    end component;

    component  AND_GATE is 
    port(
        a,b,c: in std_logic;
        x: out std_logic
    );
    end component;

    component OR_GATE is 
    port(
        a,b,c,d: in std_logic;
        x: out std_logic
    );
    end component;

signal 



