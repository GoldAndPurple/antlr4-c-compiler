int in_global = 6;

int junk(int passed_along){
    int in_function;
    in_function = 1 + passed_along;
    return in_global + in_function;
}

int main()
{
    int[3] array;
    array[1] = 1;
    int local = in_global*(1 + 5);
    int result = junk(local);
    printf("Hello, number %d! Write a new number: ",array[1]);
    scanf("%d",&result);
    printf("You've written '%d'",result);
    return 0;
}