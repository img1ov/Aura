#pragma once

#include "GameplayEffectTypes.h"

#include "AuraAbilityTypes.generated.h"

USTRUCT()
struct FAuraGameplayContext : public FGameplayEffectContext{
	GENERATED_BODY()

public:
	virtual UScriptStruct* GetScriptStruct() const override;
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
	virtual  FAuraGameplayContext* Duplicate() const override;

	virtual bool GetIsBlockedHit() { return bIsBlockedHit; }
	virtual bool GetIsCriticalHit() { return bIsCriticalHit; }
	
protected:
	UPROPERTY()
	bool bIsBlockedHit = false;

	UPROPERTY()
	bool bIsCriticalHit = false;
};

template<>
struct TStructOpsTypeTraits< FAuraGameplayContext > : public TStructOpsTypeTraitsBase2< FAuraGameplayContext >
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};