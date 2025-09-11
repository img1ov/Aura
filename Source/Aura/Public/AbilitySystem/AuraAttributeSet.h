// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "AuraAttributeSet.generated.h"

//typedef is specific to the FGameplayAttribute() signature, but TStaticFuncPtr is generic to any signature chosen.
//typedef TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultDelegateUserPolicy>::FFuncPtr FAttributeFunPtr;
template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;

/** 快捷定义属性访问器 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/** End 快捷定义属性访问器*/

/**
 * 在PostGameplayEffectExecute时存储Effect数据
 */
USTRUCT(BlueprintType)
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties(){}
	
	FEffectProperties(
		const FGameplayEffectContextHandle& InEffectContextHandle,
		
		UAbilitySystemComponent* InSourceASC,
		AActor* InSourceAvatarActor,
		AController* InSourceController,
		ACharacter* InSourceCharacter,

		UAbilitySystemComponent* InTargetASC,
		AActor* InTargetAvatarActor,
		AController* InTargetController,
		ACharacter* InTargetCharacter
		) :
	EffectContextHandle(InEffectContextHandle),
	
	SourceASC(InSourceASC),
	SourceAvatarActor(InSourceAvatarActor),
	SourceController(InSourceController),
	SourceCharacter(InSourceCharacter),
	
	TargetASC(InTargetASC),
	TargetAvatarActor(InTargetAvatarActor),
	TargetController(InTargetController),
	TargetCharacter(InTargetCharacter)
	{}

	FGameplayEffectContextHandle EffectContextHandle;
	
	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	
	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;

	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributeMap; // TStaticFuncPtr<FGameplayAttribute()> = FGameplayAttribute(*)();
	
	/**
	 * Primary Attributes
	 */

	/**力量*/
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category= "Primary Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Strength)

	/**智力*/
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category= "Primary Attributes")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Intelligence)

	/**韧性*/
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resilience, Category= "Primary Attributes")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Resilience)

	/**活力*/
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vigor, Category= "Primary Attributes")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Vigor)

	/**
	 * Secondary Attributes
	 */

	/**护甲*/
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category= "Primary Attributes")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Armor)

	/**护穿*/
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration, Category= "Primary Attributes")
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArmorPenetration)

	/**格挡几率*/
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BlockChance, Category= "Primary Attributes")
	FGameplayAttributeData BlockChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, BlockChance)

	/**暴击几率*/
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalChance, Category= "Primary Attributes")
	FGameplayAttributeData CriticalChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalChance)

	/**暴击伤害*/
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitDamage, Category= "Primary Attributes")
	FGameplayAttributeData CriticalHitDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitDamage)

	/**暴击抗性*/
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitResistance, Category= "Primary Attributes")
	FGameplayAttributeData CriticalHitResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitResistance)

	/**生命再生*/
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegeneration, Category= "Primary Attributes")
	FGameplayAttributeData HealthRegeneration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, HealthRegeneration)

	/**法力再生*/
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegeneration, Category= "Primary Attributes")
	FGameplayAttributeData ManaRegeneration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ManaRegeneration)

	/**最大生命*/
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth)

	/**最大法力*/
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Vital Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana)
	
	/**
	 * Vital Attributes
	 */

	/**生命*/
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health)

	/**法力*/
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;
		ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana)

	/**
	 * Meta Attributes
	 */

	UPROPERTY(BlueprintReadOnly, Category="Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, IncomingDamage);

	
public:
	UAuraAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	/**
	 * OnRep Primary Attributes
	 */
	
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldValue) const;
	
	/**
	 * OnRep Secondary Attributes
	 */

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	void OnRep_CriticalChance(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_ManaRegeneration(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldValue) const;

	/**
	 * OnRep Vital Attributes
	 */
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue) const;
	
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldValue) const;

private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& InEffectProperties);

	void ShowFloatingText(const FEffectProperties& EffectProperties, const float Damage, const bool IsBlockedHit, const bool IsCriticalHit) const;
};

