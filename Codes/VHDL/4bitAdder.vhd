LIBRARY IEEE;
USE IEEE.std_logic_1164.ALL;
USE IEEE.numeric_std.ALL;

ENTITY adder4 IS

    PORT (
        aIN1 : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
        aIN2 : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
        aOUT : OUT STD_LOGIC_VECTOR(3 DOWNTO 0);
        cout : OUT STD_LOGIC
    );

END adder4;

ARCHITECTURE structural OF adder4 IS

    COMPONENT full_adder IS
        PORT (
            a : IN STD_LOGIC;
            b : IN STD_LOGIC;
            cin : IN STD_LOGIC; -- cin for input carry
            cout : OUT STD_LOGIC; -- cout for output carry
            s : OUT STD_LOGIC -- s for sum
        );
    end component;
        SIGNAL Sum : STD_LOGIC_VECTOR(3 DOWNTO 0);
        SIGNAL Carry : STD_LOGIC_VECTOR(3 DOWNTO 0);
    BEGIN

        a1 : full_adder
        PORT MAP(aIN1(0), aIN2(0),'0', Carry(0), Sum(0));
        a2 : full_adder
        PORT MAP(aIN1(1), aIN2(1), Carry(0), Carry(1), Sum(1));
        a3 : full_adder
        PORT MAP(aIN1(2), aIN2(2), Carry(1), Carry(2), Sum(2));
        a4 : full_adder
        PORT MAP(aIN1(3), aIN2(3), Carry(2), cout, Sum(3));

        aOUT(3)<= Sum(3);
        aOUT(2)<= Sum(2);
        aOUT(1)<= Sum(1);
        aOUT(0)<= Sum(0);

END structural; -- structural