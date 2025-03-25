int getNumber(int x, int y) {
    return x+y;
}

int callGetNumber() {
    int x = getNumber(32, 42);
    auto c  = 2*x; // Calling getNumber and returning its result
    return c;
}
