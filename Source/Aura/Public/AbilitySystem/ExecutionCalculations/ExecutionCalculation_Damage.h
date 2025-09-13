// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "GameplayTags/AuraGameplayTags.h"
#include "ExecutionCalculation_Damage.generated.h"

USTRUCT()
struct FAttributeCaptureStatics
{
	GENERATED_BODY()

public:
	static FAttributeCaptureStatics* Get()
	{
		static FAttributeCaptureStatics* AttributeCaptureStaticInstance;
		if (!AttributeCaptureStaticInstance)
		{
			AttributeCaptureStaticInstance = new FAttributeCaptureStatics();
		}

		return AttributeCaptureStaticInstance;
	}

	UPROPERTY()
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagToCaptureDefMap;
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration)
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalChance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance)
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance)
	
	FAttributeCaptureStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalChance, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false)
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false)

		TagToCaptureDefMap.Add(Attributes_Secondary_Armor, ArmorDef);
		TagToCaptureDefMap.Add(Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagToCaptureDefMap.Add(Attributes_Secondary_BlockChance, BlockChanceDef);
		TagToCaptureDefMap.Add(Attributes_Secondary_CriticalChance, CriticalChanceDef);
		TagToCaptureDefMap.Add(Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);
		TagToCaptureDefMap.Add(Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);
		
		TagToCaptureDefMap.Add(Attributes_Resistance_Physical, PhysicalResistanceDef);
		TagToCaptureDefMap.Add(Attributes_Resistance_Arcane, ArcaneResistanceDef);
		TagToCaptureDefMap.Add(Attributes_Resistance_Fire, FireResistanceDef);
		TagToCaptureDefMap.Add(Attributes_Resistance_Lightning, LightningResistanceDef);
	}
};

/**
 * 
 */
UCLASS()
class AURA_API UExecutionCalculation_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecutionCalculation_Damage()
	{
		RelevantAttributesToCapture.Add(FAttributeCaptureStatics::Get()->ArmorDef);
		RelevantAttributesToCapture.Add(FAttributeCaptureStatics::Get()->ArmorPenetrationDef);
		RelevantAttributesToCapture.Add(FAttributeCaptureStatics::Get()->BlockChanceDef);
		RelevantAttributesToCapture.Add(FAttributeCaptureStatics::Get()->CriticalChanceDef);
		RelevantAttributesToCapture.Add(FAttributeCaptureStatics::Get()->CriticalHitDamageDef);
		RelevantAttributesToCapture.Add(FAttributeCaptureStatics::Get()->CriticalHitResistanceDef);
		
		RelevantAttributesToCapture.Add(FAttributeCaptureStatics::Get()->PhysicalResistanceDef);
		RelevantAttributesToCapture.Add(FAttributeCaptureStatics::Get()->ArcaneResistanceDef);
		RelevantAttributesToCapture.Add(FAttributeCaptureStatics::Get()->FireResistanceDef);
		RelevantAttributesToCapture.Add(FAttributeCaptureStatics::Get()->LightningResistanceDef);
	}

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
