float in_global = 0.0;

int main()
{
    int local = in_global*(1 + 5);
    {
        local = 3;
    }
    if (local > 0){
        printf("Hello, world!");
    }
    return 0;
}