library library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity parallelMultiplier is 

port (
        A: in std_logic_vector(3 downto 0);
        -- A, B is the first number and the second number
        B: in std_logic_vector(3 downto 0);
        S: out std_logic_vector(7 downto 0)
        -- S is the sum 
);

end parallelMultiplier;

architecture structural of parallelMultiplier is 

-- full_adder delcaration 

component full_adder 

port (
        a: in std_logic;
        b: in std_logic;
        cin: in  std_logic;
        cout: out std_logic;
        s: out std_logic
);
end component;


-- half_adder delcaration 
