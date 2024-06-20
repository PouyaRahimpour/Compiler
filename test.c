// Function with incorrect return type
char greet() {
    printf("Hello, world!\n");
    return "Hello";  // Error: Cannot return a string literal
}


int main() {
    // Variable declarations
    int x, y;
    char z;
    int arr[10];  // Correct array declaration

    // Multiple variables of the same type separated by comma
    int a, b, c;  // Correct multiple variables of the same type

    // Incorrect variable declaration
    //int d, e, f,;  // Error: Extra comma

    // Conditional statements
    x = 10;
    y = 5;


    // Incorrect format specifier for string
    char str[] = "Hello, world!";
    printf("String: %d\n", str);  // Error: Incorrect format specifier for string

    // Function calls with errors
    greet();
    printNumber('5');  // Error: Character '5' instead of integer

    return 0;
}
