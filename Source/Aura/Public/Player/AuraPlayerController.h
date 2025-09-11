// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UDamageTextComponent;
struct FGameplayTag;
struct FInputActionValue;

class UAuraAbilitySystemComponent;
class UAuraInputConfig;
class AAuraEnemy;
class UInputMappingContext;
class UInputAction;
class IEnemyInterface;
class USplineComponent;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY(EditAnywhere, Category="Input|Actions")
	TObjectPtr<UInputAction> MoveInputAction;

	UPROPERTY(EditAnywhere, Category="Input|Actions")
	TObjectPtr<UInputAction> ShiftInputAction;
	bool bShiftKeyPressed;
	
	FHitResult CursorHit;
	
	UPROPERTY()
	TScriptInterface<IEnemyInterface> LastActor;

	UPROPERTY()
	TScriptInterface<IEnemyInterface> ThisActor;

	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.35f;
	bool bAutoRunning = false;
	bool bTargeting = false;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
	
public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(const float DamageAmount, ACharacter* TargetCharacter, const bool IsBlockedHit, const bool IsCriticalHit);
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
private:
	UAuraAbilitySystemComponent* GetAuraGameplayAbilitySystem();

	void Move(const FInputActionValue& InputActionValue);
	void AutoRun();
	
	void CursorTrace();

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	void ShiftPressed() { bShiftKeyPressed = true; }
	void ShiftReleased() { bShiftKeyPressed = false; }

};
