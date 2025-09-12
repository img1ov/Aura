// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

#include "AuraGameplayTags.generated.h"

/**
 * 
 */

USTRUCT()
struct FAuraGameplayTagStatics
{
	GENERATED_BODY()

private:
	TMap<FGameplayTag, FGameplayTag> DamageTypeToResistanceMap;

public:
	static FAuraGameplayTagStatics* Get()
	{
		static FAuraGameplayTagStatics* AuraGameplayTagStaticInstance;
		if (!AuraGameplayTagStaticInstance)
		{
			AuraGameplayTagStaticInstance = new FAuraGameplayTagStatics();
		}
		return AuraGameplayTagStaticInstance;
	}

	uint8 RegisterDamageTypeToResistance(FGameplayTag DamageType, FGameplayTag Resistance)
	{
		if (!DamageTypeToResistanceMap.Contains(DamageType))
		{
			DamageTypeToResistanceMap.Add(DamageType, Resistance);
			return true;
		}

		return false;
	}

	FGameplayTag GetResistanceTypeWithDamageType(FGameplayTag DamageType)
	{
		if (DamageTypeToResistanceMap.Contains(DamageType))
		{
			return DamageTypeToResistanceMap[DamageType];
		}

		return FGameplayTag();
	}
};

/** Primary Attributes */

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Strength)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Intelligence)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Resilience)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Vigor)

/** Secondary Attributes */

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_Armor)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_ArmorPenetration)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_BlockChance)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_CriticalChance)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_CriticalHitDamage)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_CriticalHitResistance)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_HealthRegeneration)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_ManaRegeneration)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_MaxHealth)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_MaxMana)

/** Resistance Attributes */

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Physical)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Arcane)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Fire)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Lightning)

/** Damage */

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effects_Damage)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effects_Damage_Physical)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effects_Damage_Arcane)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effects_Damage_Fire)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effects_Damage_Lightning)

/** Hit React */

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effects_HitReact)


/** Input Tags */

UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_LMB)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_RMB)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_1)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_2)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_3)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_4)