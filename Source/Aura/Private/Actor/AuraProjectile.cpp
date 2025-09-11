// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	//生成一个仅和WorldDynamic与Pawn交互发生碰撞交互的Sphere
	SphereComponent = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(SphereComponent);

	SphereComponent->SetCollisionObjectType(ECC_Projectile);
	//Collision仅作用为碰撞触发器，不带理物理模拟
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	//重置所有碰撞通道(碰撞层级)为Ignore
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	//与WorldDynamic碰撞通道的物体发生Overlap时产生触发交互
	SphereComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	//与Pawn碰撞通道的物体发生Overlap时产生触发交互
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovementComponent->InitialSpeed = 550.f;
	ProjectileMovementComponent->MaxSpeed = 550.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	//设置存活时间
	SetLifeSpan(LifeSpan);
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);

	//播放火球移动音效
	if (LoopingSound)
	{
		LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
	}
}

void AAuraProjectile::Destroyed()
{
	//如果由于客户端的不稳定因素导致碰撞检测时机小于服务器，在客户端接收到销毁指令时直接触发HitEffect
	if (!bHit && !HasAuthority())
	{
		PlayHitEffect();
	}
	
	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//防止收到服务器RPC后重复播放
	if (!bHit)
	{
		//处理击中效果
		PlayHitEffect();
	}
	
	//击中后在服务器中移除本身，再同步给各个客户端
	if (HasAuthority())
	{
		//删除前造对目标成伤害效果
		if (DamageEffectSpecHandle.IsValid())
		{
			if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
			}
		}
		
		Destroy();
	}
	//以服务器的销毁为权威，客户端可能由于卡顿或者其他不可预测的因素导致碰撞会过早/过晚产生碰撞交互
	//如果过早产生碰撞交互，在本地处理HitEffect后，先将Hit状态存下来，防止服务器再发送销毁指令到客户端时再次触发一遍HitEffect
	else
	{
		bHit = true;
	}
}

void AAuraProjectile::PlayHitEffect() const
{
	//播放击中特效
	if (ImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	}

	//停止播放火球移动音效
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
	}
	
	//播放击中音效
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), GetActorRotation());
	}
}

