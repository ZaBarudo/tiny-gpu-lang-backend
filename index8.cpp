int callerFunc(int a, int b){
    return a+b+1;
}

int main() {
    int a = 1;
    int b = 2;
    int c = callerFunc(a, b);
    return 0;
}