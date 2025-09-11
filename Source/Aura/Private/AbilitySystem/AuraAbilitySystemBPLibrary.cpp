// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemBPLibrary.h"

#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

UOverlayWidgetController* UAuraAbilitySystemBPLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* HUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			return HUD->GetOverlayWidgetController(FWidgetControllerParams(PC, PS, ASC, AS));
		}
	}

	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemBPLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* HUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			return HUD->GetAttributeMenuWidgetController(FWidgetControllerParams(PC, PS, ASC, AS));
		}
	}
	
	return nullptr;
}

void UAuraAbilitySystemBPLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,
	const ECharacterClass CharacterClass, const int32 ActorLevel, UAbilitySystemComponent* SourceASC)
{
	if (AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		check(AuraGameModeBase->CharacterClassInfo)
		UCharacterClassInfo* CharacterClassInfo = AuraGameModeBase->CharacterClassInfo;
		
		FCharacterClassDefaultInfo CharacterClassDefaultInfo = CharacterClassInfo->GetCharacterClassDefaultInfo(CharacterClass);

		if (SourceASC && CharacterClassDefaultInfo.PrimaryAttributes)
		{
			check(CharacterClassDefaultInfo.PrimaryAttributes)
			check(CharacterClassInfo->SecondaryAttributes)
			check(CharacterClassInfo->VitalAttributes)

			FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
			EffectContextHandle.AddSourceObject(SourceASC->GetAvatarActor());
			EffectContextHandle.AddInstigator(SourceASC->GetAvatarActor(), SourceASC->GetAvatarActor());
				
			FGameplayEffectSpecHandle PrimaryEffectSpecHandle =
				SourceASC->MakeOutgoingSpec(CharacterClassDefaultInfo.PrimaryAttributes, ActorLevel, EffectContextHandle);
			SourceASC->ApplyGameplayEffectSpecToSelf(*PrimaryEffectSpecHandle.Data.Get());

			FGameplayEffectSpecHandle SecondaryEffectSpecHandle =
				SourceASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, ActorLevel, EffectContextHandle);
			SourceASC->ApplyGameplayEffectSpecToSelf(*SecondaryEffectSpecHandle.Data.Get());

			FGameplayEffectSpecHandle VitalEffectSpecHandle =
				SourceASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, ActorLevel, EffectContextHandle);
			SourceASC->ApplyGameplayEffectSpecToSelf(*VitalEffectSpecHandle.Data.Get());
		}
	}
}

void UAuraAbilitySystemBPLibrary::GiveStartupAbilities(const UObject* WorldContextObject,
	UAbilitySystemComponent* SourcesASC)
{
	if (UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject))
	{
		for (const TSubclassOf<UGameplayAbility> CommonAbilityClass : CharacterClassInfo->CommonAbilities)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(CommonAbilityClass, 1);
			SourcesASC->GiveAbility(AbilitySpec);
		}
	}
}

UCharacterClassInfo* UAuraAbilitySystemBPLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	if (AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return AuraGameModeBase->CharacterClassInfo;
	}

	return nullptr;
}
