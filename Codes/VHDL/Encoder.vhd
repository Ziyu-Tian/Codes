LIBRARY IEEE;
USE IEEE.std_logic_1164.ALL;

ENTITY pri_enc IS
    PORT (
        a : IN STD_LOGIC;
        b : IN STD_LOGIC;
        c : IN STD_LOGIC;
        d : IN STD_LOGIC;
        e : IN STD_LOGIC;
        f : IN STD_LOGIC;
        f0 : OUT STD_LOGIC;
        f1 : OUT STD_LOGIC;
        f2 : OUT STD_LOGIC
    );
END pri_enc;

ARCHITECTURE rtl OF pri_enc IS
    SIGNAL enc_in : STD_LOGIC_VECTOR(0 TO 5);
    SIGNAL enc_out : STD_LOGIC_VECTOR(0 TO 2);
BEGIN
    enc_in <= a & b & c & d & e & f;

    WITH enc_in SELECT
        enc_out <= "000" WHEN "000000",
        "001" WHEN "000001",
        "010" WHEN "00001-",
        "011" WHEN "0001--",
        "100" WHEN "001---",
        "101" WHEN "01----",
        "110" WHEN "1-----",
        "---" when others;

    f0 <= enc_out(0);
    f1 <= enc_out(1);
    f2 <= enc_out(2);
END rtl;