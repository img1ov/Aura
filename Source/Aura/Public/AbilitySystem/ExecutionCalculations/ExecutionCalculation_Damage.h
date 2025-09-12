// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "ExecutionCalculation_Damage.generated.h"

USTRUCT()
struct FAttributeCaptureStatics
{
	GENERATED_BODY()

public:
	static FAttributeCaptureStatics* Get()
	{
		static FAttributeCaptureStatics* CaptureStaticsInstance;
		if (!CaptureStaticsInstance)
		{
			CaptureStaticsInstance = new FAttributeCaptureStatics();
		}

		return CaptureStaticsInstance;
	}
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration)
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalChance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance)
	
	FAttributeCaptureStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalChance, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false)
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
	}

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
