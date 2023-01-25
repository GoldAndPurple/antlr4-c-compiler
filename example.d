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

    if (result > 20){
        printf("%d is more than 20",result);
    } else {
        printf("%d is less than 20",result);
    }

    int i = 0;
    while (i < 5){
        printf("%d\n",i);
        i = i+1;
    }

    return 0;
}