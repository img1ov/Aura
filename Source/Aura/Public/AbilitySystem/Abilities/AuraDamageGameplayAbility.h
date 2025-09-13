// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "GameplayTags/AuraGameplayTags.h"
#include "AuraDamageGameplayAbility.generated.h"

USTRUCT(BlueprintType)
struct FDamageContext
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DamageType = Effects_Damage_Physical;
	
	UPROPERTY(EditDefaultsOnly)
	FScalableFloat DamageScalableFloat;
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	FDamageContext Damage;
};
