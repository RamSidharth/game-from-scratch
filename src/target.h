#ifndef TARGET_H
#define TARGET_H

typedef struct
{
    float x;
    float y;
} Target;

void target_spawn(Target* target);

#endif