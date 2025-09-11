// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagnitudeCalculation/MMC_MaxHealth.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigorCaptureDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	VigorCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorCaptureDef.bSnapshot = false;

	//在Spec构造时捕获, 捕获在GESpec构造时的参数，以该参数为权威做计算，防止后续因为外部因素导致该Attribute被更改
	RelevantAttributesToCapture.Add(VigorCaptureDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters AggregatorEvaluateParameters;
	AggregatorEvaluateParameters.SourceTags = SourceTags;
	AggregatorEvaluateParameters.TargetTags = TargetTags;

	//获取构造时捕获的值
	float VigorMagnitude = 0.f;
	GetCapturedAttributeMagnitude(VigorCaptureDef, Spec, AggregatorEvaluateParameters, VigorMagnitude);
	VigorMagnitude = FMath::Max(VigorMagnitude, 0);

	if (const ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject()))
	{
		const int32 ActorLevel = CombatInterface->GetActorLevel();
		return 80.f + 2.f * VigorMagnitude * ActorLevel;
	}
	
	return Super::CalculateBaseMagnitude_Implementation(Spec);
}
