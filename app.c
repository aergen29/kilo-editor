#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include <errno.h>


/** Data **/
struct termios orgi_termios;


/** Terminal **/
void die(const char *s) // Error handling
{
    perror(s);
    exit(1);
}

void disableRawMode()
{ // Disable raw mode at exit
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orgi_termios) == -1) // Error handling
    {
        die("tcsetattr");
    }
}


void enableRawMode()
{
    if(tcgetattr(STDIN_FILENO,&orgi_termios) == -1) die("tcgetattr"); // Error handling
    tcgetattr(STDIN_FILENO, &orgi_termios);
    atexit(disableRawMode); // Disable raw mode at exit
    struct termios raw = orgi_termios;

    raw.c_cflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); // Fix Ctrl-M // Disable Ctrl-S and Ctrl-Q // Miscellaneous flags
    raw.c_cflag &= ~(OPOST);                                  // Turn off all output processing
    raw.c_cflag |= ~(CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); // Turn off canonical mode  // Disable Ctrl-V  // Turn off Ctrl-C and Ctrl-Z signals
    raw.c_cc[VMIN] = 0;                              // A timeout for read()
    raw.c_cc[VTIME] = 1;
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr"); // Error handling
}


/**  init **/
int main()
{
    enableRawMode();
    printf("Hello World\n");

    while (1) // entering raw mode and Press q to quit?
    {
        char c = '\0';
        if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read"); // A timeout for read() //Error handling
        if (iscntrl(c))            // Display keypresses
        {
            printf("%d\r\n", c);
        }
        else
        {
            printf("%d('%c')\r\n", c, c);
        }
        if (c == 'q')
            break;
    }
    return 0;
}