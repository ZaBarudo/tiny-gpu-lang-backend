int callerFunc(int a, int b){
    return a+b+1;
}

int main() {
    int a = 1;
    int b = 2;
    int c = callerFunc(a, b);
    int d = 2;
    int e = 2;
    return 0;
}