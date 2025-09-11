// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagnitudeCalculation/MMC_MaxMana.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxMana::UMMC_MaxMana()
{
	IntelligenceCaptureDef.AttributeToCapture = UAuraAttributeSet::GetIntelligenceAttribute();
	IntelligenceCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IntelligenceCaptureDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(IntelligenceCaptureDef);
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float IntelligenceMagnitude = 0.f;
	GetCapturedAttributeMagnitude(IntelligenceCaptureDef, Spec, EvaluateParameters, IntelligenceMagnitude);
	IntelligenceMagnitude = FMath::Max(IntelligenceMagnitude, 0.f);
	
	if (const ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject()))
	{
		int32 ActorLevel = CombatInterface->GetActorLevel();
		return 50.f + 2.5f * IntelligenceMagnitude + 15.f * ActorLevel;
	}
	
	return Super::CalculateBaseMagnitude_Implementation(Spec);
}
