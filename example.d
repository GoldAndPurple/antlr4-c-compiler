int in_global = 6;

int junk(int passed_along){
    int in_function;
    in_function = 1 + passed_along;
    return in_global + in_function;
}

int main()
{
    char[5] str;
    str[0] = 'T';
    str[1] = 'H';
    str[2] = 'I';
    str[3] = 'S';
    str[4] = '\0';
    
    int local = in_global*(1 + 5);
    int result = junk(local);

    printf("Hello, number %d! Write a new number: ", result);
    scanf("%d",&result);
    printf("You've written \n'%d'\n",result);
    printf("Also %s is a char array\n",&str[0]);

    return 0;
}