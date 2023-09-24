
typedef const uint NewType0;
typedef const uint**** NewType1[5];
typedef const NewType0* (*NewType2)[2][3];

typedef struct {
    int x, y, z;
    char string[128];
} MyStruct;

typedef enum enum_t {
    RED=0,
    GREEN,
    BLUE
} MyEnum;

typedef union {
    int x;
    bool b;
} Onion;