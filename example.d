int in_global = 6;

int junk(char* str){
    int in_function;
    in_function = 1 + 1;
    return 1 + in_function;
}

int main()
{
    int local = in_global*(1 + 5);
    junk("Hello, world!");
    return 0;
}