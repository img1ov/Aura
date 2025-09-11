#pragma once

#include "GameplayEffectTypes.h"

#include "AuraAbilityTypes.generated.h"

USTRUCT()
struct FAuraGameplayEffectContext : public FGameplayEffectContext{
	GENERATED_BODY()

public:
	virtual UScriptStruct* GetScriptStruct() const override;
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
	virtual  FAuraGameplayEffectContext* Duplicate() const override;

	bool GetIsBlockedHit() const { return bIsBlockedHit; }
	bool GetIsCriticalHit() const { return bIsCriticalHit; }

	void SetIsBlockedHit(const bool InIsBlockedHit) { bIsBlockedHit = InIsBlockedHit; }
	void SetIsCriticalHit(const bool InIsCriticalHit){ bIsCriticalHit = InIsCriticalHit; }
	
protected:
	UPROPERTY()
	bool bIsBlockedHit = false;

	UPROPERTY()
	bool bIsCriticalHit = false;
};

template<>
struct TStructOpsTypeTraits< FAuraGameplayEffectContext > : public TStructOpsTypeTraitsBase2< FAuraGameplayEffectContext >
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};