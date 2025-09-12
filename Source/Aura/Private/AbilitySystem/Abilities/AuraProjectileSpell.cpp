// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	
}

AAuraProjectile* UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	// Only spawn on server
	if (GetAvatarActorFromActorInfo()->HasAuthority())
	{
		check(ProjectileClass)
		
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo())){

			const FVector WeaponTipSocketLocation = CombatInterface->GetCombatSocketLocation();
			FRotator ProjectileRotation = (ProjectileTargetLocation - WeaponTipSocketLocation).Rotation();
			
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(WeaponTipSocketLocation);
			SpawnTransform.SetRotation(ProjectileRotation.Quaternion());
			
			AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
				ProjectileClass,
				SpawnTransform,
				GetAvatarActorFromActorInfo(),
				Cast<APawn>(GetOwningActorFromActorInfo()),
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			
			if (UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo()))
			{
				check(DamageEffectClass)
				
				FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
				EffectContextHandle.AddSourceObject(Projectile);
				EffectContextHandle.SetAbility(this);
				
				EffectContextHandle.AddInstigator(GetOwningActorFromActorInfo(), GetAvatarActorFromActorInfo());
				FGameplayEffectSpecHandle EffectSpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
				UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
					EffectSpecHandle, Damage.DamageType, Damage.DamageScalableFloat.GetValueAtLevel(GetAbilityLevel()));
				
				Projectile->DamageEffectSpecHandle = EffectSpecHandle;
			}
			
			Projectile->FinishSpawning(SpawnTransform);

			return Projectile;
		}
	}

	return nullptr;
}
