
typedef const uint NewType0;
typedef const uint**** NewType1[5];
typedef NewType0* (*NewType2)[2][3];

typedef struct struct_t {
    int x, y, z;
    char string[128];
} MyStruct;

typedef enum enum_t {
    RED=0,
    GREEN,
    BLUE
} MyEnum;

typedef union union_t {
    int x;
    bool b;
} Onion;