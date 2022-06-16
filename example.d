int in_global = 6;

int junk(int passed_along){
    int in_function;
    in_function = 1 + passed_along;
    return in_global + in_function;
}

int main()
{
    int local = in_global*(1 + 5);
    int result = junk(local);
    if (result > 0){
        printf("Hello, %d!",result);
    } else {
        printf("You're too small, %d!",result);
    }
    return 0;
}