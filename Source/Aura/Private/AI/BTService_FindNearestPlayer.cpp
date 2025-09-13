// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const FName TargetTag = AIOwner->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");
	TArray<AActor*> ActorsOfTargetTag;
	UGameplayStatics::GetAllActorsWithTag(AIOwner, TargetTag, ActorsOfTargetTag);

	if (!ActorsOfTargetTag.IsEmpty())
	{
		float Distance = UBTFunctionLibrary::GetBlackboardValueAsFloat(this, DistanceToFindNearestTargetSelector);
		if(AActor* NearestActor = UGameplayStatics::FindNearestActor(AIOwner->GetPawn()->GetActorLocation(), ActorsOfTargetTag, Distance))
		{
			UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, NearestActor);
			GEngine->AddOnScreenDebugMessage(-1, .5f, FColor::Orange, *NearestActor->GetName());
		}
	}
}
