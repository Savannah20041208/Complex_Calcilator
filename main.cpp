#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100

#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASIBLE -1
#define OVERFLOW -2

// 定义复数结构体
typedef struct {
    double real;
    double imag;
} Complex;

// 定义栈结构体
typedef struct {
    Complex items[MAX];
    int top;
} Stack;

// 初始化栈
void initStack(Stack *s) {
    s->top = -1; // 栈顶初始化为 -1，表示空栈
}

// 判断栈是否为空
int isEmpty(Stack *s) {
    return s->top == -1;
}

// 判断栈是否满
int isFull(Stack *s) {
    return s->top == MAX - 1;
}

// 返回栈顶元素
Complex peek(Stack*s) {
    if(isEmpty(s)) {
        Complex empty = {0, 0}; // 返回一个零复数
        return empty;
    }
    return s->items[s->top];// 返回栈顶元素
}

// 压栈
void push(Stack *s, Complex value) {
    if (!isFull(s)) {
        s->items[++s->top] = value;// 栈顶指针加1后存入复数
    }
}

// 弹栈
Complex pop(Stack *s) {
    if (!isEmpty(s)) {
        return s->items[s->top--];
    }
    Complex empty = {0, 0};
    return empty; // 空栈时返回0
}

// 运算符优先级

int precedence(char op) {
    switch (op) {
        case '+': case '-': return 1;
        case '*': case '/': return 2;
        case '(': return 0;
        default: return -1;
    }

}

// 进行复数加法
Complex add(Complex a, Complex b) {
    Complex result = {a.real + b.real, a.imag + b.imag};
    return result;
}

// 进行复数减法
Complex subtract(Complex a, Complex b) {
    Complex result = {a.real - b.real, a.imag - b.imag};
    return result;
}

// 进行复数乘法
Complex multiply(Complex a, Complex b) {
    Complex result = {
        a.real * b.real - a.imag * b.imag,
        a.real * b.imag + a.imag * b.real
    };
    return result;
}

// 进行复数除法
Complex divide(Complex a, Complex b) {
    double denominator = b.real * b.real + b.imag * b.imag;
    Complex result{
        result.real = (a.real * b.real + a.imag * b.imag) / denominator,// 实部
        result.imag = (a.imag * b.real - a.real * b.imag) / denominator // 虚部
    };
    if (denominator == 0) {
        printf("Division by zero error\n");
        exit(1);
    }
    return result;
}
// 解析复数
Complex parseComplex(char *token) {
    Complex result;
    int front = 0;
    int back = 0;
    // 检查复数字符串中的正负号位置
    for(int i = 0; i < strlen(token); ++i) {
        if(token[i] == '-' && i == 0) { // 负号位于最前面
            front = 1;
        }
        if(token[i] == '-' && i != 0) { // 负号位于中间
            back = 1;
        }
    }
    if(front == 1 && back == 0) {
        sscanf(token, "-%lf+%lfi", &result.real, &result.imag);
        result.real = -result.real;
    } else if (front == 0 && back == 1) {
        sscanf(token, "%lf-%lfi", &result.real, &result.imag);
        result.imag = -result.imag;
    } else if (front == 1 && back == 1) {
        sscanf(token, "-%lf-%lfi", &result.real, &result.imag);
        result.real = -result.real;
        result.imag = -result.imag;
    } else
        sscanf(token, "%lf+%lfi", &result.real, &result.imag);
    return result;
}

// 检查是否是复数（简单判断格式）
int isComplex(char *token) {
    return strchr(token, 'i') != NULL;//用strchr查看字符串中是否有i
}

void printComplex(Complex);

// 计算后缀表达式
Complex evaluatePostfix(char postfix[][MAX], int length) {
    Stack s;
    initStack(&s);
    for (int i = 0; i < length; i++) {
        if(postfix[i][0] == 0) continue; // 跳过空白符
        if (isComplex(postfix[i])) {
            Complex num = parseComplex(postfix[i]); // 解析复数
            push(&s, num);
        } else {
            Complex b = pop(&s);
            Complex a = pop(&s);
            Complex result;
            switch (postfix[i][0]) {
                case '+': result = add(a, b); break;
                case '-': result = subtract(a, b); break;
                case '*': result = multiply(a, b); break;
                case '/': result = divide(a, b); break;
            }
            printComplex(result);
            push(&s, result);
        }
    }
    return pop(&s);
}

// 将中缀表达式转换为后缀表达式   输入中缀表达式   存储转换后的后缀表达式，存储复数和运算符
int infixToPostfix(char *infix, char postfix[][MAX]) {
    Stack s;
    initStack(&s);
    char token[MAX];//用来临时存放当前正在处理的复数或运算符
    int k = 0, j = 0;//k 是后缀表达式 postfix 中的索引，表示存储位置
    //j 是 token 中的索引，表示复数或运算符的当前字符位置

    for (int i = 0; i <= strlen(infix); i++) {
        if (isdigit(infix[i]) || (infix[i] == '+' && j > 0) || infix[i] == '-' || infix[i] == 'i') {
            token[j++] = infix[i]; // 继续构建复数
        } else {
            if (j > 0) {
                token[j] = '\0'; // 完成一个复数或运算符
                printf("token: %s\n", token);
                strcpy(postfix[k++], token);// 存储复数
                j = 0;//重置token的索引
            }

            if (infix[i] == ' ') {
                continue; // 忽略空格
            } else if (infix[i] == '(') {
                Complex temp = {'(', 0};
                push(&s, temp); // '(' 直接压入栈
            } else if (infix[i] == ')') {
                while (!isEmpty(&s) && peek(&s).real != '(') {
                    Complex op = pop(&s);
                    postfix[k][0] = (char)op.real;
                    postfix[k++][1] = '\0';
                }
                pop(&s); // 弹出 '('
            } else {               //当前
                while (!isEmpty(&s) && precedence(infix[i]) <= precedence((char)peek(&s).real)) {
                    Complex op = pop(&s);
                    postfix[k][0] = (char)op.real;
                    postfix[k++][1] = '\0';
                }
                Complex op = {(double)infix[i], 0.0};
                if(infix[i] != 0)
                push(&s, op);
            }
        }
    }

    if (j > 0) { // 处理最后的复数
        token[j] = '\0';
        strcpy(postfix[k++], token);
    }

    while (!isEmpty(&s)) {
        Complex op = pop(&s);
        if(op.real != 0) {
            postfix[k][0] = (char)op.real;
            postfix[k++][1] = '\0';
        }

    }

    for(int i = 0; i < k; ++i) {
        printf("%s\n", postfix[i]);
    }

    return k;
}

// 输出复数
void printComplex(Complex c) {
    if (c.imag >= 0)
        printf("%.2f + %.2fi\n", c.real, c.imag);
    else
        printf("%.2f - %.2fi\n", c.real, -c.imag);
}

int main() {
    // 使用1维字符数组来存储表达式
    system("chcp 65001");
    char infix[MAX]; // 声明一个字符数组用于存储中缀表达式

    // 提示用户输入中缀表达式
    printf("请输入一个中缀表达式 (例如: 4-3i * -3-3i): ");

    // 使用scanf获取用户输入，限制读取的字符数以避免缓冲区溢出
    scanf("%99[^\n]", infix); // 读取输入直到遇到换行符，最多读取99个字符

    char postfix[MAX][MAX];

    int postfixLength = infixToPostfix(infix, postfix);
    Complex result = evaluatePostfix(postfix, postfixLength);

    printf("结果：");
    printComplex(result);

    return 0;
}
