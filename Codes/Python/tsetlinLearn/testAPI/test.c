#include<Python/Python.h>

int main() {
    Py_Initialize();  // 初始化Python解释器

    // 调用Python的print函数
    PyRun_SimpleString("print('Hello from Python!')");

    Py_Finalize();    // 关闭Python解释器
    return 0;
}
