#include <stdio.h>
#include <stdbool.h>
int test_function ( int a , int b , bool c ) { if ( c == true ) { return a + b ; } else { return a - b ; } } int main ( ) { bool add = true ; char _assign1 = '=' ; char String_1 [ ] = " + " ; char String_2 [ ] = " - " ; int num1 = 10 ; int num2 = 5 ; int _result = 0 ; for ( int i = 0 ; i <= num1 / 2 ; i = i + 1 ) { for ( int j = 0 ; j != num2 * - 1 ; j = j - 1 ) { _result = i + j ; printf ( "Result: " , _result ) ; } } add = false ; for ( int i = 0 ; ! ( i == 5 ) ; i = i + 1 ) { for ( int j = 0 ; j >= - 5 ; j = j - 1 ) { if ( ( i % 4 ) == 0 || ( i % 3 ) == 0 ) continue ; if ( j < - 4 && i > 3 ) break ; _result = i - j ; printf ( "Result: " , _result ) ; } } printf ( "Test string with escaped characters: " ) ; printf ( "\\n newline, \\t tab, \\r carriage return." ) ; printf ( "this is\" a whole string no other token like '=' or 'else' or even \\\\comment should be recognized\n" ) ; char back = '\\' ; char quote = '\'' ; int _123 = 0XABCdef12 ; return 0 ; } 