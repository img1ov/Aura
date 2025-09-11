// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecutionCalculations/ExecutionCalculation_Damage.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemBPLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "GameplayTags/AuraGameplayTags.h"
#include "Interaction/CombatInterface.h"

UExecutionCalculation_Damage::UExecutionCalculation_Damage()
{
	// Register RelevantAttributesToCapture
	RelevantAttributesToCapture.Add(GetDamageCaptureStatics().ArmorDef);
	RelevantAttributesToCapture.Add(GetDamageCaptureStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(GetDamageCaptureStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(GetDamageCaptureStatics().CriticalChanceDef);
	RelevantAttributesToCapture.Add(GetDamageCaptureStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(GetDamageCaptureStatics().CriticalHitResistanceDef);
}

void UExecutionCalculation_Damage::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// Generate Params
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC->GetAvatarActor();
	AActor* TargetAvatar = TargetASC->GetAvatarActor();

	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

	/** Calculation Space */
	
	// Get Damage Set by Caller Magnitude <EffectSpec::SetSetByCaller...>
	float IncomingDamageMag = EffectSpec.GetSetByCallerMagnitude(Effects_Damage);

	// Capture BlockChance on Target, and determine if there was a successful Block
	float TargetBlockChanceMag = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCaptureStatics().BlockChanceDef, EvaluateParameters, TargetBlockChanceMag);
	TargetBlockChanceMag = FMath::Max<float>(TargetBlockChanceMag, 0.f);

	// If Block, half the Damage
	const bool bBlock = FMath::RandRange(1, 100) < TargetBlockChanceMag;
	IncomingDamageMag = bBlock ? IncomingDamageMag * .5f : IncomingDamageMag;

	// Capture Armor on Target
	float TargetArmorMag = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCaptureStatics().ArmorDef, EvaluateParameters, TargetArmorMag);
	TargetArmorMag = FMath::Max<float>(TargetArmorMag, 0.f);
	
	// Capture Source ArmorPenetration on Source
	float SourceArmorPenetrationMag = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCaptureStatics().ArmorPenetrationDef, EvaluateParameters, SourceArmorPenetrationMag);
	SourceArmorPenetrationMag = FMath::Max<float>(SourceArmorPenetrationMag, 0.f);

	// Get DamageCalculationCoefficients
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemBPLibrary::GetCharacterClassInfo(TargetAvatar);
	const UCurveTable* DamageCalculationCoefficients = CharacterClassInfo->DamageCalculationCoefficients;
	check(DamageCalculationCoefficients)
	
	// ArmorPenetrationCoefficient from CurveTable
	const FRealCurve* ArmorPenetrationCurve = DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	check(ArmorPenetrationCurve)
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetActorLevel());

	// EffectiveArmorCoefficient from CurveTable
	const FRealCurve* EffectiveArmorCurve = DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	check(EffectiveArmorCurve)
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetActorLevel());
	
	// ArmorPenetration ignores a percentage of the Target's Armor
	const float EffectiveArmor = TargetArmorMag * (100 - SourceArmorPenetrationMag * ArmorPenetrationCoefficient) / 100.f;

	// Armor ignores a percentage of incoming Damage
	IncomingDamageMag *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;
	
	// Capture Critical Attributes
	float SourcesCriticalChanceMag = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCaptureStatics().CriticalChanceDef, EvaluateParameters, SourcesCriticalChanceMag);
	SourcesCriticalChanceMag = FMath::Max<float>(SourcesCriticalChanceMag, 0.f);
	
	float SourcesCriticalHitDamageMag = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCaptureStatics().CriticalHitDamageDef, EvaluateParameters, SourcesCriticalHitDamageMag);
	SourcesCriticalHitDamageMag = FMath::Max<float>(SourcesCriticalHitDamageMag, 0.f);

	float TargetCriticalHitResistanceMag = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCaptureStatics().CriticalChanceDef, EvaluateParameters, TargetCriticalHitResistanceMag);
	TargetCriticalHitResistanceMag = FMath::Max<float>(TargetCriticalHitResistanceMag, 0.f);

	// CriticalHitResistanceCoefficient from CurveTable
	const FRealCurve* CriticalHitResistanceCurve = DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	check(CriticalHitResistanceCurve)
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetCombatInterface->GetActorLevel());
	
	// CriticalHitResistance reduces CriticalHitChance by a certain percentage
	const float EffectiveCriticalHitChance = SourcesCriticalChanceMag - TargetCriticalHitResistanceMag * CriticalHitResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(1, 100) <= EffectiveCriticalHitChance;

	// Double damage plus a bonus if critical hit
	IncomingDamageMag = bCriticalHit ? 2.f * IncomingDamageMag + SourcesCriticalHitDamageMag : IncomingDamageMag;
	
	/** End Calculation Space */

	// Apply IncomingDamage
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, IncomingDamageMag);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
