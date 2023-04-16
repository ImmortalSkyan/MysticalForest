// Fill out your copyright notice in the Description page of Project Settings.


#include "Singleton/BaseSingleton.h"

UBaseSingleton& UBaseSingleton::Get()
{
    UBaseSingleton* Singleton = Cast<UBaseSingleton>(GEngine->GameSingleton);
    return Singleton ? *Singleton : *NewObject<UBaseSingleton>(StaticClass());
}
