int in_global = 6;

int junk(int passed_along){
    int in_function;
    in_function = 1 + passed_along;
    return in_global + in_function;
}

int main()
{
    char[3] str;
    str[0] = '~';
    str[1] = '7';
    str[2] = '\n';
    
    int local = in_global*(1 + 5);
    int result = junk(local);

    printf("Hello, number %d! Write a new number: ", result);
    scanf("%d",&result);
    printf("You've written '%d'\n",result);
    printf("Also %s is in char array",&str[0]);

    return 0;
}