// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Character/AuraEnemy.h"
#include "GameplayTagContainer.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Gameplaytags/AuraGameplayTags.h"
#include "UI/Widget/DamageTextComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	AutoRun();
}

void AAuraPlayerController::ShowDamageNumber_Implementation(const float DamageAmount, ACharacter* TargetCharacter, const bool IsBlockedHit, const bool IsCriticalHit)
{
	if (TargetCharacter && IsLocalController())
	{
		check(DamageTextComponentClass)

		UDamageTextComponent* DamageTextComponent = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageTextComponent->RegisterComponent();
		DamageTextComponent->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageTextComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		DamageTextComponent->SetDamageText(DamageAmount, IsBlockedHit, IsCriticalHit);
	}
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(InputMappingContext);
	
	if(UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		SubSystem->AddMappingContext(InputMappingContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	check(AuraInputComponent);
	check(InputConfig);

	AuraInputComponent->BindInputActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
	AuraInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftInputAction, ETriggerEvent::Started, this, &ThisClass::ShiftPressed);
	AuraInputComponent->BindAction(ShiftInputAction, ETriggerEvent::Completed, this, &ThisClass::ShiftReleased);
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraGameplayAbilitySystem()
{
	if (!AuraAbilitySystemComponent)
	{
		AuraAbilitySystemComponent =
			Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}

	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(.0f, Rotation.Yaw, .0f);

	//从Rotator矩阵中取方向，矩阵中X代表Forward，Y代表Right
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	if( APawn* ControlPawn = GetPawn<APawn>() )
	{
		bAutoRunning = false;
		ControlPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::AutoRun()
{
	if (bAutoRunning)
	{
		if (APawn* ControlledPawn = GetPawn())
		{
			//从平滑后的导航路径中获取离自身最近的点
			const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
			//获取导航方向
			const FVector NavDirection = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
			//向目标点移动，CharacterMovementComponent::AddMovementInput拥有网络预测与同步，该做法能让玩家为Client时能够将自身移动任务同步到服务器，
			//玩家为Client时果使用AI导航那套东西，那套本身就是跑在服务器上的，本身不支持网络预测和服务器同步，所以移动可能会失效
			ControlledPawn->AddMovementInput(NavDirection);

			//目标点在自身AutoRunAcceptanceRadius内时停止移动
			const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
			if (DistanceToDestination <= AutoRunAcceptanceRadius)
			{
				bAutoRunning = false;
			}
		}
	}
}

void AAuraPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit)
	{
		return;
	}
	
	LastActor = ThisActor;
	ThisActor = Cast<AAuraEnemy>(CursorHit.GetActor());

	/**
	 * Line trace from cursor, There are several scenarios:
	 *	A. LastActor is null && ThisActor is null
	 *		- Do notiong.
	 *	B. LastActor is null && ThisActor is valid
	 *		- Highlight ThisActor
	 *	C. LastActor is valid && ThisActor is null 
	 *		- UnHighlight LastActor
	 *	D. Both actors are valid, but LastActor != ThisActor
	 *		- UnHighlight LastActor, and Highlight ThisActor
	 *	E. Both actors are valid, and are the same actor
	 *		-Do nothing 
	 */

	if (ThisActor != LastActor)
	{
		if (LastActor)
		{
			LastActor->UnHighlightActor();	
		}
		
		if (ThisActor)
		{
			ThisActor->HighLightActor();
		}
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//点击寻路
	if (InputTag.MatchesTagExact(InputTag_LMB))
	{
		bTargeting = ThisActor != nullptr;
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(InputTag_LMB))
	{
		if (UAuraAbilitySystemComponent* AuraASC = GetAuraGameplayAbilitySystem())
		{
			AuraASC->AbilityInputTagReleased(InputTag);
		}

		return;
	}

	if (UAuraAbilitySystemComponent* AuraASC = GetAuraGameplayAbilitySystem())
	{
		AuraASC->AbilityInputTagReleased(InputTag);
	}
	
	if (!bTargeting && !bShiftKeyPressed)
	{
		//点击左键时 计算寻路路径, 寻路时需要在编辑器中添加Volume/NavMeshBoundsVolume去烘培场景
		//在Cline中使用NavSystem计算路径前需要在ProjectSettings中开启NavigationSystem/AllowClientSideNavigation
		//如果目标点不在NavMesh烘培范围内，也就鼠标击了某个建筑Collision，则会计算出不合法的路径点导致角色一直朝错误的路径点移动，此时需要在建筑Collision中将Trace Responses的Visibility设置为Ignore
		APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			if (UNavigationPath* NavigationPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				//生成寻路路径后使用Spline平滑
				Spline->ClearSplinePoints();
				for (const FVector& PathPoint : NavigationPath->PathPoints)
				{
					Spline->AddSplinePoint(PathPoint, ESplineCoordinateSpace::World);
				}
				//为了防止鼠标点击位置CachedDestination不合法，将他重置为导航路径终点
				if (NavigationPath->PathPoints.Num() > 0)
				{
					CachedDestination = NavigationPath->PathPoints[NavigationPath->PathPoints.Num() -1];
				}
				bAutoRunning = true;
			}
		}
		FollowTime = 0.f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(InputTag_LMB))
	{
		if (UAuraAbilitySystemComponent* AuraASC = GetAuraGameplayAbilitySystem())
		{
			AuraASC->AbilityInputTagHeld(InputTag);
		}

		return;
	}

	//有敌人时按住左键放技能，没有则移动导航
	if (bTargeting || bShiftKeyPressed)
	{
		if (UAuraAbilitySystemComponent* AuraASC = GetAuraGameplayAbilitySystem())
		{
			AuraASC->AbilityInputTagHeld(InputTag);
		}
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();

		//获取导航目标点
		if (CursorHit.bBlockingHit)
		{
			//缓存导航点
			CachedDestination = CursorHit.ImpactPoint;
		}

		if (APawn* ControlPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlPawn->GetActorLocation()).GetSafeNormal();
			ControlPawn->AddMovementInput(WorldDirection);
		}
	}
}

