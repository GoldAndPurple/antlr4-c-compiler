int in_global = 6;

int print(char* str){
    return 0;
}

int main()
{
    int local = in_global*(1 + 5);
    {
        local = 3;
    }
    if (local > 0){
        print("Hello, world!");
    }
    return 0;
}