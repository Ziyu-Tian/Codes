library IEEE;
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

component half_adder 

port ( 
        a: in std_logic;
        b: in std_logic;
        c: out std_logic;
        s: out std_logic
);
end component;

-- internal signals declaration 

signal PP0, PP1, PP2, PP3: std_logic_vector(3 downto 0);

-- PP for signal of partial-product after AND gates

signal C1, C2, C3, C4: std_logic_vector(2 downto 0);

-- C1,C2,C3 for 12 internal carry 

signal P1, P2, P3, P4: std_logic_vector(3 downto 0);

-- P1, P2, P3 for 22 internal sum 

begin
    -- the input of partial-product 
PP0(0) <= A(0) and B(0);
PP0(1) <= A(1) and B(0);
PP0(2) <= A(2) and B(0);
PP0(3) <= A(3) and B(0);

PP1(0) <= A(0) and B(1);
PP1(1) <= A(1) and B(1);
PP1(2) <= A(2) and B(1);
PP1(3) <= A(3) and B(1);

PP2(0) <= A(0) and B(2);
PP2(1) <= A(1) and B(2);
PP2(2) <= A(2) and B(2);
PP2(3) <= A(3) and B(2);

PP3(0) <= A(0) and B(3);
PP3(1) <= A(1) and B(3);
PP3(2) <= A(2) and B(3);
PP3(3) <= A(3) and B(3);

-- instantiate the FA and HA (s)
-- 8 FA and 4 HA 

HA1: half_adder
    port map(
        PP0(1), PP1(0), C1(0), P1(0)
    );

HA2: half_adder
    port map(
        PP1(1), PP0(2), C1(1), P1(1)
    );

HA3: half_adder
    port map(
        PP0(3), PP1(2), C1(2), P1(2)
    );

HA4: half_adder
    port map(
        C3(0), P3(1), C4(0), P4(0)
    );

FA1: full_adder 
    port map(
        PP2(0), P1(1), C1(0), C2(0), P2(0)
    );

FA2: full_adder
    port map(
        PP3(0), P2(1), C2(0), C3(0), P3(0)
    );

FA3: full_adder
    port map(
        PP2(1), P1(2), C1(1), C2(1), P2(1)
    );


FA4: full_adder
    port map(
        PP2(2), P1(3), C1(2), C2(2), P2(2)
    );

FA5: full_adder
    port map(
        PP3(1), P2(2), C2(1), C3(1), P3(1)
    );

FA6: full_adder
    port map(
        C4(0), P3(2), C3(1), C4(1), P4(1)
    );

FA7: full_adder
    port map(
        C4(1), P3(3), C3(2), C4(2), P4(2)
    );

FA8: full_adder
    port map(
        PP3(2), P2(3), C2(2), C3(2), P3(2)
    );

-- Single wire connection 

P1(3) <= PP1(3);

P2(3) <= PP2(3);

P3(3) <= PP3(3);

P4(3) <= C4(2);

-- Multiplier result 

S(0) <= PP0(0);

S(1) <= P1(0);

S(2) <= P2(0);

S(3) <= P3(0);

S(4) <= P4(0);

S(5) <= P4(1);

S(6) <= P4(2);

S(7) <= P4(3);

end structural;
