float in_global = 0.0;

int main()
{
    int local = 2;
    {
        int deepscope;
        local = 3;
    }
    printf("Hello, world!");
    return 0;
}