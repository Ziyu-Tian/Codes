#define LED 3

void setup() {
    // Put your setup code here, to run once.
    // 将你用于初始化的代码放在这里，这些代码只会执行一次。

    pinMode(LED, OUTPUT);   // 将 `LED` 引脚初始化为输出模式
}

void loop() {
    // Put your main code here, to run repeatedly.
    // 将你的业务函数放在这里，这些代码会一直循环的运行。

    digitalWrite(LED, HIGH);    // 将 `LED` 引脚设为 高电平
    sleep(1000);                // 停止 1 秒
    digitalWrite(LED, LOW);     // 将 `LED` 引脚设为 低电平
    sleep(1000);                // 停止 1 秒
}