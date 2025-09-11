// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag,
	bool bLogNotFound) const
{
	for (const FAuraInputAction& AuraInputAction : AbilityInputActions)
	{
		return AuraInputAction.InputAction && AuraInputAction.InputTag == InputTag ? AuraInputAction.InputAction : nullptr;
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can not find AbilityInputAction for InputTag [%s], on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
