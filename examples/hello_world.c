void sayHelloWorld() {
    syscall(1, 1, "Hello, World!\n");
}

int main(int argc, char** argv) {
    sayHelloWorld();
    return 0;
}