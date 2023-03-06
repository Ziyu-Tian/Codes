library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

ENTITY sec_enc IS 
    PORT (
        a : OUT STD_LOGIC;
        b : OUT STD_LOGIC;
        c : OUT STD_LOGIC;
        d : OUT STD_LOGIC;
        e : OUT STD_LOGIC;
        f : OUT STD_LOGIC;
        g : OUT STD_LOGIC;
        h : OUT STD_LOGIC;
        f0 : IN STD_LOGIC;
        f1 : IN STD_LOGIC;
        f2 : IN STD_LOGIC   
    );
END sec_enc;

ARCHITECTURE rtl OF sec_enc IS
    SIGNAL enc_in : STD_LOGIC_VECTOR(0 TO 2);
    SIGNAL enc_out : STD_LOGIC_VECTOR(0 TO 7);
BEGIN
    enc_in <= f0 & f1 & f2;

    WITH enc_in SELECT
        enc_out <= "00000001" WHEN "000",
        "00000010" WHEN "001",
        "00000100" WHEN "010",
        "00001000" WHEN "011",
        "00010000" WHEN "100",
        "00100000" WHEN "101",
        "01000000" WHEN "110",
        "10000000" WHEN "111",
        "--------" when others;

    a <= enc_out(0);
    b <= enc_out(1);
    c <= enc_out(2);
    d <= enc_out(3);
    e <= enc_out(4);
    f <= enc_out(5);
    g <= enc_out(6);
    h <= enc_out(7);
END rtl;


