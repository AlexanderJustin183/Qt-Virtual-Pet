//
// Created by AlexanderJustin183 on 2024/1/4.
//

#ifndef VIRTUALPET_PET_H
#define VIRTUALPET_PET_H


class Pet
{
public:
    bool eating, sleeping, walking, playing, doctor;
    int tc;  //tc = time-cycle
    int hunger, happiness, health;
    bool forceAwake;
    Pet();
};


#endif //VIRTUALPET_PET_H
